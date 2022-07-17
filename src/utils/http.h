/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef EXGT_HTTP_H
#define EXGT_HTTP_H

#include <stdio.h>

void http_status(FILE *f, int code);
void http_content(FILE *f, const char *type);
void http_header(FILE *f, int code, const char *type);

enum http_type {
	TEXT_HTML, TEXT_CSS, OTHER,
};

enum http_type http_request_type();

#endif /* EXGT_HTTP_H */
