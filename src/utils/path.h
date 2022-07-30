/* SPDX-License-Identifier: GPL-3.0-or-later */

/**
 * @file path.h
 * Path handling helpers.
 */

#ifndef EXGT_PATH_H
#define EXGT_PATH_H

/**
 * Build full path from \p root and \p path.
 * Example: @code "/home" + "/user" => "/home/user" @endcode
 * The caller is responsible for freeing the string after use.
 *
 * @param root Directory \p path is relative to.
 * @param path Path relative to \p root.
 * @return New string with full path of object.
 */
char *build_path(const char *root, const char *path);

/**
 * Skip first elements in path, starting from nth element.
 *
 * Example: @code path = /exgt/Kimplul/exgt, n = 0 => /exgt/Kimplul @endcode
 *
 * @param path Path to skip elements in.
 * @param n Index of element to skip last.
 * @return Path with nth element skipped. \c NULL if \p n >= number of elements
 * in path.
 */
char *path_skip_nth(const char *path, size_t n);

/**
 * Get the nth element in path.
 *
 * Example: @code path = /exgt/Kimplul/exgt, n = 0 => /exgt @endcode
 *
 * @param path Path to extract nth element from.
 * @param n Index of element to extract.
 * @return Nth element in \p path. \c NULL if \p n >= number of elements in
 * path.
 */
char *path_only_nth(const char *path, size_t n);

/**
 * Cut everything following the nth element.
 *
 * Example: @code path = /exgt/Kimplul/exgt, n = 1 => /exgt/Kimplul @endcode
 *
 * @param path Path to cut elements from.
 * @param n Index of element to cut after.
 * @return Path with everything following nth element removed. \c NULL if \p n
 * >= number of elements in path.
 */
char *path_cut_nth(const char *path, size_t n);

#endif /* EXGT_PATH_H */
