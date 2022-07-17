/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file path.h
 * Path handling helpers.
 */

#ifndef EXGT_PATH_H
#define EXGT_PATH_H

/**
 * Build full path from \p root and \p path.
 * \c "/home" + \c "/user" => \c "/home/user"
 * The caller is responsible for freeing the string after use.
 *
 * @param root Directory \p path is relative to.
 * @param path Path relative to \p root.
 * @return New string with full path of object.
 */
char *build_path(const char *root, const char *path);

#endif /* EXGT_PATH_H */
