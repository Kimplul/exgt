/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file path.c
 * Path handling helper implementations.
 */

#include <stdlib.h>
#include <string.h>
#include "path.h"

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
