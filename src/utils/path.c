/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file path.c
 * Path handling helper implementations.
 */

#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "path.h"

char *path_skip_nth(const char *path, size_t n)
{
	for (size_t i = 0; i < n; ++i)
		if (!(path = strchr(path + 1, '/')))
			return NULL;

	/* missing trailing '/' should just be an empty string. */
	if (!(path = strchr(path + 1, '/')))
		return strdup("");

	char *path_dup;
	if (!(path_dup = strdup(path + 1)))
		return NULL;

	return path_dup;
}

char *path_only_nth(const char *path, size_t n)
{
	char *path_dup;
	if (!(path_dup = path_skip_nth(path, n - 1)))
		return NULL;

	char *slash;
	if ((slash = strchr(path_dup, '/')))
		*slash = 0;

	return path_dup;
}

char *path_cut_nth(const char *path, size_t n)
{
	const char *orig = path;
	for (size_t i = 0; i < n; ++i)
		if (!(path = strchr(path + 1, '/')))
			return NULL;

	/* missing trailing '/' should be the string itself*/
	if (!(path = strchr(path + 1, '/')))
		return strdup(orig);

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

char *web_root_path()
{
	static char *web_root = NULL;

	char *request_uri;
	if (!(request_uri = getenv("REQUEST_URI"))) {
		error("couldn't find REQUEST_URI\n");
		return NULL;
	}

	if (web_root)
		goto out;

	char *path_info;
	if (!(path_info = getenv("PATH_INFO"))) {
		error("couldn't find PATH_INFO\n");
		return NULL;
	}

	if (strcmp(path_info, "/") == 0) {
		web_root = request_uri + strlen(request_uri);
		goto out;
	}

	if (!(web_root = strstr(request_uri, path_info))) {
			error("couldn't get web root\n");
			return NULL;
	}

out:
	return strndup(request_uri, web_root - request_uri);
}

char *build_web_path(const char *path)
{
	char *web_root;
	if (!(web_root = web_root_path()))
		return NULL;

	char *web_path;
	if (!(web_path = build_path(web_root, path))) {
		free(web_root);
		return NULL;
	}

	free(web_root);
	return web_path;
}

char *path_last_elem(const char *path)
{
	char *slash = strrchr(path, '/');

	/* path contains only one element */
	if (!slash)
		return strdup(path);

	/* path is just '/' */
	if (slash == path)
		return strdup(path);

	return strdup(slash + 1);
}
