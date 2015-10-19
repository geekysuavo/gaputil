
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

/* include the jittering header. */
#include "jit.h"

/* jitsearch(): locate all adjacent available indices to a given grid index.
 *
 * arguments:
 *  @black: input tuple of black-listed indices.
 *  @mask: input tuple of masking information.
 *  @x: input tuple to search out from.
 *  @N: input tuple of search grid sizes.
 *  @xadj: output tuple of adjacent indices.
 */
void jitsearch (tuple_t *black, tuple_t *mask,
                tuple_t *x, tuple_t *N,
                tuple_t *xadj) {
  /* declare required variables:
   *  @i: tuple element loop index.
   *  @si: tuple stride value.
   *  @xi: packed linear index.
   */
  unsigned int i, si, xi;

  /* pack the initial search index into a linear value. */
  tuppack(x, N, &xi);

  /* loop over all possible search directions. */
  for (i = 0; i < tupsize(x); i++) {
    /* get the linear string along the current direction. */
    si = tupstride(N, i);

    /* check if the previous point is available. */
    if (tupget(x, i) > 0 &&
        tupget(mask, xi - si) &&
        !tupsearch(black, xi - si)) {
      /* yes. append it to the list. */
      tupappend(xadj, xi - si);
    }

    /* check if the next point is available. */
    if (tupget(x, i) < tupget(N, i) - 1 &&
        tupget(mask, xi + si) &&
        !tupsearch(black, xi + si)) {
      /* yes. append it to the list. */
      tupappend(xadj, xi + si);
    }
  }
}

/* jitdist(): compute the distance of a linear index from a centroid.
 *
 * arguments:
 *  @xi: linear index from which to compute the distance.
 *  @c: values of the current centroid.
 *  @x: temporary tuple for unpacking.
 *  @N: grid size for unpacking.
 *
 * returns:
 *  squared euclidean distance from @xi to @c.
 */
double jitdist (unsigned int xi, double *c, tuple_t *x, tuple_t *N) {
  /* declare required variables:
   *  @i: tuple element index.
   *  @d: output distance.
   */
  unsigned int i;
  double d;

  /* unpack the linear index. */
  tupunpack(xi, N, x);

  /* compute the distance contributions from each index element. */
  for (i = 0, d = 0.0; i < tupsize(x); i++)
    d += pow((double) tupget(x, i) - c[i], 2.0);

  /* return the squared distance. */
  return d;
}

/* jitcent(): update the centroid of a set of linear indices.
 *
 * arguments:
 *  @Y: tuple of elements, the last of which is new.
 *  @c: values of the centroid to update.
 *  @x: temporary tuple for unpacking.
 *  @N: grid size for unpacking.
 */
void jitcent (tuple_t *Y, double *c, tuple_t *x, tuple_t *N) {
  /* declare required variables:
   *  @i: tuple element index.
   *  @xi: new linear index.
   */
  unsigned int i, xi;

  /* get the new linear index and unpack it. */
  xi = tupget(Y, tupsize(Y) - 1);
  tupunpack(xi, N, x);

  /* update each centroid value. */
  for (i = 0; i < tupsize(x); i++)
    c[i] += ((double) tupget(x, i) - c[i]) / ((double) tupsize(Y));
}

/* jitsamp(): compute a new multidimensional index via jittered sampling.
 *
 * arguments:
 *  @G: pointer to a quasirandom number generator structure.
 *  @pdf: array of normalized density function values.
 *  @pjit: target probability for jittered region selection.
 *  @mask: pointer to the tuple of available linear indices.
 *  @x: pointer to the tuple to be updated.
 *  @N: pointer to the tuple of sizes.
 *
 * returns:
 *  integer indicating whether sampling succeeded (1) or failed (0).
 */
