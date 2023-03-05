#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

/**
 * @file file.c
 * File operation helper function implementations.
 */

char *read_file(const char *path)
{
	FILE *f = fopen(path, "r");
	fseek(f, 0, SEEK_END);
	size_t s = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buf = malloc(s + 1);
	fread(buf, s + 1, 1, f);
	fclose(f);
	return buf;
}
