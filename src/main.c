/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <stdio.h>

#include "css/css.h"
#include "html/html.h"
#include "utils/http.h"

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

int main()
{
	serve();
}