int jitsamp (qrng_t *G, double *pdf, double pjit, tuple_t *mask,
             tuple_t *x, tuple_t *N) {
  /* declare required variables:
   *  @i: general-purpose loop index.
   *  @done: completion status of the region identification.
   *  @pcur: current probability of the jittered region.
   *  @Y: tuple of indices in the current jittered region.
   *  @Yadj: tuple of indices located via adjacency searching.
   */
  unsigned int i, imax, k, kmax, done = 0;
  double pcur, p, pmax, d, dmax, *Yc;
  tuple_t Y, Yadj;

  /* initialize the region and adjacency tuples. */
  tupinit(&Y);
  tupinit(&Yadj);

  /* allocate the centroid array. */
  Yc = (double*) calloc(tupsize(N), sizeof(double));
  if (!Yc)
    return 0;

  /* locate the most probable available index on the grid. */
  for (i = 0, imax = 0; i < tupprod(N); i++) {
    /* check for a better candidate. */
    if (tupget(mask, i) && (!tupget(mask, imax) || pdf[i] > pdf[imax]))
      imax = i;
  }

  /* ensure that a suitable index was located. */
  if (!tupget(mask, imax))
    return 1;

  /* append the index into the region tuple. */
  tupappend(&Y, imax);
  jitcent(&Y, Yc, x, N);
  pcur = pdf[imax];

  /* loop until a new jittered region has been defined. */
  while (!done) {
    /* re-initialize the adjacency tuple. */
    tupfree(&Yadj);

    /* obtain a list of available adjacent indices. */
    tupunpack(tupget(&Y, tupsize(&Y) - 1), N, x);
    jitsearch(&Y, mask, x, N, &Yadj);

    /* ensure that candidates were found. */
    if (tupsize(&Yadj) == 0)
      break;

    /* initialize the candidate search. */
    kmax = tupget(&Yadj, 0);
    pmax = pdf[kmax];

    /* compute the initial best distance to centroid. */
    dmax = jitdist(kmax, Yc, x, N);

    /* find the most probable adjacent candidate. */
    for (i = 1; i < tupsize(&Yadj); i++) {
      /* get the current probability value. */
      k = tupget(&Yadj, i);
      p = pdf[k];

      /* compute the current distance to centroid. */
      d = jitdist(k, Yc, x, N);

      /* check if the current index is a better one. */
      if (p > pmax || (p == pmax && d < dmax)) {
        /* update the best candidate grid index. */
        kmax = k;
        pmax = p;
        dmax = d;
      }
    }

    /* check if the candidate index acceptably modifies the total
     * probability of the jittered region.
     */
    if (fabs(pcur + pmax - pjit) >= fabs(pcur - pjit)) {
      /* the jittered region has been defined. */
      break;
    }

    /* add the candidate index into the jittered region. */
    tupappend(&Y, kmax);
    pcur += pdf[kmax];
    jitcent(&Y, Yc, x, N);
  }

  /* identify the largest density value in the region. */
  for (i = 0, pmax = 0.0; i < tupsize(&Y); i++) {
    /* get the current density value. */
    p = pdf[tupget(&Y, i)];

    /* update the maximum value. */
    if (p > pmax)
      pmax = p;
  }

  /* rejection sample from the jittered region. */
  do {
    /* sample a new quasirandom iterate. */
    qrngeval(G);

    /* construct the grid index. */
    G->x[0] *= ((double) (tupsize(&Y) - 1));
    imax = (unsigned int) round(G->x[0]);

    /* extract the uniform deviate. */
    d = G->x[1] * pmax;

    /* extract the density value. */
    p = pdf[tupget(&Y, imax)];
  }
  while (d > p);

  /* retrieve the highest-ranked index from the jittered region. */
  i = tupget(&Y, imax);
  tupunpack(i, N, x);

  /* mask off all indices in the jittered region. */
  for (i = 0; i < tupsize(&Y); i++)
    tupset(mask, tupget(&Y, i), 0);

  /* free the centroid array. */
  free(Yc);
  Yc = NULL;

  /* free the allocated tuples. */
  tupfree(&Y);
  tupfree(&Yadj);

  /* return success. */
  return 1;
}

/* jit(): generate a list of linear indices that represent the jittered
 * quasirandom sampling schedule over a multidimensional grid, given
 * a few input parameters.
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
int jit (const char *fn, tuple_t *N, double d, tuple_t *lst) {
  /* declare required variables:
   *  @pdf: probability density function, evaluated on the grid.
   *  @pdfsum: summed values of @pdf over the entire data grid.
   *  @x: unpacked grid point index for density evaluation.
   *  @Tlst: binary search tree for index storage.
   *  @G: quasirandom number generator structure.
   *  @i: term generation loop counter.
   *  @n: term generation loop size.
   *  @xi: packed linear index.
   */
  unsigned int i, n, xi;
  double *pdf, pdfsum;
  tuple_t x, mask;
  bst_t *Tlst;
  qrng_t G;

  /* initialize the output tuple. */
  tupinit(lst);

  /* initialize the binary search tree. */
  Tlst = NULL;

  /* allocate the index and mask tuples. */
  if (!tupalloc(&x, tupsize(N)) ||
      !tupalloc(&mask, tupprod(N))) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to allocate tuples\n");
    return 0;
  }

  /* initialize the density function evaluation environment. */
  if (!evalinit(fn, EVAL_PDF)) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to compile density equation\n");
    return 0;
  }

  /* initialize the quasirandom number generator. */
  if (!qrngalloc(&G, 2)) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to initialize quasirandom generator\n");
    return 0;
  }

  /* run the generator into a more suitable region. */
  for (i = 0; i < 100; i++)
    qrngeval(&G);

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
  for (i = 0, pdfsum = 0.0; i < tupprod(N); i++) {
    /* unpack the linear index. */
    tupunpack(i, N, &x);

    /* evaluate the density function. */
    if (evalpdf(pdf + i, &x, N) != EVAL_OK)
      return 0;

    /* sum the current value into the normalization factor. */
    pdfsum += pdf[i];
  }

  /* normalize the evaluated densities. */
  for (i = 0; i < tupprod(N); i++)
    pdf[i] /= pdfsum;

  /* compute the target probability of each sampling region. */
  pdfsum = 1.0 / ((double) n);

  /* initialize the mask. */
  tupfill(&mask, 1);

  /* loop over the number of grid points to compute. */
  for (i = 0; i < n; i++) {
    /* sample a new point from the grid. */
    if (!jitsamp(&G, pdf, pdfsum, &mask, &x, N))
      return 0;

    /* pack and insert the new value into the search tree. */
    tuppack(&x, N, &xi);
    Tlst = bstinsert(Tlst, xi);
  }

  /* dumped the sorted samples from the search tree. */
  bstsort(Tlst, lst);
  bstfree(Tlst);

  /* free the allocated tuples. */
  tupfree(&mask);
  tupfree(&x);

  /* free the quasirandom number generator and density array. */
  qrngfree(&G);
  free(pdf);

  /* return success. */
  return 1;
}

