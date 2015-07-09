
/* gaputil: generalized gap sampling schedule generation utility.
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

/* include the tuple header. */
#include "tup.h"

/* tupalloc(): allocate memory for an n-tuple.
 *
 * arguments:
 *  @t: pointer to the tuple to allocate.
 *  @n: size of the tuple.
 *
 * returns:
 *  integer indicating whether allocation succeeded (1) or failed (0).
 */
int tupalloc (tuple_t *t, unsigned int n) {
  /* ensure the tuple pointer is valid. */
  if (!t)
    return 0;

  /* allocate the element array. */
  t->elem = (unsigned int*) calloc(n, sizeof(unsigned int));
  if (!t->elem)
    return 0;

  /* store the tuple size. */
  t->n = n;

  /* return success. */
  return 1;
}

/* tupdup(): copy the contents of a tuple into a new tuple structure.
 *
 * arguments:
 *  @tdst: destination tuple structure pointer.
 *  @tsrc: source tuple structure pointer.
 *
 * returns:
 *  integer indicating whether duplication succeeded (1) or failed (0).
 */
int tupdup (tuple_t *tdst, tuple_t *tsrc) {
  /* allocate the destination tuple. */
  if (!tupalloc(tdst, tsrc->n))
    return 0;

  /* copy the tuple contents. */
  memcpy(tdst->elem, tsrc->elem, tsrc->n * sizeof(unsigned int));

  /* return success. */
  return 1;
}

/* tupinit(): initialize the fields of a tuple structure.
 *
 * arguments:
 *  @t: pointer to the tuple to initialize.
 */
void tupinit (tuple_t *t) {
  /* ensure the tuple pointer is valid. */
  if (!t)
    return;

  /* initialize the tuple structure members. */
  t->elem = NULL;
  t->n = 0;
}

/* tupfree(): free allocated memory from an n-tuple.
 *
 * arguments:
 *  @t: pointer to the tuple to free.
 */
void tupfree (tuple_t *t) {
  /* ensure the tuple pointer is valid. */
  if (!t)
    return;

  /* free the element array, if it is allocated. */
  if (t->n && t->elem)
    free(t->elem);

  /* re-initialize the tuple. */
  tupinit(t);
}

/* tupfill(): set all elements in a tuple to a given value.
 *
 * arguments:
 *  @t: pointer to the tuple to modify.
 *  @value: new element value.
 */
void tupfill (tuple_t *t, unsigned int value) {
  /* declare required variables:
   *  @i: tuple element index.
   */
  unsigned int i;

  /* ensure the tuple pointer is valid. */
  if (!t)
    return;

  /* loop over the tuple elements. */
  for (i = 0; i < t->n; i++)
    tupset(t, i, value);
}

/* tupset(): set a specified element in a tuple.
 *
 * arguments:
 *  @t: pointer to the tuple to modify.
 *  @i: element index to modify.
 *  @value: new element value.
 */
void tupset (tuple_t *t, unsigned int i, unsigned int value) {
  /* set the tuple element, if possible. */
  if (t && i < t->n)
    t->elem[i] = value;
}

/* tupget(): get a specified element from a tuple.
 *
 * arguments:
 *  @t: pointer to the tuple to query.
 *  @i: element index to retrieve.
 *
 * returns:
 *  the requested tuple element, or zero if either the tuple pointer is null
 *  or the element index is out of bounds.
 */
unsigned int tupget (tuple_t *t, unsigned int i) {
  /* return the tuple element, or zero if the pointer or index are invalid. */
  return (t && i < t->n ? t->elem[i] : 0);
}

/* tupsize(): get the number of elements in a tuple.
 *
 * arguments:
 *  @t: pointer to the tuple to query.
 *
 * returns:
 *  the tuple element count, or zero if the tuple pointer is null.
 */
unsigned int tupsize (tuple_t *t) {
  /* return the size of the tuple, or zero if the pointer is invalid. */
  return (t ? t->n : 0);
}

/* tupprint(): write a tuple to a file stream.
 *
 * arguments:
 *  @t: pointer to the input tuple.
 *  @fh: output file stream pointer.
 */
void tupprint (tuple_t *t, FILE *fh) {
  /* declare a required variable.
   *  @i: tuple element index.
   */
  unsigned int i;

  /* ensure the tuple pointer is valid. */
  if (!t)
    return;

  /* loop over the tuple elements. */
  for (i = 0; i < t->n; i++) {
    /* print a spacer, if necessary. */
    if (i > 0)
      fprintf(fh, " ");

    /* print the current element. */
    fprintf(fh, "%u", t->elem[i]);
  }

  /* print a newline. */
  fprintf(fh, "\n");
}

/* tuppack(): pack the values of an n-tuple into a linear index.
 *
 * arguments:
 *  @t: pointer to the input tuple.
 *  @n: pointer to the tuple of sizes.
 *  @idx: pointer to the output index.
 *
 * returns:
 *  integer indicating whether packing occurred (1) or not (0).
 */
