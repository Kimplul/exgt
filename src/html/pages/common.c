/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file common.c
 * Common HTML element generation implementation.
 * Header, footer, etc.
 */

#include <html/html.h>
#include <utils/error.h>
#include <utils/path.h>
#include <utils/git.h>
#include <html/pages/pages.h>

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

struct html_elem *pages_generate_head(struct html_elem *html,
                                      const char *page_title)
{
	struct html_elem *head = html_add_child(html, "head", NULL);
	struct html_elem *title = html_add_child(head, "title", page_title);
	struct html_elem *utf8 = html_add_elem(title, "meta", NULL);
	html_add_attr(utf8, "charset", "utf8");

	struct html_elem *link = html_add_elem(utf8, "link", NULL);
	html_add_attr(link, "rel", "stylesheet");
	html_add_attr(link, "href", "styles.css");

	return head;
}

struct html_elem *pages_generate_header(struct html_elem *body,
                                        const char *search_text,
                                        struct html_elem **cont)
{
	struct html_elem *header = html_add_child(body, "header", NULL);
	/** @todo allow user to specify home button text? */
	struct html_elem *exgt_button = html_add_child(header, "a", "EXGT");
	html_add_attr(exgt_button, "class", "button");
	html_add_attr(exgt_button, "href", "/exgt");

	struct html_elem *user_button = html_add_elem(exgt_button, "a", "User");
	html_add_attr(user_button, "class", "button");
	html_add_attr(user_button, "href", "/user");

	struct html_elem *search = html_add_elem(user_button, "input", NULL);
	html_add_attr(search, "class", "search border");
	html_add_attr(search, "type", "search");
	html_add_attr(search, "placeholder", search_text);

	if (cont)
		*cont = search;

	return header;
}

struct html_elem *pages_generate_common(const char *title, const char *search,
                                        struct html_elem **page_main,
                                        struct html_elem **page_header)
{
	/* normally I don't use C89 style initialization, but lto gives a
	 * warning about possibly uninitialized variables if I use my typical
	 * style. */
	struct html_elem *html = NULL;
	struct html_elem *head = NULL;
	struct html_elem *body = NULL;
	struct html_elem *elem_header = NULL;
	struct html_elem *elem_main = NULL;

	if (!(html = html_create_elem("html", NULL))) {
		error("couldn't create html");
		return NULL;
	}

	if (!(head = pages_generate_head(html, title))) {
		error("couldn't create head");
		goto out;
	}

	if (!(body = html_add_elem(head, "body", NULL))) {
		error("couldn't create body");
		goto out;
	}

	if (!(elem_header = pages_generate_header(body, search, NULL))) {
		error("couldn't create header");
		goto out;
	}

	if (!(elem_main = html_add_elem(elem_header, "main", NULL))) {
		error("couldn't create main");
		goto out;
	}

out:
	if (page_main)
		*page_main = elem_main;

	if (page_header)
		*page_header = elem_header;

	return html;
}

struct html_elem *pages_generate_clone(struct html_elem *page_main,
                                       struct res *r)
{
	struct html_elem *clone = html_add_child(page_main, "div", NULL);
	html_add_attr(clone, "class", "clone");

	struct html_elem *http_clone = html_add_child(clone, "span",
	                                              "https://tmp");
	html_add_elem(http_clone, "span", "exgt@tmp:tmp");
	return clone;
}

struct html_elem *pages_generate_path(struct html_elem *clone,
                                      struct res *r)
{
	char *path;
	if (!(path = git_path()))
		return NULL;
	res_add(r, path);

	char *web_root;
	if (!(web_root = git_web_root()))
		return NULL;
	res_add(r, web_root);

	struct html_elem *path_div = html_add_elem(clone, "div", NULL);
	html_add_attr(path_div, "class", "path");

	char *repo_name;
	if (!(repo_name = git_repo_name()))
		return NULL;
	res_add(r, repo_name);

	struct html_elem *root = html_add_child(path_div, "a", repo_name);
	html_add_attr(root, "class", "path-elem hover-underline");
	html_add_attr(root, "href", web_root);

	struct html_elem *elem = html_add_elem(root, "span", "/");
	html_add_attr(elem, "class", "path-sep");

	char *next, *prev = path, *href = web_root;
	while ((next = strchr(prev, '/'))) {
		*next++ = 0;

		elem = html_add_elem(elem, "a", prev);

		href = build_path(href, prev);
		html_add_attr(elem, "class", "path-elem hover-underline");
		html_add_attr(elem, "href", href);
		res_add(r, href);

		elem = html_add_elem(elem, "span", "/");
		html_add_attr(elem, "class", "path-sep");

		prev = next;
	}

	if (*prev != 0) {
		struct html_elem *fname = html_add_elem(elem, "span", prev);
		html_add_attr(fname, "class", "path-elem");
	}

	return path_div;
}

void pages_generate_doctype(FILE *file)
{
	fprintf(file, "<!DOCTYPE html>\n");
}
