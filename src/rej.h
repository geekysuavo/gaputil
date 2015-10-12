
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

/* ensure once-only inclusion. */
#ifndef __GAPUTIL_REJ_H__
#define __GAPUTIL_REJ_H__

/* include standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* include the julia library header. */
#include <julia.h>

/* include the tuple, search tree and qrng headers. */
#include "tup.h"
#include "bst.h"
#include "qrng.h"

/* define required function return values:
 *  REJ_OK: indicates success.
 *  REJ_ERR: indicates a general error.
 *  REJ_INVALID: previous sequence term is out of bounds.
 *  REJ_EXCEPTION: an error occurred during gap evalution.
 */
#define REJ_OK          1
#define REJ_ERR         0
#define REJ_INVALID    -1
#define REJ_EXCEPTION  -2

/* function declarations: */

void rejfree (void);

int rej (const char *fn, tuple_t *N, double d, tuple_t *lst);

#endif /* !__GAPUTIL_REJ_H__ */

