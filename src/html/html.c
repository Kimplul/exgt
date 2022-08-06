/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file html.c
 * HTML generation implementation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#include <utils/path.h>
#include <utils/git.h>
#include <utils/chain.h>

#include "pages/pages.h"
#include "html.h"

/**
 * Print one html attribute.
 *
 * @param file Output file to print to.
 * @param attr Attribute to print.
 */
static void html_print_attr(FILE *file, struct html_attr *attr)
{
	if (attr->value)
		fprintf(file, " %s=\"%s\"", attr->name, attr->value);
	else
		fprintf(file, " %s", attr->name);
}

/**
 * Print html element attributes.
 *
 * @param file Output file to print to.
 * @param attr Attributes to print.
 */
static void html_print_attrs(FILE *file, struct html_attr *attr)
{
	for (; attr; attr = attr->prev)
		html_print_attr(file, attr);
}

/**
 * Print start tag, stuff like <p> etc.
 * If element is just raw text, no tag will be printed.
 *
 * @param file Output file to print to.
 * @param elem Element whose start tag to print.
 */
static void html_print_starttag(FILE *file, struct html_elem *elem)
{
	if (elem->tag) {
		fprintf(file, "<%s", elem->tag);
		html_print_attrs(file, elem->attrs);
		fprintf(file, ">");
	}
}

/**
 * Print end tag, stuff like </p> etc.
 * If element is raw text, no tag will be printed.
 *
 * @param file Output file to print to.
 * @param elem Element whose end tag to print.
 */
static void html_print_endtag(FILE *file, struct html_elem *elem)
{
	if (elem->tag)
		fprintf(file, "</%s>\n", elem->tag);
}

/**
 * Print one HTML element.
 *
 * @param file Output file to print to.
 * @param elem Element to print.
 */
static void html_print_elem(FILE *file, struct html_elem *elem)
{
	html_print_starttag(file, elem);

	if (elem->value)
		fprintf(file, "%s", elem->value);

	html_print(file, elem->child);
	html_print_endtag(file, elem);
}

void html_print(FILE *file, struct html_elem *elem)
{
	for (; elem; elem = elem->next)
		html_print_elem(file, elem);
}

struct html_attr *html_create_attr(const char *name, const char *value)
{
	struct html_attr content = (struct html_attr){name, value, NULL};
	struct html_attr *attr = malloc(sizeof(struct html_attr));
	memcpy(attr, &content, sizeof(struct html_attr));
	return attr;
}

struct html_elem *html_create_elem(const char *tag, const char *value)
{
	struct html_elem content =
		(struct html_elem){tag, value, NULL, NULL, NULL};
	struct html_elem *elem = malloc(sizeof(struct html_elem));
	memcpy(elem, &content, sizeof(struct html_elem));
	return elem;
}

void html_append_attr(struct html_elem *elem, struct html_attr *attr)
{
	attr->prev = elem->attrs;
	elem->attrs = attr;
}

struct html_attr *html_add_attr(struct html_elem *elem, const char *name,
                                const char *value)
{
	struct html_attr *attr = html_create_attr(name, value);
	html_append_attr(elem, attr);
	return attr;
}

void html_append_elem(struct html_elem *prev, struct html_elem *next)
{
	assert(!prev->next);
	prev->next = next;
}

struct html_elem *html_add_elem(struct html_elem *prev, const char *tag,
                                const char *value)
{
	struct html_elem *elem = html_create_elem(tag, value);
	html_append_elem(prev, elem);
	return elem;
}

void html_append_child(struct html_elem *parent, struct html_elem *child)
{
	assert(!parent->child);
	parent->child = child;
}

struct html_elem *html_add_child(struct html_elem *parent, const char *tag,
                                 const char *value)
{
	struct html_elem *elem = html_create_elem(tag, value);
	html_append_child(parent, elem);
	return elem;
}

/**
 * Destroy element attribues.
 *
 * @param attr Start of attribute list.
 */
static void html_attrs_destroy(struct html_attr *attr)
{
	while (attr) {
		struct html_attr *tmp = attr->prev;
		free(attr);
		attr = tmp;
	}
}

void html_destroy(struct html_elem *elem)
{
	while (elem) {
		html_destroy(elem->child);
		html_attrs_destroy(elem->attrs);

		struct html_elem *tmp = elem->next;
		free(elem);
		elem = tmp;
	}
}

/**
 * Serve page that is based on a "real" file in some repo.
 *
 * @param file Output file to print to.
 */
static void real_serve(FILE *file)
{
	char *object = git_object();
	char *root = git_real_root();
	char **cmds[] =
	{(char *[]){"git", "-C", root, "cat-file", "-t", object, 0}};
	FILE *file_type = exgt_chain(1, cmds);

	free(object);
	free(root);

	if (!file_type) {
		error_serve(file, 500, "illegal command format");
		return;
	}

	size_t len = 0;
	char *line = NULL;
	getline(&line, &len, file_type);

	if (!line) {
		error_serve(file, 500, "not a git repo");
		return;
	}

	if (strncmp(line, "tree", 4) == 0)
		dir_serve(file);
	else if (strncmp(line, "blob", 4) == 0)
		file_serve(file);
}

/**
 * Serve page that is not based on "real" files.
 * Currently only the index page, but eventually probably other pages like git
 * log or git commit or whatever.
 *
 * @param file Output file to print to.
 */
static void unreal_serve(FILE *file)
{
	index_serve(file);
}

void html_serve()
{

	char *buf;
	size_t size;
	FILE *file = open_memstream(&buf, &size);
	if (!file) {
		perror("open_memstream failed");
		return;
	}

	char *path = getenv("PATH_INFO");
	if (!path) {
		fprintf(stderr, "PATH_INFO missing\n");
		error_serve(file, 500, "PATH_INFO missing\n");
		goto out;
	}

	/** @todo utf8? */
	/** @todo be more exagt, i.e. regex /exgt/? or something */
	if (strncmp(path, "/exgt/", 7) > 0)
		/* skip /exgt/ part of path */
		real_serve(file);
	else
		unreal_serve(file);

out:
	/* print file buffer content to server */
	fclose(file);
	fputs(buf, stdout);
	free(buf);
}
