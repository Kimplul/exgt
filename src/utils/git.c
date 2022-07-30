#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "url.h"
#include "git.h"
#include "path.h"

/**
 * @file git.c
 * Git helper implementations.
 * @todo implement path nth element parser or something, this is getting dumb.
 */

char *git_path()
{
	char *path = getenv("PATH_INFO");
	if (!path)
		return NULL;

	return path_skip_nth(path, 3);
}

char *git_commit()
{
	/*@todo implement url option parsing */
	char *commit;
	if ((commit = url_option("commit")))
		return commit;

	return strdup("HEAD");
}

char *git_object()
{
	char *path;
	if (!(path = git_path()))
		return NULL;

	char *commit;
	if (!(commit = git_commit())) {
		free(path);
		return NULL;
	}

	char *object;
	if (!(object = calloc(1, strlen(commit) + strlen(path) + 2))) {
		free(path);
		free(commit);
		return NULL;
	}

	strcat(object, commit);
	strcat(object, ":");
	strcat(object, path);

	free(path);
	free(commit);

	return object;
}

char *git_user_name()
{
	char *path;
	if (!(path = getenv("PATH_INFO")))
		return NULL;

	return path_only_nth(path, 1);
}

char *git_repo_name()
{
	char *path;
	if (!(path = getenv("PATH_INFO")))
		return NULL;

	return path_only_nth(path, 2);
}

char *git_root()
{
	char *path;
	if (!(path = getenv("PATH_INFO")))
		return NULL;

	char *start;
	if (!(start = path_cut_nth(path, 2)))
		return NULL;

	char *mid;
	if (!(mid = path_skip_nth(start, 1))) {
		free(start);
		return NULL;
	}

	free(start);
	return mid;
}

char *git_real_root()
{
	char *path;
	if (!(path = getenv("GIT_PROJECT_ROOT")))
		return NULL;

	char *root;
	if (!(root = git_root()))
		return NULL;

	if (!(path = build_path(path, root))) {
		free(root);
		return NULL;
	}

	free(root);
	return path;
}

char *git_web_root()
{
	char *path;
	if (!(path = getenv("PATH_INFO")))
		return NULL;

	return path_cut_nth(path, 2);
}
