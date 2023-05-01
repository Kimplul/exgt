/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

#ifndef EXGT_FILE_H
#define EXGT_FILE_H

/**
 * @file file.h
 * File operation helper functions.
 */

/**
 * Read a whole file into a buffer.
 * Allocates memory, remember to free the
 * buffer after use.
 *
 * @param path Path to file to read.
 * @return Contents of file in buffer.
 */
char *read_file(const char *path);

#endif /* EXGT_FILE_H */
