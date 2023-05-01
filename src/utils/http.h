/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

/**
 * @file http.h
 * HTTP helper functions.
 */

#ifndef EXGT_HTTP_H
#define EXGT_HTTP_H

#include <stdio.h>

/**
 * Write only \c http status.
 *
 * @param f Output file to write to.
 * @param code Status code to attach.
 */
void http_status(FILE *f, int code);

/**
 * Write only \c http content type.
 *
 * @param f Output file to write to.
 * @param type Content type. \c "text/html", \c "text/css" etc.
 */
void http_content(FILE *f, const char *type);

/**
 * Write both status and content.
 * Note that it's not possible by chaining http_status() and http_content(), as
 * the HTTP header requires two newlines after it.
 *
 * @param f Output file to write to.
 * @param code Status code to attach.
 * @param type Content type.
 */
void http_header(FILE *f, int code, const char *type);

/** Requested content type. We only serve \c html and \c css. */
enum http_type {
	TEXT_HTML, TEXT_CSS, OTHER,
};

/**
 * Get \c http request type.
 *
 * @return \c http request type.
 */
enum http_type http_request_type();

#endif /* EXGT_HTTP_H */
