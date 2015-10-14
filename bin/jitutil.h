
/* jitutil: quasirandom jittered sampling schedule generation utility.
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

/* include standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* include the tuple, sorting, jittering and evaluation headers. */
#include "tup.h"
#include "srt.h"
#include "jit.h"
#include "eval.h"

/* define a soft-limit for the number of dimensions that the program
 * is willing to build grids on.
 */
#define JITUTIL_DIMS_MIN 1
#define JITUTIL_DIMS_MAX 3

/* define a short help message for users who've got no clue.
 */
#define JITUTIL_USAGE "\
 jitutil: A command-line utility for quasirandom schedule generation.\n\
 Copyright (C) 2015 Bradley Worley <geekysuavo@gmail.com>.\n\
 Released under the GNU General Public License, ver. 2.0.\n\
\n\
 Usage:\n\
  %s density N1 [N2 [N3]] densfunc\n\
\n\
 The jittered sampling utility permits the creation of generalized\n\
 quasirandom sampling schedules based on an arbitrary density equation.\n\
 The equation specified in denfunc will be used to construct a sampling\n\
 schedule on a one-, two- or three-dimensional grid, having a global\n\
 sampling density equal to D.\n\
\n\
 For more information on how to use and/or cite the jittered sampling\n\
 utility, please consult the manual page for jitutil(1).\n\
"

