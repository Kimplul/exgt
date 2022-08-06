/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file dir.c
 * Code browser directory view generator.
 */

#include <html/html.h>
#include <utils/http.h>
#include <utils/chain.h>
#include <utils/path.h>
#include <utils/res.h>
#include <utils/git.h>

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "pages.h"

/**  Directory generator resource manager */
static struct res *r;

/**
 * Generate -rwx- permission string from octal git format.
 * Apparently git only supports parts of the whole UNIX permissions spectrum.
 * https://stackoverflow.com/questions/737673/how-to-read-the-mode-field-of-git-ls-trees-output
 *
 * @param perms Octal permission string.
 * @return -rwx- permission string.
 */
static char *generate_perms(char *perms)
{
	if (strcmp(perms, "040000") == 0)
		return strdup("d---------");

	if (strcmp(perms, "100644") == 0)
		return strdup("-rw-r--r--");

	if (strcmp(perms, "100665") == 0)
		return strdup("-rw-rw-r--");

	if (strcmp(perms, "100755") == 0)
		return strdup("-rwxr-xr-x");

	if (strcmp(perms, "120000") == 0)
		return strdup("l---------");

	if (strcmp(perms, "160000") == 0)
		return strdup("m---------");

	return strdup("??????????");
}

/**
 * Return uppercase version of \p fname.
 *
 * @param fname Filename to make uppercase.
 * @return Uppercase version of \p fname in new buffer.
 */
static char *get_upper(char *fname)
{
	char *new;
	if (!(new = strdup(fname)))
		return NULL;

	/** @todo explicit utf8? */
	char *p = new;
	for (; *p; p++)
		*p = toupper(*p);

	return new;
}

/**
 * Check if file is README.* or any variation of upper/lowercase letters.
 *
 * @param fname Filename to check.
 * @return \c true if \p fname is some README, \c false otherwise.
 */
static bool check_readme(char *fname)
{
	char *upper;
	if (!(upper = get_upper(fname)))
		return false;

	if (strncmp(upper, "README", 5) == 0) {
		free(upper);
		return true;
	}

	free(upper);
	return false;
}

/**
 * Generate reference path for directory entries.
 * Essentially should be links to other files or directories to be used in href.
 *
 * @todo check if this works.
 *
 * @param fname Filename to generate reference path for.
 * @return Reference path of \p fname.
 */
static char *generate_ref_path(char *fname)
{
	/* since we already know we're dealing with a dir, the web dir is just
	 * PATH_INFO. No need to fiddle with parsing a git command or anything. */
	char *web_dir;
	if (!(web_dir = getenv("PATH_INFO")))
		return NULL;

	char *ref;
	if (!(ref = build_path(web_dir, fname)))
		return NULL;

	return ref;
}

/**
 * Helper for generate_dir();
 * Move char pointer to next element in whitespace separated line
 * and split current element at whitespace.
 *
 * @param next Pointer to move.
 */
#define NEXT_FIELD(next)             \
	next;                        \
	next = strpbrk(next, " \t"); \
	*next++ = 0;                 \
	next += strspn(next, " \t");

/**
 * Generate directory entry in dirview based on \p ls_line.
 *
 * @param ls_line One line of output from \c 'git ls-tree -l'.
 * @param readme Pointer to set to git object string if file is a README.
 * Caller should free.
 * @return dir element.
 */
static struct html_elem *generate_dir(char *ls_line, char **readme)
{

	char *next = ls_line;
	char *perms = NEXT_FIELD(next);
	char *type = NEXT_FIELD(next);
	char *object = NEXT_FIELD(next);
	char *size = NEXT_FIELD(next);
	char *fname = next;

	size = strdup(size);
	res_add(r, size);

	fname = strdup(fname);
	res_add(r, fname);

	(void)type;

	struct html_elem *dir = html_create_elem("div", NULL);
	html_add_attr(dir, "class", "dir");

	char *perms_rwx = generate_perms(perms);
	struct html_elem *attrs_elem = html_add_child(dir, "span", perms_rwx);
	html_add_attr(attrs_elem, "class", "attrs");
	res_add(r, perms_rwx);

	struct html_elem *size_elem = html_add_elem(attrs_elem, "span", size);
	html_add_attr(size_elem, "class", "size");

	char *ref_path = generate_ref_path(fname);
	struct html_elem *location = html_add_elem(size_elem, "a", fname);
	html_add_attr(location, "class", "hover-underline");
	html_add_attr(location, "href", ref_path);
	res_add(r, ref_path);

	/** @todo modification time? could be cool but would need a git
	 * invocation per object file, not great */

