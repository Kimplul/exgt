/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

/**
 * @file chain.c
 * Process pipe chaining implementation.
 */

#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "chain.h"

/** Environment pointer. Weird that you have to manually define but eh. */
extern char **environ;

FILE *exgt_chain(size_t n, char **cmds[])
{
	int out = 0;
	int cout_pipe[2];
	for (size_t i = 0; i < n; ++i) {
		if (pipe(cout_pipe)) {
			perror("pipe failed");
			return NULL;
		}

		posix_spawn_file_actions_t actions;
		posix_spawn_file_actions_init(&actions);
		posix_spawn_file_actions_addclose(&actions, cout_pipe[0]);
		posix_spawn_file_actions_adddup2(&actions, cout_pipe[1],
		                                 STDOUT_FILENO);
		posix_spawn_file_actions_addclose(&actions, cout_pipe[1]);

		if (out) {
			posix_spawn_file_actions_adddup2(&actions, out,
			                                 STDIN_FILENO);
			posix_spawn_file_actions_addclose(&actions, out);
		}

		int pid;
		if (posix_spawnp(&pid, cmds[i][0], &actions, NULL, cmds[i],
		                 environ)) {
			perror(cmds[i][0]);
			return NULL;
		}

		posix_spawn_file_actions_destroy(&actions);

		if (out)
			close(out);

		close(cout_pipe[1]);
		out = cout_pipe[0];
	}

	return fdopen(out, "r");
}
