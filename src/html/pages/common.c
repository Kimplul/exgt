/**
 * @file common.c
 * Common HTML element generation implementation.
 * Header, footer, etc.
 */

#include <stddef.h>
#include <html/html.h>

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
	html_add_attr(exgt_button, "href", "/");

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
