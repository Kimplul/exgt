#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "html.h"

static void html_print_attr(struct html_attr *attr)
{
	if (attr->value)
		printf(" %s=\"%s\"", attr->name, attr->value);
	else
		printf(" %s", attr->name);
}

static void html_print_attrs(struct html_attr *attr)
{
	for (; attr; attr = attr->prev)
		html_print_attr(attr);
}

static void html_print_starttag(struct html_elem *elem)
{
	if (elem->tag) {
		printf("<%s", elem->tag);
		html_print_attrs(elem->attrs);
		puts(">");
	}
}

static void html_print_endtag(struct html_elem *elem)
{
	if (elem->tag)
		printf("</%s>\n", elem->tag);
}

static void html_print_elem(struct html_elem *elem)
{
	html_print_starttag(elem);
	puts(elem->value);
	html_print(elem->child);
	html_print_endtag(elem);
}

void html_print(struct html_elem *elem)
{
	for (; elem; elem = elem->next)
		html_print_elem(elem);
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
