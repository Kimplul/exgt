/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <stdlib.h>
#include <string.h>
#include "path.h"

char *build_path(const char *root, const char *path_info)
{
	size_t rl = strlen(root);
	size_t pl = strlen(path_info);
	char *path = calloc(1, rl + pl + 1);
	strcat(path, root);
	strcat(path, path_info);
	return path;
}
