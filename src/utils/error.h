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
 * @param format Format of error message.
 * @return \c 0 on success, non-zero otherwise.
 */
int error(const char *format, ...);

#endif /* EXGT_ERROR_H */
