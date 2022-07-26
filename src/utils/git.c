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

	/* /exgt */
	if (!(path = strchr(path + 1, '/')))
		return NULL;

	/* project owner name */
	if (!(path = strchr(path + 1, '/')))
		return NULL;

	/* git root name */
	if ((path = strchr(path + 1, '/')))
		/* actual path */
		return strdup(path);
	else
		/* missing slash means git root */
		return strdup("");


	return NULL;
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

	if (!(path = strchr(path, '/')))
		return NULL;

	/* skip first slash */
	if (!(path = strdup(path + 1)))
		return NULL;

	char *next;
	if (!(next = strchr(path, '/'))) {
		free(path);
		return NULL;
	}

	*next = 0;

	return path;
}

char *git_repo_name()
{
	char *path;
	if (!(path = getenv("PATH_INFO")))
		return NULL;

	/* skip /exgt */
	if (!(path = strchr(path + 1, '/')))
		return NULL;

	/* skip username */
	if (!(path = strchr(path + 1, '/')))
		return NULL;

	/* skip first slash */
	if (!(path = strdup(path + 1)))
		return NULL;

	char *next;
	if (!(next = strchr(path, '/'))) {
		free(path);
		return NULL;
	}

	*next = 0;

	return path;
}

char *git_root()
{
	char *path;
	if (!(path = getenv("PATH_INFO")))
		return NULL;

	/* skip /exgt */
	if (!(path = strchr(path + 1, '/')))
		return NULL;

	if (!(path = strdup(path + 1)))
		return NULL;

	char *next = path;
	if (!(next = strchr(next + 1, '/'))) {
		free(path);
		return NULL;
	}

	if ((next = strchr(next + 1, '/')))
		*next = 0;

	return path;
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
