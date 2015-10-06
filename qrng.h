
/* rejutil: quasirandom rejection sampling schedule generation utility.
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
#ifndef __GAPUTIL_QRNG_H__
#define __GAPUTIL_QRNG_H__

/* include standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include the tuple header. */
#include "tup.h"

/* qrng_t: type definition of an n-dimensional quasirandom number generator.
 */
typedef struct {
  /* @n: number of random variates per iteration. */
  unsigned int n;

  /* @bv: array of relatively prime bases.
   * @sv: current state of the quasirandom sequence.
   */
  unsigned int *bv;
  unsigned int **sv;

  /* @x: array of quasirandom iterates.
   */
  double *x;
}
qrng_t;

/* function declarations: */

int qrngalloc (qrng_t *g, unsigned int n);

void qrngfree (qrng_t *g);

void qrngeval (qrng_t *g);

#endif /* !__GAPUTIL_QRNG_H__ */

