
/* gaputil: generalized gap sampling schedule generation utility.
 *
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

/* include the sorting header. */
#include "srt.h"

/* tupsort_subheapify(): repair the heap sub-tree rooted at a given index.
 *
 * arguments:
 *  @t: pointer to the tuple to sort.
 *  @n: size of the tuple heap.
 *  @i: index at which to heapify.
 */
void tupsort_subheapify (tuple_t *t, unsigned int n, unsigned int i) {
  /* declare required variables:
   *  @left, @eleft: left node index and tuple element value.
   *  @right, @eright: right node index and tuple element value.
   *  @max, @emax: maximum node index and tuple element value.
   *  @swp: temporary swap variable.
   */
  unsigned int left, right, max, eleft, eright, emax, swp;

  /* set up the indices. */
  left = (2 * i) + 1;
  right = (2 * i) + 2;
  max = i;
  swp = 0;

  /* get the values at the indices. */
  eleft = (left < n ? t->elem[left] : 0);
  eright = (right < n ? t->elem[right] : 0);
  emax = (max < n ? t->elem[max] : 0);

  /* check if a larger value exists on the left node. */
  if (left < n && eleft > emax) {
    /* swap the nodes. */
    max = left;
    emax = eleft;
  }

  /* check if a larger value exists on the right node. */
  if (right < n && eright > emax) {
    /* swap the nodes. */
    max = right;
    emax = eright;
  }

  /* check if node swaps were performed. */
  if (max != i) {
    /* swap the values in the array. */
    swp = t->elem[i];
    t->elem[i] = emax;
    t->elem[max] = swp;

    /* heapify the child subtree of the new node. */
    tupsort_subheapify(t, n, max);
  }
}

/* tupsort_heappop(): pop the topmost (largest) value off a max-heap.
 *
 * arguments:
 *  @t: pointer to the tuple to pop an element from.
 *  @n: pointer to the size of the heaped elements.
 *
 * returns:
 *  the popped tuple element.
 */
unsigned int tupsort_heappop (tuple_t *t, unsigned int *n) {
  /* declare required variables:
   *  @elem: popped element value.
   */
  unsigned int elem;

  /* retrieve the topmost tuple element. */
  elem = t->elem[0];

  /* set the new root value to that of the smallest leaf. */
  t->elem[0] = t->elem[*n - 1];
  (*n)--;

  /* re-heapify the remaining elements. */
  tupsort_subheapify(t, *n, 0);

  /* return the popped element value. */
  return elem;
}

/* tupsort_heapify(): arrange some or all of the elements of a tuple
 * into heap order.
 *
 * arguments:
 *  @t: pointer to the tuple to sort.
 *  @n: number of elements to heapify.
 */
void tupsort_heapify (tuple_t *t, unsigned int n) {
  /* declare required variables:
   *  @i: loop counter.
   */
  unsigned int i;

  /* heapify each leaf node, working back up to the root. */
  for (i = n / 2 - 1; (signed int) i >= 0; i--)
    tupsort_subheapify(t, n, i);
}

/* tupsort(): sort the elements of a tuple using the heapsort algorithm.
 *
 * arguments:
 *  @t: pointer to the tuple to sort.
 */
void tupsort (tuple_t *t) {
  /* declare required variables:
   *  @i: tuple element counter.
   *  @n: tuple heap size.
   */
  unsigned int i, n;

  /* heapify the tuple elements. */
  tupsort_heapify(t, t->n);

  /* pop all the values off the heap in sorted order. */
  for (i = 0, n = t->n; i < t->n; i++)
    t->elem[t->n - i - 1] = tupsort_heappop(t, &n);
}

/* tupuniq(): sort the elements of a tuple using the heapsort algorithm,
 * and leave only the unique elements in the tuple after sorting.
 *
 * arguments:
 *  @t: pointer to the tuple to sort.
 */
void tupuniq (tuple_t *t) {
  /* declare required variables:
   *  @i: tuple element index.
   *  @nnew: new tuple size.
   *  @ttmp: temporary tuple.
   */
  unsigned int i, nnew;
  tuple_t ttmp;

  /* duplicate the input tuple and sort its contents. */
  tupdup(&ttmp, t);
  tupsort(&ttmp);

  /* store the first tuple element without question. */
  t->elem[0] = ttmp.elem[0];
  nnew = 1;

  /* loop over the sorted tuple elements. */
  for (i = 1; i < ttmp.n; i++) {
    /* store every distinct tuple element into the final array. */
    if (ttmp.elem[i] != ttmp.elem[i - 1])
      t->elem[nnew++] = ttmp.elem[i];
  }

  /* resize the tuple element array to its new size. */
  t->elem = (unsigned int*) realloc(t->elem, nnew * sizeof(unsigned int));
  t->n = nnew;

  /* free the temporarily allocated tuple. */
  tupfree(&ttmp);
}

