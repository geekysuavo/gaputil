
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

/* include the term header. */
#include "term.h"

/* TERM_FMT: format string for all gap equation assignment statements. */
#define TERM_FMT \
  "g(x::Float64, d::Int32, O::Array, N::Array, L::Float64) = %s + 1.0;"

/* TERM_EXP_SG: expression for the sine-gap preprogrammed method. */
#define TERM_EXP_SG \
  "sinegap(x, d, O, N, L) = \
   L * sin((pi / 2) * (x + sum(O)) / sum(N))"

/* TERM_EXP_SB: expression for the sine-burst preprogrammed method. */
#define TERM_EXP_SB \
  "sineburst(x, d, O, N, L) = \
   L * sin((pi / 2) * (x + sum(O)) / sum(N)) \
     * sin((pi / 4) * N[d] * (x + sum(O)) / sum(N))^2"

/* termfn: julia function handle that holds the compiled gap equation. */
jl_function_t *termfn;

/* terminit(): initialize the gap sequence term computation environment.
 *
 * arguments:
 *  @fstr: julia function string to compile and call.
 *
 * returns:
 *  integer indicating whether initialization succeeded.
 */
int terminit (const char *fstr) {
  /* declare required variables:
   *  @stmt: gap equation assignment statement string.
   *  @nstmt: number of characters in the statement string.
   */
  char *stmt;
  int nstmt;

  /* allocate a function string to evaluate. */
  nstmt = strlen(fstr) + strlen(TERM_FMT) + 32;
  stmt = (char*) malloc(nstmt * sizeof(char));

  /* check that the string was evaluated. */
  if (!stmt)
    return TERM_ERR;

  /* build the function assignment string. */
  snprintf(stmt, nstmt, TERM_FMT, fstr);

  /* evaluate the function assignment. */
  (void) jl_eval_string(TERM_EXP_SG);
  (void) jl_eval_string(TERM_EXP_SB);
  (void) jl_eval_string(stmt);
  free(stmt);

  /* check that the evaluation succeeded. */
  if (jl_exception_occurred())
    return TERM_ERR;

  /* get the compiled function handle. */
  termfn = jl_get_function(jl_current_module, "g");

  /* return success. */
  return TERM_OK;
}

/* termfree(): allow the julia environment to free its internals.
 */
void termfree (void) {
  /* clean up the julia internals. */
  jl_atexit_hook();
}

/* term(): compute the next term in the deterministic gap sequence
 * given the current value and the sequence parameters.
 *
 * arguments:
 *  @x: pointer to the current and next sequence term.
 *  @d: current dimension of the Nyquist grid.
 *  @O: current offset position in the Nyquist grid.
 *  @N: total size of the Nyquist grid.
 *  @L: scaling factor for sequence terms.
 *
 * returns:
 *  integer indicating whether the sequence is well-behaved (1) (i.e. whether
 *  the scaling factor is in bounds) or not (0).
 */
int term (double *x, int d, tuple_t *O, tuple_t *N, double L) {
  /* declare required variables:
   *  @i: general array index and loop counter.
   *  @ret: return status value for this function.
   *  @theta: sequence term angular value.
   */
  int i, ret = TERM_OK;
  double theta;

  /* declare required julia variables:
   *  @gval: boxed return value of the gap method call.
   *  @args: array of value pointers passed to the gap method.
   *  @arrtype: data type of the arrays passed to the gap method.
   *  @arro, @arrn: arrays passed to the gap method.
   *  @dato, @datn: array data pointers.
   */
  jl_value_t *xval, *dval, *Lval, *gval;
  jl_value_t **args;
  jl_value_t *arrtype;
  jl_array_t *arro, *arrn;
  double *dato, *datn;

  /* compute the angular term value. */
  theta = (*x + tupsum(O)) / tupsum(N);

  /* determine whether the angular term is in bounds.
   *
   * this check is to ensure that the value of the provided scaling
   * factor generates a well-behaved sequence. poorly behaved
   * sequences having large scaling factors must be identified
   * in order to assign large errors to their parameters and
   * thus ensure simplex optimization succeeds.
   */
  if (theta > 1.0)
    ret = TERM_INVALID;

  /* box up the scalar arguments. */
  xval = jl_box_float64(*x);
  dval = jl_box_int32(d + 1);
  Lval = jl_box_float64(L);

  /* initialize the array data type. */
  arrtype = jl_apply_array_type(jl_float64_type, 1);

  /* allocate the origin and size arrays. */
  arro = jl_alloc_array_1d(arrtype, tupsize(O));
  arrn = jl_alloc_array_1d(arrtype, tupsize(N));

  /* access the origin and size array data pointers. */
  dato = (double*) jl_array_data(arro);
  datn = (double*) jl_array_data(arrn);

  /* fill the origin and size arrays. */
  for (i = 0; i < tupsize(O); i++) {
    dato[i] = (double) tupget(O, i);
    datn[i] = (double) tupget(N, i);
  }

  /* initialize the argument array. */
  JL_GC_PUSHARGS(args, 5);

  /* construct an argument array for the gap method call. */
  args[0] = xval;
  args[1] = dval;
  args[2] = (jl_value_t*) arro;
  args[3] = (jl_value_t*) arrn;
  args[4] = Lval;

  /* call the gap equation with the current arguments. */
  gval = jl_call(termfn, args, 5);

  /* check if an exception occurred. */
  if (jl_exception_occurred()) {
    /* output an error. */
    fprintf(stderr, "error: g(%.3lf, %d, [%u.0",
      *x, d, tupget(O, 0));
    for (i = 1; i < tupsize(O); i++)
      fprintf(stderr, ", %u.0", tupget(O, i));
    fprintf(stderr, "], [%u.0", tupget(N, 0));
    for (i = 1; i < tupsize(N); i++)
      fprintf(stderr, ", %u.0", tupget(N, i));
    fprintf(stderr, "], %.3lf) ==> %s\n", L,
      jl_typeof_str(jl_exception_occurred()));

    /* force the error to be printed. */
    fflush(stderr);

    /* return an exception status. */
    ret = TERM_EXCEPTION;
  }
  else {
    /* unbox the computed result. */
    *x += jl_unbox_float64(gval);
  }

  /* release the references to the function arguments. */
  JL_GC_POP();

  /* return the well-behaved flag. */
  return ret;
}

