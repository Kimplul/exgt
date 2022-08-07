/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file pages.h
 * Exposes different page generators.
 */

#ifndef EXGT_PAGES_H
#define EXGT_PAGES_H

#include <stdio.h>
#include <html/html.h>
#include <utils/res.h>

/**
 * Serve error page.
 *
 * @param file Output file to write to.
 * @param code Status code.
 * @param msg Error message.
 */
void error_serve(FILE *file, int code, const char *msg);

/**
 * Serve landing page.
 *
 * @param file Output file to write to.
 */
void index_serve(FILE *file);

/**
 * Serve one file page.
 *
 * @param file Output file to write to.
 */
void file_serve(FILE *file);

/**
 * Serve one directory page.
 *
 * @param file Output file to write to.
 */
void dir_serve(FILE *file);

/* Not entirely sure which features I want to implement, but here are a few
 * possibilities
 *
 * void issue_serve();
 * void pr_serve();
 * void commit_serve();
 * void hist_serve();
 * void wiki_serve();
 * void blame_serve();
 * void raw_serve();
 */

/* Common functions to all pages */

/**
 * Generate page header.
 *
 * @param r Resource manager.
 * @param html Root html tag.
 * @param page_title Title of page.
 * @return Pointer to head tag element.
 */
struct html_elem *pages_generate_head(struct res* r,
                                      struct html_elem *html,
                                      const char *page_title);

/**
 * Generate page header.
 *
 * @param r Resource manager.
 * @param body Body tag.
 * @param search_text Text to be inserted into search box.
 * @param cont If not \c NULL, place search element node into this address.
 * Allows use to continue appending elements to header from some higher level
 * function.
 * @return Pointer to header tag element.
 */
struct html_elem *pages_generate_header(struct res *r,
                                        struct html_elem *body,
                                        const char *search_text,
                                        struct html_elem **cont);

/**
 * Generate common elements of all pages.
 *
 * @param r Resource manager.
 * @param title Title of page.
 * @param search Search box placeholder value.
 * @param page_main Pointer where to place main element.
 * @param page_header Pointer where to place header element.
 * @return \c html element.
 */
struct html_elem *pages_generate_common(struct res *r, const char *title,
                                        const char *search,
                                        struct html_elem **page_main,
                                        struct html_elem **page_header);

/**
 * Generate clone for page.
 *
 * @param r Resource manager.
 * @param page_main Parent main element.
 * @return Clone element.
 */
struct html_elem *pages_generate_clone(struct res *r,
                                       struct html_elem *page_main);

/**
 * Generate path for page.
 *
 * @param r Resource manager.
 * @param clone Previous clone element.
 * @return Path element.
 */
struct html_elem *pages_generate_path(struct res *r, struct html_elem *clone);

/**
 * Generate doctype for html page.
 *
 * @param file File to ouput doctype to.
 */
void pages_generate_doctype(FILE *file);

#endif /* EXGT_PAGES_H */
