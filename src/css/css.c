/* SPDX-License-Identifier: copyleft-next-0.3.1 */
/* Copyright 2023 Kim Kuparinen < kimi.h.kuparinen@gmail.com > */

/**
 * @file css.c
 * CSS generation implementation main file.
 */

#include <stdio.h>

void css_serve()
{
	fputs("Content-type: text/css\n\n", stdout);

	/** @todo how should I store user themes? In theory I could use
	 * postgres, just add something like `theme text` to the user's entry?
	 * Then just dump the whole css portion into it? */
	/** @todo figure out css file placement, probably check first USE_CSS
	 * env variable, then something like
	 * `.`, `~/.local/share/exgt`, `/usr/share/exgt` and otherwise give up?
	 */

	/* temporary */
	FILE *css = fopen("res/styles.css", "r");
	if (!css)
		return;

	int c;
	while ((c = fgetc(css)) != EOF)
		fputc(c, stdout);
}
