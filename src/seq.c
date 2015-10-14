
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

/* include the sequence header. */
#include "seq.h"

/* define constants that determine the behavior of the seq() optimizer.
 */
#define SEQ_MAX_ITER  100     /* maximum number of iterations. */
#define SEQ_EPSILON   0.005   /* error threshold of convergence. */

/* seqappend(): append a single vector of linear indices that represent
 * the deterministic gap sequence along a unidimensional path,
 * given a few input parameters.
 *
 * arguments:
 *  @N: pointer to the tuple of Nyquist grid sizes.
 *  @L: sequence term scaling factor to use during computation.
 *  @origin: origin at which to begin the sequence.
 *  @dir: direction along which to append the vector.
 *  @Tlst: pointer to the output tree of indices.
 *
 * returns:
 *  integer indicating whether the function succeeded (1) or not (0).
 */
int seqappend (tuple_t *N, double L, tuple_t *origin, unsigned int dir,
               bst_t *Tlst) {
  /* declare required variables:
   *  @xi: output packed linear sequence index.
   *  @oridx: linear index value of the origin.
   *  @stride: linear index stride from the origin.
   *  @x: current floating-point sequence term.
   *  @xend: maximum value allowed for @x.
   *  @ret: return value from the term() function.
   */
  unsigned int xi, oridx, stride;
  double x, xend;
  int ret;

  /* pack the origin into a linear index. */
  tuppack(origin, N, &oridx);

  /* compute the linear stride along the current direction. */
  stride = tupstride(N, dir);

  /* compute the maximum allowed sequence value. */
  xend = (double) tupget(N, dir) - (double) tupget(origin, dir);

  /* initialize the sequence value. */
  x = 0.0;

  /* loop over the terms of the sequence. */
  do {
    /* compute the next term in the sequence, but return failure
     * if the sequence is not well-behaved.
     */
    ret = evalgap(&x, dir, origin, N, L);
    if (ret != EVAL_OK)
      return ret;

    /* ensure the computed term is in bounds. */
    if (round(x) <= xend) {
      /* compute the new index value. */
      xi = oridx + stride * (unsigned int) round(x - 1.0);

      /* insert the new value to the output search tree. */
      bstinsert(Tlst, xi);
    }
  }
  while (round(x) <= xend);

  /* return success. */
  return 1;
}

/* seqfn(): generate a sub-sequence of deterministic gap samples originating
 * from a specified point and filling a specified region. recursively
 * calls itself until the lowest level (a single vector of samples)
 * is reached at each (origin, mask) pair.
 *
 * arguments:
 *  @N: pointer to the tuple of Nyquist grid sizes.
 *  @L: sequence term scaling factor to use during computation.
 *  @origin: current origin from which to generate subsequences.
 *  @mask: current available dimensions for new subsequences.
 *  @Tlst: pointer to the output tree of indices.
 *
 * returns:
 *  integer indicating whether sub-sequence generation
 *  succeeded (1) or not (0).
 */
int seqfn (tuple_t *N, double L, tuple_t *origin, tuple_t *mask,
           bst_t *Tlst) {
  /* declare required variables:
   *  @i: general-purpose loop index.
   *  @pos: offset position of the current sub-sequence.
   *  @dir: pivot direction of the current sub-sequence.
   *  @done: whether the current branch of recursion is complete.
   *  @suborigin: origin tuple passed to the next level of recursion.
   *  @submask: mask tuple passed to the next level of recursion.
   *  @ret: return value from the next level of recursion.
   */
  unsigned int i, pos, dir, done;
  tuple_t suborigin, submask;
  int ret;

  /* determine whether we've reached a leaf on the recursion tree. */
  if (tupsum(mask) == 1) {
    /* determine the append direction. */
    dir = tupfind(mask) - 1;

    /* drop a single vector of sequence terms down. */
    return seqappend(N, L, origin, dir, Tlst);
  }

  /* allocate the sub-level origin and mask tuples. */
  if (!tupalloc(&suborigin, tupsize(origin)) ||
      !tupalloc(&submask, tupsize(mask)))
    return 0;

  /* loop through the current branch of recursion. */
  for (pos = 0, done = 0; !done; pos++) {
    /* loop over each dimension at the current offset position. */
    for (dir = 0, done = 1; dir < tupsize(mask); dir++) {
      if (!tupget(mask, dir) || pos >= tupget(N, dir))
        continue;

      /* if execution reaches this point, we have not exhausted all
       * positions at this level of recursion.
       */
      done = 0;

      /* set the elements of the sub-level mask tuple. */
      for (i = 0; i < tupsize(mask); i++)
        tupset(&submask, i, i != dir && tupget(mask, i) ? 1 : 0);

      /* set the elements of the sub-level origin tuple. */
      for (i = 0; i < tupsize(origin); i++)
        tupset(&suborigin, i, i == dir ? pos : tupget(origin, i));

      /* execute this function at a lower level of recursion. */
      ret = seqfn(N, L, &suborigin, &submask, Tlst);

      /* check that execution succeeded. */
      if (ret != EVAL_OK)
        return ret;
    }
  }

  /* free the allocated tuples. */
  tupfree(&suborigin);
  tupfree(&submask);

  /* return success. */
  return 1;
}