	if (check_readme(fname))
		*readme = strdup(object);

	return dir;
}

#undef NEXT_FIELD

/**
 * Generate directory view.
 *
 * @param path Directory path (URL) to generate view for.
 * @param readme Pointer to set to git object string if dir contains README.
 * @return dirview element.
 */
static struct html_elem *generate_dirview(struct html_elem *path, char **readme)
{
	/** @todo error checking */
	struct html_elem *dirview = html_add_elem(path, "dir", NULL);
	html_add_attr(dirview, "class", "border dirview");

	char *object = git_object();
	char *root = git_real_root();
	char **cmds[] =
	{(char *[]){"git", "-C", root, "ls-tree", "-l", object, 0}};
	FILE *ls_tree = exgt_chain(1, cmds);
	free(object);
	free(root);

	if (!ls_tree)
		return NULL;

	size_t len = 0;
	ssize_t read = 0;
	char *line = NULL;
	struct html_elem *dir = NULL;
	while ((read = getline(&line, &len, ls_tree)) != -1) {
		struct html_elem *newdir = generate_dir(line, readme);

		if (dir)
			html_append_elem(dir, newdir);
		else
			html_append_child(dirview, newdir);

		dir = newdir;
	}

	if (line)
		free(line);

	fclose(ls_tree);

	return dirview;
}

/**
 * Generate markdown output.
 *
 * @param readme Git object string to README.
 * @return Corresponding markdown html output.
 */
static char *generate_markdown(char *readme)
{
	char *root = git_real_root();
	char **cmds[] =
	{(char *[]){"git", "-C", root, "show", readme, 0},
		/* currently uses my fork of discount, include it as a lib? */
	 (char *[]){"markdown", "-a", "exgt-",
		    "-ffencedcode,fencedinline,toc,taganchor", 0}};
	FILE *markdown = exgt_chain(2, cmds);
	free(root);

	if (!markdown)
		return NULL;


	/* arguably not the fastest way, but likely good enough for now */
	int c;
	size_t i;
	size_t buf_size = 64;
	char *buf = malloc(buf_size);
	for(i = 0; (c = fgetc(markdown)) != EOF; ++i) {
		/* -1 to ensure we always have enough space for a trailing 0. */
		if (i >= buf_size - 1) {
			buf = realloc(buf, buf_size *= 2);
			if (!buf) {
				fclose(markdown);
				return NULL;
			}
		}

		buf[i] = c;
	}

	/* set trailing 0. */
	buf[i] = 0;

	fclose(markdown);

	return buf;
}

/**
 * Generate readme view. Nothing is output if directory doesn't contain readme.
 *
 * @param dirview Directory view that readme view should follow.
 * @param readme Pointer to git object string or \c null if readme doesn't exist.
 * @return readmeview if readme exists, dirview otherwise.
 */
static struct html_elem *generate_readmeview(struct html_elem *dirview,
                                             char *readme)
{
	if (!readme)
		return dirview;

	char *markdown;
	if (!(markdown = generate_markdown(readme)))
		return NULL;

	struct html_elem *readmeview = html_add_elem(dirview, "div", markdown);
	html_add_attr(readmeview, "class", "border readmeview");
	res_add(r, markdown);

	return readmeview;
}

/**
 * Generate directory main.
 *
 * @param dir_main Main element to add dir content as child to.
 * @return Last element in main.
 */
static struct html_elem *generate_main(struct html_elem *dir_main)
{
	struct html_elem *clone;
	if (!(clone = pages_generate_clone(dir_main, r)))
		return NULL;

	struct html_elem *path;
	if (!(path = pages_generate_path(clone, r)))
		return NULL;

	char *readme = NULL;
	struct html_elem *dirview;
	if (!(dirview = generate_dirview(path, &readme)))
		return NULL;

	struct html_elem *readmeview;
	if (!(readmeview = generate_readmeview(dirview, readme))) {
		free(readme);
		return NULL;
	}

	free(readme);

	return readmeview;
}

void dir_serve(FILE *file)
{
	r = res_create();

	http_header(file, 200, "text/html");

	struct html_elem *html, *dir_main;
	/** @todo set dir name instead of "dir" as title */
	if (!(html =
		      pages_generate_common("dir\n", "Search project",
		                            &dir_main, NULL))) {
		error_serve(file, 500, "error serving dir\n");
		goto out;
	}

	if (!generate_main(dir_main)) {
		error_serve(file, 500, "couldn't generate dir main\n");
		goto out;
	}

	html_print(file, html);
out:
	html_destroy(html);
	res_destroy(r);
}
