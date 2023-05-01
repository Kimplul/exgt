/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

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

struct html_elem *pages_generate_head(struct res *r, struct html_elem *html,
                                      const char *page_title)
{
	struct html_elem *head = html_add_child(html, "head", NULL);
	struct html_elem *title = html_add_child(head, "title", page_title);
	struct html_elem *utf8 = html_add_elem(title, "meta", NULL);
	html_add_attr(utf8, "charset", "utf8");

	char *styles;
	if (!(styles = build_web_path("styles.css")))
		return NULL;

	struct html_elem *preload = html_add_elem(utf8, "link", NULL);
	html_add_attr(preload, "rel", "preload");
	html_add_attr(preload, "href", styles);
	html_add_attr(preload, "as", "style");

	struct html_elem *link = html_add_elem(preload, "link", NULL);
	html_add_attr(link, "rel", "stylesheet");
	html_add_attr(link, "href", styles);

	res_add(r, styles);

	return head;
}

struct html_elem *pages_generate_header(struct res *r, struct html_elem *body,
                                        struct html_elem **cont)
{
	struct html_elem *header = html_add_child(body, "header", NULL);
	/** @todo allow user to specify home button text? */
	char *web_root = web_root_path();
	struct html_elem *exgt_button = html_add_child(header, "a", "EXGT");
	html_add_attr(exgt_button, "class", "button");
	html_add_attr(exgt_button, "href", web_root);
	res_add(r, web_root);

	if (cont)
		*cont = exgt_button;

	return header;
}

struct html_elem *pages_generate_common(struct res *r,
                                        const char *title,
                                        struct html_elem **page_main,
                                        struct html_elem **page_header)
{
	(void)r; /* unused for now */

	/* normally I don't use C89 style initialization, but lto gives a
	 * warning about possibly uninitialized variables if I use my typical
	 * style. */
	struct html_elem *html = NULL;
	struct html_elem *head = NULL;
	struct html_elem *body = NULL;
	struct html_elem *elem_header = NULL;
	struct html_elem *elem_main = NULL;

	if (!(html = html_create_elem("html", NULL))) {
		error("couldn't create html\n");
		return NULL;
	}

	if (!(head = pages_generate_head(r, html, title))) {
		error("couldn't create head\n");
		goto out;
	}

	if (!(body = html_add_elem(head, "body", NULL))) {
		error("couldn't create body\n");
		goto out;
	}

	if (!(elem_header = pages_generate_header(r, body, NULL))) {
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

struct html_elem *pages_generate_clone(struct res *r,
                                       struct html_elem *page_main)
{
	(void)r; /* unused for now */
	struct html_elem *clone = html_add_child(page_main, "div", NULL);
	html_add_attr(clone, "class", "clone");

	struct html_elem *http_clone = html_add_child(clone, "span",
	                                              "https://tmp");
	html_add_elem(http_clone, "span", "exgt@tmp:tmp");
	return clone;
}

struct html_elem *pages_generate_path(struct res *r, struct html_elem *clone)
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
