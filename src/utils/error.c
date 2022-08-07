/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <stdio.h>
#include <stdarg.h>
#include "error.h"

/**
 * @file error.c
 * Error helper implementation.
 */

int _error(const char *file, int line, const char *format, ...)
{
	fprintf(stderr, "error: %s:%d: ", file, line);
	va_list args;
	va_start(args, format);
	int ret = vfprintf(stderr, format, args);
	va_end(args);
	return ret;
}
