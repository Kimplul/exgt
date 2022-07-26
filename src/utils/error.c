#include <stdio.h>
#include <stdarg.h>
#include "error.h"

/**
 * @file error.c
 * Error helper implementation.
 */

int error(const char *format, ...)
{
	fprintf(stderr, "error: ");
	va_list args;
	va_start(args, format);
	int ret = vfprintf(stderr, format, args);
	va_end(args);
	return ret;
}
