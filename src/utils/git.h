#ifndef EXGT_GIT_H
#define EXGT_GIT_H

/**
 * @file git.h
 * Git helpers.
 */

/**
 * Get current page path relative to git directory. No trailing newlines.
 *
 * @return Path of file relative to current git directory.
 */
char *git_path();

/**
 * Get git commit from URL.
 *
 * @todo check if HEAD works in bare repositories.
 * @return Commit ID to use. HEAD if commit is missing from URL.
 */
char *git_commit();

/**
 * Get git repository name from URL.
 *
 * @return Git repository name, i.e. Kimplul/exgt -> exgt.
 */
char *git_repo_name();

/**
 * Get git user name from URL.
 *
 * @return Git user name, i.e. Kimplul/exgt -> Kimplul
 */
char *git_user_name();

/**
 * Get git root from URL.
 *
 * @return Git root, i.e. Kimplul/exgt/whatever -> Kimplul/exgt
 */
char *git_root();

/**
 * Get git object from URL.
 *
 * @return Git object, i.e. COMMIT:PATH.
 */
char *git_object();

/**
 * Get git root on actual filesystem.
 *
 * @return Git root on filesystem, i.e. /home/kimplul/exgt.
 */
char *git_real_root();

/**
 * Get git root on web.
 * @todo should it also contain commit info?
 *
 * @return Git root on web, i.e. /exgt/Kimplul/exgt/file.c -> /exgt/Kimplul/exgt
 */
char *git_web_root();

#endif /* EXGT_GIT_H */