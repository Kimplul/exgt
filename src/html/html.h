#ifndef EXGT_HTML_H
#define EXGT_HTML_H

#include <stdio.h>

/** Linked list of html attributes. */
struct html_attr {
	/** Name of attribute. */
	const char *name;
	/** Value of attribute. */
	const char *value;
	/** Previous attribute in list. */
	struct html_attr *prev;
};

/**
 * Simplified view of an html element, probably good enough for us.
 *
 * Each element is handled the same way, if there exists a tag, it is printed.
 * If the tag has attributes, they are also printed. The value of the element is
 * always printed, as well as the child. A raw text element may have a child,
 * though it's likely more clear if you avoid such situations.
 * */
struct html_elem {
	/** Tag of element, p or a or div or whatever. */
	const char *tag;
	/**
	 * If tag is null, this is raw text that should be inserted. Otherwise
	 * unused.
	 */
	const char *value;

	/** List of attributes. */
	struct html_attr *attrs;

	/** Child element. */
	struct html_elem *child;

	/** Next element. */
	struct html_elem *next;
};

/**
 * Append attribute to element.
 * Element must have a tag.
 *
 * @param elem Element to add attribute to.
 * @param attr Attribute to add to element \p elem.
 */
void html_append_attr(struct html_elem *elem, struct html_attr *attr);

/**
 * Add attribute with specified values to element.
 *
 * @param elem Element to add attributes to.
 * @param name Name of attribute.
 * @param value Value of attribute.
 * @return New attribute.
 */
struct html_attr *html_add_attr(struct html_elem *elem, const char *name,
                                const char *value);

/**
 * Append element to list of html elements in current context.
 *
 * @param prev Element to append \p next after.
 * @param next Element to append after \p prev.
 */
void html_append_elem(struct html_elem *prev, struct html_elem *next);

/**
 * Add element with specified values to element.
 *
 * @param prev Element to append after.
 * @param tag Tag of new element.
 * @param value Value of new element.
 * @return New element.
 */
struct html_elem *html_add_elem(struct html_elem *prev, const char *tag,
                                const char *value);

/**
 * Append child element.
 * Any element can only do this once.
 *
 * @param parent Parent element to assign a child to.
 * @param child Child element to assign to \p parent.
 */
void html_append_child(struct html_elem *parent, struct html_elem *child);

/**
 * Add child with specified values to element.
 * Any element can only do this once.
 *
 * @param parent Parent element.
 * @param tag Tag of new element.
 * @param value Value of new element.
 * @return New element.
 */
struct html_elem *html_add_child(struct html_elem *parent, const char *tag,
                                 const char *value);

/**
 * Print out html.
 *
 * @param file File to print to.
 * @param elem Tree of html element nodes.
 */
void html_print(FILE *file, struct html_elem *elem);

/**
 * Helper function for creating an attribute.
 *
 * @param name Name of attribute.
 * @param value Value of attribute.
 * @return Allocated node with specified parameters.
 */
struct html_attr *html_create_attr(const char *name, const char *value);

/**
 * Helper function for creating an element.
 *
 * @param tag Tag of element.
 * @param value Value of element.
 * @return Allocated node with specified attributes.
 */
struct html_elem *html_create_elem(const char *tag, const char *value);

/**
 * Iterate tree and free nodes.
 * Note that text strings associated with each element and/or attribute
 * are not freed, that is up to the caller.
 *
 * @param elem Element to free (probably root of HTML).
 */
void html_destroy(struct html_elem *elem);

/** Generate html document. */
void html_serve();

#endif /* EXGT_HTML_H */
