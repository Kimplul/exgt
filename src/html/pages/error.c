/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file error.c
 * Error page generator.
 */

#include <unistd.h>

#include <html/html.h>
#include <utils/http.h>

#include "pages.h"

void error_serve(FILE *file, int code, const char *msg)
{
	static int loop = 0;
	if (loop++) {
		fprintf(stderr, "error loop detected, aborting\n");
		return;
	}

	fprintf(stderr, "reporting error: %s\n", msg);

	/* set file length to zero, essentially erase whole file */
	ftruncate(fileno(file), 0);

	/* for now, just go with absolute minimum effort. */
	http_header(file, code, "text/html");
	struct html_elem *err = html_create_elem("p", msg);
	html_print(file, err);
	html_destroy(err);
	loop = 0;
}
