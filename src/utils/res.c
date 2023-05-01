/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

/**
 * @file res.c
 * Simple resource manager implementation.
 */

#include <stdint.h>
#include <stdlib.h>
#include "res.h"

/**
 * Grow resource manager buffer.
 * Doubles every call.
 *
 * @param r Resource manager to expand.
 */
static void res_expand(struct res *r)
{
	r->max *= 2;
	r->p = realloc(r->p, r->max * sizeof(void *));
}

struct res *res_create()
{
	struct res *r = malloc(sizeof(struct res));
	r->n = 0;
	/* arbitrary number */
	r->max = 1024;
	r->p = calloc(1, r->max * sizeof(void *));
	return r;
}

void res_add(struct res *r, void *p)
{
	if (r->n >= r->max)
		res_expand(r);

	r->p[r->n++] = p;
}

void *res_alloc(struct res *r, size_t size)
{
	void *p = malloc(size);
	if (!p)
		return NULL;

	res_add(r, p);
	return p;
}

void res_destroy(struct res *r)
{
	for (size_t i = 0; i < r->n; ++i)
		free(r->p[i]);

	free(r->p);
	free(r);
}
