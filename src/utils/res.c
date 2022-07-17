/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <stdint.h>
#include <stdlib.h>
#include "res.h"

static void res_expand(struct res *r)
{
	r->max *= 2;
	r->p = realloc(r->p, r->max);
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

void *res_alloc(struct res *r, size_t size)
{
	if (r->n >= r->max)
		res_expand(r);

	void *p = malloc(size);
	r->p[r->n++] = p;
	return p;
}

void res_destroy(struct res *r)
{
	for (size_t i = 0; i < r->n; ++i)
		free(r->p[i]);

	free(r);
}
