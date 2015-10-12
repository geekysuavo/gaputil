
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

/* include the rejection header. */
#include "rej.h"

/* rejsamp(): compute a new multidimensional index via rejection sampling.
 *
 * arguments:
 *  @G: pointer to a quasirandom number generator structure.
 *  @pdf: array of normalized density function values.
 *  @x: pointer to the tuple to be updated.
 *  @N: pointer to the tuple of sizes.
 */
void rejsamp (qrng_t *G, double *pdf, tuple_t *x, tuple_t *N) {
  /* declare required variables:
   *  @i: dimension loop counter.
   *  @p: current grid density value.
   *  @u: uniform random deviate.
   */
  unsigned int i;
  double p, u;

  /* loop until a candidate index is accepted. */
  do {
    /* sample a new quasirandom iterate. */
    qrngeval(G);

    /* construct the grid index. */
    for (i = 0; i < tupsize(x); i++) {
      G->x[i] *= ((double) (tupget(N, i) - 1));
      tupset(x, i, (unsigned int) round(G->x[i]));
    }

    /* extract the uniform deviate. */
    u = G->x[G->n - 1];

    /* extract the density value. */
    tuppack(x, N, &i);
    p = pdf[i];
  }
  while (u > p);
}

/* rej(): generate a list of linear indices that represent the quasirandom
 * sampling schedule over a multidimensional grid, given a few input
 * parameters.
 *
 * arguments:
 *  @fn: string representation of the gap equation.
 *  @N: pointer to the tuple of Nyquist grid sizes.
 *  @d: desired sampling density.
 *  @lst: pointer to the output tuple of indices.
 *
 * returns:
 *  integer indicating whether sampling succeeded (1) or not (0).
 */
int rej (const char *fn, tuple_t *N, double d, tuple_t *lst) {
  /* declare required variables:
   *  @pdf: probability density function, evaluated on the grid.
   *  @pdfmax: largest value of @pdf over the entire data grid.
   *  @x: unpacked grid point index for density evaluation.
   *  @Tlst: binary search tree for index storage.
   *  @G: quasirandom number generator structure.
   *  @i: term generation loop counter.
   *  @n: term generation loop size.
   *  @xi: packed linear index.
   */
  unsigned int i, n, xi;
  double *pdf, pdfmax;
  bst_t *Tlst;
  tuple_t x;
  qrng_t G;

  /* initialize the output tuple. */
  tupinit(lst);

  /* initialize the binary search tree. */
  Tlst = NULL;

  /* allocate the index tuple. */
  if (!tupalloc(&x, tupsize(N))) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to allocate index tuple\n");
    return 0;
  }

  /* initialize the density function evaluation environment. */
  if (!evalinit(fn, EVAL_PDF)) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to compile density equation\n");
    return 0;
  }

  /* initialize the quasirandom number generator. */
  if (!qrngalloc(&G, tupsize(N) + 1)) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to initialize quasirandom generator\n");
    return 0;
  }

  /* allocate the density values array. */
  pdf = (double*) calloc(tupprod(N), sizeof(double));
  if (!pdf) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to allocate density values array\n");
    return 0;
  }

  /* compute the desired number of sampled grid points. */
  n = (unsigned int) round(d * (double) tupprod(N));

  /* loop over the grid points. */
  for (i = 0, pdfmax = 0.0; i < tupprod(N); i++) {
    /* unpack the linear index. */
    tupunpack(i, N, &x);

    /* evaluate the density function. */
    if (evalpdf(pdf + i, &x, N) != EVAL_OK)
      return 0;

    /* check for a new maximum value. */
    if (pdf[i] > pdfmax)
      pdfmax = pdf[i];
  }

  /* normalize the evaluated densities. */
  for (i = 0; i < tupprod(N); i++)
    pdf[i] /= pdfmax;

  /* loop over the number of grid points to compute. */
  do {
    /* sample a new value on the grid. */
    rejsamp(&G, pdf, &x, N);

    /* pack and insert the new value into the search tree. */
    tuppack(&x, N, &xi);
    Tlst = bstinsert(Tlst, xi);
  }
  while (Tlst->n < n);

  /* dump the sorted samples from the search tree. */
  bstsort(Tlst, lst);
  bstfree(Tlst);

  /* free the allocated memory. */
  qrngfree(&G);
  tupfree(&x);
  free(pdf);

  /* return success. */
  return 1;
}

