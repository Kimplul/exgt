/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file path.c
 * Path handling helper implementations.
 */

#include <stdlib.h>
#include <string.h>
#include "path.h"

char *path_skip_nth(const char *path, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		if (!(path = strchr(path + 1, '/')))
			return NULL;

	char *path_dup;
	if (!(path_dup = strdup(path + 1)))
		return NULL;

	return path_dup;
}

char *path_only_nth(const char *path, size_t n)
{
	char *path_dup;
	if (!(path_dup = path_skip_nth(path, n)))
		return NULL;

	char *slash;
	if ((slash = strchr(path_dup, '/')))
		*slash = 0;

	return path_dup;
}

char *path_cut_nth(const char *path, size_t n)
{
	const char *orig = path;
	for (size_t i = 0; i <= n; ++i)
		if (!(path = strchr(path + 1, '/')))
			return NULL;

	return strndup(orig, path - orig);
}

char *build_path(const char *root, const char *path_info)
{
	size_t rl = strlen(root);
	size_t pl = strlen(path_info);
	char *path = calloc(1, rl + pl + 2);
	strcat(path, root);

	size_t sep_idx = strlen(path) - 1;
	if (path[sep_idx] != '/')
		path[sep_idx + 1] = '/';

	if (path_info[0] == '/')
		strcat(path, path_info + 1);
	else
		strcat(path, path_info);

	return path;
}
