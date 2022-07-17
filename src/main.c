/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file main.c
 * Main file of server. Dispatches lower lever handlers.
 */

#include <stdio.h>

#include "css/css.h"
#include "html/html.h"
#include "utils/http.h"

/**
 * Serve one document.
 * Separated from main() in case I want to turn this program into a FastCGI program.
 */
static void serve()
{
	enum http_type ht = http_request_type();
	switch (ht) {
	case TEXT_HTML:
		html_serve();
		break;

	case TEXT_CSS:
		css_serve();
		break;

	default:
		http_status(stdout, 406);
		break;
	}
}

/**
 * Main entry point.
 *
 * @return \c 0
 */
int main()
{
	serve();
}
