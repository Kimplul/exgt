#ifndef EXGT_DB_H
#define EXGT_DB_H

/**
 * @file db.h
 * Database handling helpers. If I eventually decide to allow the user to select
 * which database to use, these might come in handy.
 */

#include <postgresql/libpq-fe.h>

/**
 * Create new (PostgreSQL) connection.
 *
 * @return New connection when succesful, \c NULL otherwise.
 */
PGconn *new_connection();

/**
 * Close (PostgreSQL) connection.
 *
 * @param conn Database connection.
 */
void close_connection(PGconn *conn);

#endif /* EXGT_DB_H */
