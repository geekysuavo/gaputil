
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
#ifndef __NUSUTILS_EVAL_H__
#define __NUSUTILS_EVAL_H__

/* include standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* include the julia library header. */
#include <julia.h>

/* include the tuple and qrng headers. */
#include "tup.h"
#include "qrng.h"

/* define required function return values:
 *  EVAL_OK: indicates success.
 *  EVAL_ERR: indicates a general error.
 *  EVAL_INVALID: previous sequence term is out of bounds.
 *  EVAL_EXCEPTION: an error occurred during gap evalution.
 */
#define EVAL_OK          1
#define EVAL_ERR         0
#define EVAL_INVALID    -1
#define EVAL_EXCEPTION  -2

/* evaltype_t: enumerated type for which kind of evaluation engine
 * is in use by the main application.
 *  => EVAL_GAP: sampling from a gap equation.
 *  => EVAL_PDF: sampling from a density function.
 */
typedef enum {
  EVAL_GAP = 0,
  EVAL_PDF = 1
}
evaltype_t;

/* function declarations: */

int evalinit (const char *fstr, evaltype_t ftype);

void evalfree (void);

int evalgap (double *x, int d, tuple_t *O, tuple_t *N, double L);

int evalpdf (double *fx, tuple_t *x, tuple_t *N);

#endif /* !__NUSUTILS_EVAL_H__ */

