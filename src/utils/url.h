/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

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
