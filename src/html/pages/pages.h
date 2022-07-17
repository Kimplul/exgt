#ifndef EXGT_PAGES_H
#define EXGT_PAGES_H

#include <stdio.h>

/**
 * Serve error page.
 *
 * @param code Status code.
 * @param msg Error message.
 */
void error_serve(FILE *file, int code, const char *msg);

/** Serve landing page. */
void index_serve(FILE *file);

/** Serve one file page. */
void file_serve(FILE *file);

/** Serve one directory page. */
void dir_serve(FILE *file);

/* Not entirely sure which features I want to implement, but here are a few
 * possibilities
 *
 * void issue_serve();
 * void pr_serve();
 * void commit_serve();
 * void hist_serve();
 * void wiki_serve();
 */

#endif /* EXGT_PAGES_H */