int tuppack (tuple_t *t, tuple_t *n, unsigned int *idx) {
  /* declare required variables:
   *  @i: tuple element index.
   *  @stride: current element stride.
   */
  unsigned int i, stride;

  /* ensure the pointers are valid. */
  if (!t || !n || !idx)
    return 0;

  /* ensure the tuples are allocated. */
  if (!t->elem || !n->elem)
    return 0;

  /* ensure the tuple sizes match. */
  if (t->n != n->n)
    return 0;

  /* loop over the tuple elements. */
  for (i = 0, stride = 1, *idx = 0; i < t->n; i++) {
    /* add the current element into the linear index. */
    *idx += t->elem[i] * stride;
    stride *= n->elem[i];
  }

  /* return success. */
  return 1;
}

/* tupunpack(): unpack a linear index into an n-tuple.
 *
 * arguments:
 *  @idx: value of the input index.
 *  @n: pointer to the tuple of sizes.
 *  @t: pointer to the output tuple.
 *
 * returns:
 *  integer indicating whether unpacking occurred (1) or not (0).
 */
int tupunpack (unsigned int idx, tuple_t *n, tuple_t *t) {
  /* declare required variables:
   *  @i: tuple element index..
   *  @redidx: reduced linear index.
   */
  unsigned int i, redidx;

  /* ensure the pointers are valid. */
  if (!t || !n)
    return 0;

  /* ensure the tuples are allocated. */
  if (!t->elem || !n->elem)
    return 0;

  /* ensure the tuple sizes match. */
  if (t->n != n->n)
    return 0;

  /* loop over the tuple elements. */
  for (i = 0, redidx = idx; i < t->n; i++) {
    /* extract the current element. */
    t->elem[i] = redidx % n->elem[i];

    /* reduce the linear index by the current stride. */
    redidx = (redidx - t->elem[i]) / n->elem[i];
  }

  /* return success. */
  return 1;
}

/* tupstride(): compute the stride of the packed values in an index array
 * along a given direction.
 *
 * arguments:
 *  @sz: pointer to the tuple of sizes.
 *  @dir: direction to calculate stride.
 *
 * returns:
 *  the computed stride value, or 0 on failure.
 */
unsigned int tupstride (tuple_t *sz, unsigned int dir) {
  /* declare required variables:
   *  @i: tuple element index.
   *  @stride: computed stride value.
   */
  unsigned int i, stride;

  /* ensure the tuple pointer is valid. */
  if (!sz)
    return 0;

  /* loop over the tuple elements. */
  for (i = 0, stride = 1; i < dir; i++)
    stride *= sz->elem[i];

  /* return the computed result. */
  return stride;
}

/* tupsum(): compute the sum of all elements in a tuple.
 *
 * arguments:
 *  @t: pointer to the input tuple.
 *
 * returns:
 *  the sum of all tuple elements, or 0 on failure.
 */
unsigned int tupsum (tuple_t *t) {
  /* declare required variables:
   *  @i: tuple element index.
   *  @sum: tuple element sum.
   */
  unsigned int i, sum;

  /* ensure the tuple pointer is valid. */
  if (!t)
    return 0;

  /* loop over the tuple elements. */
  for (i = 0, sum = 0; i < t->n; i++)
    sum += t->elem[i];

  /* return the computed result. */
  return sum;
}

/* tupprod(): compute the product of all elements in a tuple.
 *
 * arguments:
 *  @t: pointer to the input tuple.
 *
 * returns:
 *  the product of all tuple elements, or 0 on failure.
 */
unsigned int tupprod (tuple_t *t) {
  /* declare required variables:
   *  @i: tuple element index.
   *  @prod: tupe element product.
   */
  unsigned int i, prod;

  /* ensure the tuple pointer is valid. */
  if (!t)
    return 0;

  /* loop over the tuple elements. */
  for (i = 0, prod = 1; i < t->n; i++)
    prod *= t->elem[i];

  /* return the computed result. */
  return prod;
}

/* tupfind(): find the first nonzero element of a tuple.
 *
 * arguments:
 *  @t: pointer to the input tuple.
 *
 * returns:
 *  one-based index of the first nonzero tuple element, or (0) on failure or
 *  all zeros.
 */
unsigned int tupfind (tuple_t *t) {
  /* declare required variables:
   *  @i: tuple element index.
   */
  unsigned int i;

  /* ensure the tuple pointer is valid. */
  if (!t)
    return 0;

  /* loop over the tuple elements. */
  for (i = 0; i < t->n; i++) {
    /* return the index of the first nonzero element encountered. */
    if (t->elem[i])
      return i + 1;
  }

  /* return failure. */
  return 0;
}

/* tupappend(): append a new value at the end of a tuple.
 *
 * arguments:
 *  @t: pointer to the tuple to modify.
 *  @newelem: new value to append.
 *
 * returns:
 *  integer indicating whether the append succeeded (1) or failed (0).
 */
int tupappend (tuple_t *t, unsigned int newelem) {
  /* declare required variables:
   *  @nnew: new number of tuple elements.
   */
  unsigned int nnew;

  /* ensure the tuple pointer is valid. */
  if (!t)
    return 0;

  /* compute the new tuple size. */
  nnew = t->n + 1;

  /* reallocate the tuple element array. */
  t->elem = (unsigned int*) realloc(t->elem, nnew * sizeof(unsigned int));
  if (!t->elem)
    return 0;

  /* store the new tuple element and size. */
  t->elem[nnew - 1] = newelem;
  t->n = nnew;

  /* return success. */
  return 1;
}

