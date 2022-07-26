#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "url.h"

/**
 * @file url.c
 * Url helper implementations.
 */

char *url_option(const char *key)
{
	char *query = getenv("QUERY_STRING");
	if (!query) {
		error("couldn't find QUERY_STRING\n");
		return NULL;
	}

	query = strdup(query);
	if (!query)
		return NULL;

	char *next = query, *prev = query;
	while ((next = strchr(next, '='))) {
		*next++ = '0';

		if (strcmp(prev, key) == 0)
			break;

		next = strchr(next, '&') + 1;

		if (!*next) {
			free(query);
			return NULL;
		}
	}

	char *res = NULL;
	if (next)
		res = strdup(next);

	free(query);

	return res;
}
