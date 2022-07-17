/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file chain.h
 * Process pipe chaining header.
 */

#ifndef EXGT_CHAIN_H
#define EXGT_CHAIN_H

#include <stdio.h>
#include <stdint.h>

/**
 * Chain one or more programs together and get output as FILE.
 *
 * Equivalent to doing
 * @code
 *	prog1 | prog2 | prog3 ...
 * @endcode
 *
 * in a shell.
 *
 * @param n Number of commands to execute.
 * @param cmds Array of commands to execute.
 * @return \c stdout of last command in \p cmds.
 */
FILE *exgt_chain(size_t n, char **cmds[]);

#endif /* EXGT_CHAIN_H */
