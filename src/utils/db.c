#include "error.h"
#include "db.h"

/**
 * @file db.c
 * Database handling implementations.
 */

PGconn *new_connection()
{
	/** @todo change database name, or let user define it */
	const char *conninfo = "dbname = bgt";
	PGconn *conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		error("couldn't open database connection: %s",
		      PQerrorMessage(conn));
		PQfinish(conn);
		return NULL;
	}

	return conn;
}

void close_connection(PGconn *conn)
{
	PQfinish(conn);
}
