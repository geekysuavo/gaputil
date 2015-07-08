
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

/* ensure once-only inclusion. */
#ifndef __GAPUTIL_TERM_H__
#define __GAPUTIL_TERM_H__

/* include standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* include the julia library header. */
#include <julia.h>

/* include the tuple header. */
#include "tup.h"

/* define required function return values:
 *  TERM_OK: indicates success.
 *  TERM_ERR: indicates a general error.
 *  TERM_INVALID: previous sequence term is out of bounds.
 *  TERM_EXCEPTION: an error occurred during gap evalution.
 */
#define TERM_OK          1
#define TERM_ERR         0
#define TERM_INVALID    -1
#define TERM_EXCEPTION  -2

/* function declarations: */

int terminit (const char *fstr);

void termfree (void);

int term (double *x, int d, tuple_t *O, tuple_t *N, double L);

#endif /* !__GAPUTIL_TERM_H__ */

