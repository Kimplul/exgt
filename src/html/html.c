/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#include <utils/path.h>

#include "pages/pages.h"
#include "html.h"

static void html_print_attr(FILE *file, struct html_attr *attr)
{
	if (attr->value)
		fprintf(file, " %s=\"%s\"", attr->name, attr->value);
	else
		fprintf(file, " %s", attr->name);
}

static void html_print_attrs(FILE *file, struct html_attr *attr)
{
	for (; attr; attr = attr->prev)
		html_print_attr(file, attr);
}

static void html_print_starttag(FILE *file, struct html_elem *elem)
{
	if (elem->tag) {
		fprintf(file, "<%s", elem->tag);
		html_print_attrs(file, elem->attrs);
		fprintf(file, ">\n");
	}
}

static void html_print_endtag(FILE *file, struct html_elem *elem)
{
	if (elem->tag)
		fprintf(file, "</%s>\n", elem->tag);
}

static void html_print_elem(FILE *file, struct html_elem *elem)
{
	html_print_starttag(file, elem);
	fprintf(file, elem->value);
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

void real_serve(FILE *file, const char *path)
{
	char *root = getenv("GIT_PROJECT_ROOT");
	if (!root) {
		fprintf(stderr, "GIT_PROJECT_ROOT missing\n");
		error_serve(file, 500, "GIT_PROJECT_ROOT missing\n");
		return;
	}

	char *full_path = build_path(root, path);
	struct stat sb;
	if (stat(full_path, &sb)) {
		perror("stat failed");
		error_serve(file, 404,
		            "File or directory could not be found\n");
		goto out;
	}

	switch (sb.st_mode & S_IFMT) {
	case S_IFDIR: dir_serve(file); break;
	case S_IFREG: file_serve(file); break;
	}

out:
	free(full_path);
}

void unreal_serve(FILE *file, const char *path)
{
	if (strcmp(path, "/") == 0)
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
	if (strcmp(path, "/exgt") == 0)
		/* skip /exgt part of path */
		real_serve(file, path + 5);
	else
		unreal_serve(file, path);

out:
	/* print file buffer content to server */
	fclose(file);
	fputs(buf, stdout);
	free(buf);
}
