/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "url.h"
#include "git.h"
#include "path.h"
#include "error.h"
#include "chain.h"
#include "file.h"

/**
 * @file git.c
 * Git helper implementations.
 * @todo implement path nth element parser or something, this is getting dumb.
 */

char *git_path()
{
	char *path;
	if (!(path = getenv("PATH_INFO"))) {
		error("couldn't find PATH_INFO\n");
		return NULL;
	}

	char *cut_path = path_cut_nth(path, 1);
	return cut_path ? cut_path : strdup("");
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
	/* this doesn't work */
	char *path;
	if (!(path = getenv("PATH_INFO"))) {
		error("couldn't find PATH_INFO\n");
		return NULL;
	}

	return path_only_nth(path, 0);
}

char *git_repo_name()
{
	char *path;
	if (!(path = getenv("PATH_INFO"))) {
		error("couldn't find PATH_INFO\n");
		return NULL;
	}

	return path_only_nth(path, 0);
}

char *git_root()
{
	return git_repo_name();
}

char *git_real_root()
{
	char *path;
	if (!(path = getenv("GIT_PROJECT_ROOT"))) {
		error("couldn't find GIT_PROJECT_ROOT\n");
		return NULL;
	}

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
	if (!(path = getenv("PATH_INFO"))) {
		error("couldn't find PATH_INFO\n");
		return NULL;
	}

	char *web_path;
	if (!(web_path = build_web_path(path))) {
		free(path);
		return NULL;
	}

	return web_path;
}

char *git_web_last()
{
	char *path;
	if (!(path = getenv("REQUEST_URI"))) {
		error("couldn't find REQUEST_URI\n");
		return NULL;
	}

	return path_last_elem(path);
}

char *repo_last_commit(char *path)
{
	char **cmds[] =
	{(char *[]){"git", "-C", path, "log", "-1", "--format=%ci", 0}};
	FILE *date = exgt_chain(1, cmds);

	if (!date)
		return NULL;

	size_t len = 0;
	char *line = NULL;
	if (getline(&line, &len, date) == -1) {
		error("reading last commit failed\n");
		if (line)
			free(line);
		line = NULL;
	}

	fclose(date);

	return line;
}

char *repo_real_file(char *path)
{
	char *real_root;
	if (!(real_root = getenv("GIT_PROJECT_ROOT"))) {
		error("couldn't get GIT_PROJECT_ROOT\n");
		return NULL;
	}

	return build_path(real_root, path);
}

char *repo_description(char *path)
{
	char *real = repo_real_file(path);
	if (!real)
		return NULL;

	char *desc_file = build_path(real, ".git/description");
	if (!desc_file) {
		free(real);
		return NULL;
	}

	char *desc = read_file(desc_file);
	free(desc_file);
	free(real);
	return desc;
}
