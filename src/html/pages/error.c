/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

/**
 * @file error.c
 * Error page generator.
 */

#include <unistd.h>

#include <html/html.h>
#include <utils/http.h>
#include <utils/error.h>

#include "pages.h"

void error_serve(FILE *file, int code, const char *msg)
{
	static int loop = 0;
	if (loop++) {
		error("error loop detected, aborting\n");
		return;
	}

	error("reporting error: %s\n", msg);

	/* reset cursor and set file length to zero, essentially erase whole file */
	fseek(file, SEEK_SET, 0);
	ftruncate(fileno(file), 0);

	/* for now, just go with absolute minimum effort. */
	http_header(file, code, "text/html");
	struct html_elem *err = html_create_elem("p", msg);
	html_print(file, err);
	html_destroy(err);
	loop = 0;
}
