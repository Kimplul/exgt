/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

/**
 * @file file.c
 * Code browser file view generator.
 */

#include <stdlib.h>
#include <string.h>

#include <utils/git.h>
#include <utils/chain.h>
#include <utils/http.h>

#include "pages.h"

/** File generator resource manager. */
static struct res *r;

/**
 * Convert \c size_t to string.
 * @todo could be made more general.
 *
 * @param lineno Where to place the pointer to the generated string.
 * @param i Line number to generate.
 * @return Number of characters in \p lineno.
 */
static size_t generate_lineno(char **lineno, size_t i)
{
	size_t sz = snprintf(NULL, 0, "%zu", i) + 1;
	if (!(*lineno = malloc(sz)))
		return 0;

	return snprintf(*lineno, sz, "%zu", i);
}

/**
 * Generate id attribute from line number.
 *
 * @param lineno_id Where to place the pointer to the generated string.
 * @param lineno Line number as a string.
 * @param len Length of the line number.
 * @return Line number id as a string.
 */
static size_t generate_lineno_id(char **lineno_id, char *lineno, size_t len)
{
	size_t l = len + 3;

	if (!(*lineno_id = malloc(l)))
		return 0;

	/* create "empty" string */
	*lineno_id[0] = 0;
	strcat(*lineno_id, "l_");
	strcat(*lineno_id, lineno);
	return l;
}

/**
 * Generate href of line number id.
 *
 * @param lineno_href Where to place the pointer to the generated string.
 * @param lineno_id Line number id.
 * @param len Length of the line number id.
 * @return Href to the line number id.
 */
static size_t generate_lineno_href(char **lineno_href, char *lineno_id,
                                   size_t len)
{
	size_t l = len + 2;

	if (!(*lineno_href = malloc(l)))
		return 0;

	*lineno_href[0] = 0;
	strcat(*lineno_href, "#");
	strcat(*lineno_href, lineno_id);
	return l;
}

/**
 * Generate one entry into the line table.
 *
 * @param line Line to insert.
 * @param i Line number.
 * @return Table entry element.
 */
static struct html_elem *generate_entry(char *line, size_t i)
{
	struct html_elem *tr = html_create_elem("tr", NULL);

	char *lineno, *lineno_id, *lineno_href;

	size_t l_n = generate_lineno(&lineno, i);
	size_t l_i = generate_lineno_id(&lineno_id, lineno, l_n);
	generate_lineno_href(&lineno_href, lineno_id, l_i);

	res_add(r, lineno);
	res_add(r, lineno_id);
	res_add(r, lineno_href);

	/* line number */
	struct html_elem *td0 = html_add_child(tr, "td", NULL);
	html_add_attr(td0, "class", "lineno");
	struct html_elem *a = html_add_child(td0, "a", lineno);
	html_add_attr(a, "id", lineno_id);
	html_add_attr(a, "href", lineno_href);

	/* line itself */
	char *line_dup = strdup(line);
	html_add_elem(td0, "td", line_dup);
	res_add(r, line_dup);

	return tr;
}

/**
 * Get the line ending of the file or if unknown, pretend file is raw text.
 *
 * @param object Path to extract file ending from.
 * @return File ending or "txt" if unknown.
 */
static char *generate_syntax(char *object)
{
	/** @todo figure out what to do with files without a suffix, like
	 * Makefile or README that aren't just raw text files? */
	char *suffix;
	if (!(suffix = strrchr(object, '.')))
		return strdup("txt");

	char *prefix;
	if (!(prefix = strrchr(object, '/')))
		prefix = strchr(object, ':') + 1;

	/* file is form .name, assume text */
	if (suffix == prefix)
		return strdup("txt");

	return strdup(suffix + 1);
}

/**
 * Generate one file, with syntax highlighting and line numbers.
 *
 * @param table Parent table element.
 * @return File element.
 */
static struct html_elem *generate_file(struct html_elem *table)
{
	char *object = git_object();
	char *root = git_real_root();
	char *syntax = generate_syntax(object);
	char **cmds[] =
	{(char *[]){"git", "-C", root, "show", object, 0},
	 (char *[]){"highlight", "-S", syntax, "-O", "html", "-f", 0}};
	FILE *highlight = exgt_chain(2, cmds);
	free(syntax);
	free(object);
	free(root);

	if (!highlight)
		return NULL;

	size_t len = 0;
	ssize_t read = 0;
	char *line = NULL;
	struct html_elem *entry = NULL;
	for (size_t i = 0; (read = getline(&line, &len, highlight)) != -1;
	     ++i) {
		struct html_elem *new_entry = generate_entry(line, i);

		if (entry)
			html_append_elem(entry, new_entry);
		else
			html_append_child(table, new_entry);

		entry = new_entry;
	}

	if (line)
		free(line);

	fclose(highlight);

	return entry;
}

/**
 * Generate fileview.
 *
 * @param path Path element fileview is to be placed after.
 * @return Fileview element.
 */
static struct html_elem *generate_fileview(struct html_elem *path)
{
	struct html_elem *fileview = html_add_elem(path, "div", NULL);
	html_add_attr(fileview, "class", "border fileview");

	struct html_elem *table = html_add_child(fileview, "table", NULL);
	html_add_attr(table, "class", "file");

	if (!generate_file(table))
		return NULL;

	return fileview;
}

/**
 * Generate file main content.
 *
 * @param file_main File main element content is to be placed under.
 * @return Last content element, currently the fileview.
 */
static struct html_elem *generate_main(struct html_elem *file_main)
{
	struct html_elem *clone;
	if (!(clone = pages_generate_clone(r, file_main)))
		return NULL;

	struct html_elem *path;
	if (!(path = pages_generate_path(r, clone)))
		return NULL;

	struct html_elem *fileview;
	if (!(fileview = generate_fileview(path)))
		return NULL;

	return fileview;
}

void file_serve(FILE *file)
{
	char *title;
	if (!(title = git_web_last())) {
		error_serve(file, 500, "couldn't get current git element\n");
		return;
	}

	r = res_create();
	res_add(r, title);

	http_header(file, 200, "text/html");

	struct html_elem *html, *file_main;
	/** @todo set file name instead of "file" as title */
	if (!(html =
		      pages_generate_common(r, title,
		                            &file_main, NULL))) {
		error_serve(file, 500, "error serving file\n");
		goto out;
	}

	if (!generate_main(file_main)) {
		error_serve(file, 500, "couldn't generate file main\n");
		goto out;
	}

	html_print(file, html);
out:
	html_destroy(html);
	res_destroy(r);
}
