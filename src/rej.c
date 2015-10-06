
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

/* REJ_FMT: format string for all density equation assignment statements. */
#define REJ_FMT \
  "f(x::Array, N::Array) = %s;"

/* rejfn: julia function handle that holds the compiled equation. */
jl_function_t *rejfn;

/* rejinit(): initialize the density function computation environment.
 *
 * arguments:
 *  @fstr: julia function string to compile and call.
 *
 * returns:
 *  integer indicating whether initialization succeeded.
 */
int rejinit (const char *fstr) {
  /* declare required variables:
   *  @stmt: density function assignment statement string.
   *  @nstmt: number of characters in the statement string.
   */
  char *stmt;
  int nstmt;

  /* allocate a function string to evaluate. */
  nstmt = strlen(fstr) + strlen(REJ_FMT) + 32;
  stmt = (char*) malloc(nstmt * sizeof(char));

  /* check that the string was evaluated. */
  if (!stmt)
    return REJ_ERR;

  /* build the function assignment string. */
  snprintf(stmt, nstmt, REJ_FMT, fstr);

  /* evaluate the function assignment. */
  (void) jl_eval_string(stmt);
  free(stmt);

  /* check that the evaluation succeeded. */
  if (jl_exception_occurred())
    return REJ_ERR;

  /* get the compiled function handle. */
  rejfn = jl_get_function(jl_current_module, "f");

  /* return success. */
  return REJ_OK;
}

/* rejfree(): allow the julia environment to free its internals.
 */
void rejfree (void) {
  /* clean up the julia internals. */
  jl_atexit_hook(0);
}

/* rejeval(): compute the density function at a given grid point.
 *
 * arguments:
 *  @x: pointer to the current grid index.
 *  @N: total size of the Nyquist grid.
 *
 * returns:
 *  floating point result.
 */
double rejeval (tuple_t *x, tuple_t *N) {
  /* declare required variables:
   *  @i: general array index and loop counter.
   *  @fval: computed density function value.
   */
  double fval = 0.0;
  int i;

  /* declare required julia variables:
   *  @boxfval: boxed return value of the method call.
   *  @args: array of value pointers passed to the method.
   *  @arrtype: data type of the arrays passed to the method.
   *  @arrx, @arrn: arrays passed to the method.
   *  @datx, @datn: array data pointers.
   */
  jl_value_t *boxfval;
  jl_value_t **args;
  jl_value_t *arrtype;
  jl_array_t *arrx, *arrn;
  double *datx, *datn;

  /* initialize the array data type. */
  arrtype = jl_apply_array_type(jl_float64_type, 1);

  /* allocate the origin and size arrays. */
  arrx = jl_alloc_array_1d(arrtype, tupsize(x));
  arrn = jl_alloc_array_1d(arrtype, tupsize(N));

  /* access the origin and size array data pointers. */
  datx = (double*) jl_array_data(arrx);
  datn = (double*) jl_array_data(arrn);

  /* fill the origin and size arrays. */
  for (i = 0; i < tupsize(x); i++) {
    datx[i] = (double) tupget(x, i);
    datn[i] = (double) tupget(N, i);
  }

  /* initialize the argument array. */
  JL_GC_PUSHARGS(args, 2);

  /* construct an argument array for the method call. */
  args[0] = (jl_value_t*) arrx;
  args[1] = (jl_value_t*) arrn;

  /* call the gap equation with the current arguments. */
  boxfval = jl_call(rejfn, args, 2);

  /* check if an exception occurred. */
  if (jl_exception_occurred()) {
    /* output an error. */
    fprintf(stderr, "error: f([%u.0",
      tupget(x, 0));
    for (i = 1; i < tupsize(x); i++)
      fprintf(stderr, ", %u.0", tupget(x, i));
    fprintf(stderr, "], [%u.0", tupget(N, 0));
    for (i = 1; i < tupsize(N); i++)
      fprintf(stderr, ", %u.0", tupget(N, i));
    fprintf(stderr, "]) ==> %s\n",
      jl_typeof_str(jl_exception_occurred()));

    /* force the error to be printed. */
    fflush(stderr);
  }
  else {
    /* unbox the computed result. */
    fval = jl_unbox_float64(boxfval);
  }

  /* release the references to the function arguments. */
  JL_GC_POP();

  /* return the well-behaved flag. */
  return fval;
}

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
    for (i = 0; i < tupsize(x); i++)
      tupset(x, i, (unsigned int) round(G->x[i] * ((double) tupget(N, i))));

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
   *  @G: quasirandom number generator structure.
   *  @i: term generation loop counter.
   *  @n: term generation loop size.
   */
  unsigned int i, n, xi;
  double *pdf, pdfmax;
  tuple_t x;
  qrng_t G;

  /* initialize the output tuple. */
  tupinit(lst);
  tupappend(lst, 0);

  /* allocate the index tuple. */
  if (!tupalloc(&x, tupsize(N))) {
    /* output an error message and return failure. */
    fprintf(stderr, "error: failed to allocate index tuple\n");
    return 0;
  }

  /* initialize the function computation environment. */
  if (!rejinit(fn)) {
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
    pdf[i] = rejeval(&x, N);

    /* check for a new maximum value. */
    if (pdf[i] > pdfmax)
      pdfmax = pdf[i];
  }

  /* normalize the evaluated densities. */
  for (i = 0; i < tupprod(N); i++)
    pdf[i] /= pdfmax;

  /* loop over the number of grid points to compute. */
  for (i = 0; i < n; i++) {
    /* sample a new value on the grid. */
    rejsamp(&G, pdf, &x, N);

    /* pack and append the new value into the output list. */
    tuppack(&x, N, &xi);
    tupappend(lst, xi);
  }

  /* free the allocated memory. */
  qrngfree(&G);
  tupfree(&x);
  free(pdf);

  /* return success. */
  return 1;
}

