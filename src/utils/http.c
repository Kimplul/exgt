#include <string.h>
#include <stdlib.h>

#include "http.h"

void http_status(FILE *f, int code)
{
	fprintf(f, "Status: %d\n\n", code);
}

void http_content(FILE *f, const char*type)
{
	fprintf(f, "Content-type: %s\n\n", type);
}

void http_header(FILE *f, int code, const char *type)
{
	fprintf(f, "Status: %d\n", code);
	fprintf(f, "Content-type: %s\n\n", type);
}

enum http_type http_request_type()
{
	char *accept = getenv("HTTP_ACCEPT");
	if (!accept)
		return OTHER;

	if (strstr(accept, "text/html"))
		return TEXT_HTML;

	if (strstr(accept, "text/css"))
		return TEXT_CSS;

	return OTHER;
}