/* seq(): generate a sequence of linear indices that represent the
 * deterministic gap sequence over a multidimensional grid,
 * given a few input parameters.
 *
 * arguments:
 *  @fn: string representation of the gap equation.
 *  @N: pointer to the tuple of Nyquist grid sizes.
 *  @d: desired sampling density.
 *  @lst: pointer to the output tuple of indices.
 *
 * returns:
 *  integer indicating whether sequence generation succeeded (1) or not (0).
 */
int seq (const char *fn, tuple_t *N, double d, tuple_t *lst) {
  /* declare required variables:
   *  @origin: top-level origin tuple for recursion.
   *  @mask: top-level mask tuple for recursion.
   *  @n: target number of generated sequence terms.
   *  @nout: number of generated sequence terms.
   *  @nerr: discrepancy between desired and generated point counts.
   *  @ntol: tolerable discrepancy value of schedules.
   *  @ret: return value from the seqfn() call.
   *  @iter: optimization iteration counter.
   *  @Tlst: binary search tree for index storage.
   *  @L: sequence term scaling factor to optimize.
   *  @w: weight applied to optimize the scaling factor.
   */
  int n, nout, nerr, ntol, ret;
  tuple_t origin, mask;
  unsigned int iter;
  bst_t *Tlst;
  double L, w;

  /* initialize the output tuple. */
  tupinit(lst);

  /* initialize the binary search tree. */
  Tlst = NULL;

  /* allocate the top-level tuples. */
  if (!tupalloc(&origin, tupsize(N)) || !tupalloc(&mask, tupsize(N)))
    return 0;

  /* initialize the gap equation evaluation environment. */
  if (!evalinit(fn, EVAL_GAP)) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to compile gap equation\n");
    return 0;
  }

  /* compute the desired number of sampled grid points. */
  n = (int) round(d * (double) tupprod(N));

  /* compute the tolerated point count error. */
  ntol = (int) round(SEQ_EPSILON * (double) n);
  ntol = (ntol < 1 ? 1 : ntol);

  /* compute an initial guess for the scaling factor, as one less
   * the inverse of the sampling density.
   */
  L = (1.0 / d) - 1.0;
  w = 1.0;

  /* initialize the iteration counter. */
  iter = 0;

  /* loop until the sequence size matches the desired sample count. */
  do {
    /* initialize the top-level tuples. */
    tupfill(&origin, 0);
    tupfill(&mask, 1);

    /* initialize the output tuple. */
    tupfree(lst);

    /* initialize the binary search tree. */
    bstfree(Tlst);
    Tlst = bstalloc();

    /* call the recursive sequence generation function. */
    ret = seqfn(N, L * w, &origin, &mask, Tlst);

    /* check the function's return value. */
    if (ret == EVAL_OK) {
      /* the function succeeded: the sequence is well-behaved. */
      nout = (signed int) Tlst->n;
    }
    else if (ret == EVAL_INVALID) {
      /* the function failed: the sequence is poorly behaved. */
      nout = (signed int) tupprod(N);
    }
    else if (ret == EVAL_EXCEPTION) {
      /* the julia function call failed. */
      fprintf(stderr, "error: failed to evaluate gap equation\n");
      return 0;
    }
    else {
      /* unknown error. */
      fprintf(stderr, "error: unknown failure\n");
      return 0;
    }

    /* compute the difference from the desired point count. */
    nerr = nout - (signed int) n;

    /* adjust the scaling factor by an amount proportional to the error. */
    w *= (1.0 + 0.5 * (double) nerr / (double) n);
  }
  while (abs(nerr) > ntol && ++iter < SEQ_MAX_ITER);

  /* dump the sorted indices from the search tree. */
  bstsort(Tlst, lst);
  bstfree(Tlst);

  /* free the top-level tuples. */
  tupfree(&origin);
  tupfree(&mask);

  /* return success. */
  return 1;
}

