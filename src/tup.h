
/* nusutils: generalized deterministic nonuniform sampling utilities.
 * Copyright (C) 2015 Bradley Worley <geekysuavo@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to:
 *
 *   Free Software Foundation, Inc.
 *   51 Franklin Street, Fifth Floor
 *   Boston, MA  02110-1301, USA.
 */

/* ensure once-only inclusion. */
#ifndef __NUSUTILS_TUP_H__
#define __NUSUTILS_TUP_H__

/* include standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* tuple_t: type definition of an n-tuple of unsigned integers.
 */
typedef struct {
  /* @n: number of tuple elements. */
  unsigned int n;

  /* @elem: array of tuple elements. */
  unsigned int *elem;
}
tuple_t;

/* function declarations: */

int tupalloc (tuple_t *t, unsigned int n);

int tupdup (tuple_t *tdst, tuple_t *tsrc);

void tupinit (tuple_t *t);

void tupfree (tuple_t *t);

void tupfill (tuple_t *t, unsigned int value);

void tupset (tuple_t *t, unsigned int i, unsigned int value);

unsigned int tupget (tuple_t *t, unsigned int i);

unsigned int tupsize (tuple_t *t);

void tupprint (tuple_t *t, FILE *fh);

int tuppack (tuple_t *t, tuple_t *n, unsigned int *idx);

int tupunpack (unsigned int idx, tuple_t *n, tuple_t *t);

unsigned int tupstride (tuple_t *sz, unsigned int dir);

unsigned int tupsum (tuple_t *t);

unsigned int tupprod (tuple_t *t);

unsigned int tupfind (tuple_t *t);

unsigned int tupsearch (tuple_t *t, unsigned int idx);

int tupappend (tuple_t *t, unsigned int newelem);

#endif /* !__NUSUTILS_TUP_H__ */

