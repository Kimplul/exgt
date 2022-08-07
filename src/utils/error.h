/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef EXGT_ERROR_H
#define EXGT_ERROR_H

/**
 * @file error.h
 * Error helper, possibly info and warnings in the future.
 */

/**
 * Print error message.
 *
 * @param file File error was called from.
 * @param line Line number error was called from.
 * @param format Format of error message.
 * @return \c 0 on success, non-zero otherwise.
 */
int _error(const char *file, int line, const char *format, ...);

/**
 * Helper for printing error messages with file/line info.
 * Depends on non-standard `##__VA_ARGS__`, but I suspect it's good enough.
 *
 * @param format Format of error message.
 * @return \c 0 on success, non-zero otherwise.
 */
#define error(format, ...) _error(__FILE__, __LINE__, format,##__VA_ARGS__)

#endif /* EXGT_ERROR_H */
