/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef EXGT_RES_H
#define EXGT_RES_H

/**
 * Very simple resource manager.
 * Keeps track of all allocations and frees them all at once.
 */
struct res {
	/** Number of allocations. */
	size_t n;
	/** Maximum number of allocations. */
	size_t max;
	/** Pointer to array of pointers to allocations. */
	void **p;
};

/**
 * Create new resource manager.
 *
 * @return Pointer to resource manager.
 */
struct res *res_create();

/**
 * Resource manager wrapper around malloc().
 *
 * @param r Resource manager.
 * @param size Size of allocation.
 * @return Pointer to newly allocated area.
 */
void *res_alloc(struct res *r, size_t size);


/**
 * Destroy resource manager and free all associated allocations.
 *
 * @param r Resource manager to destroy.
 */
void res_destroy(struct res *r);

#endif /* EXGT_RES_H */
