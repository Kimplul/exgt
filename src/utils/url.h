/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef EXGT_URL_H
#define EXGT_URL_H

/**
 * @file url.h
 * Url helpers.
 */

/**
 * Return value associated with key.
 *
 * @param key Key to search for.
 * @return Associated value, allocated in new buffer.
 */
char *url_option(const char *key);

#endif /* EXGT_URL_H */
