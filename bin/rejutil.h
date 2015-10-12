
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

/* include standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* include the tuple and sampling headers. */
#include "tup.h"
#include "rej.h"

/* define a soft-limit for the number of dimensions that the program
 * is willing to build grids on.
 */
#define REJUTIL_DIMS_MIN 1
#define REJUTIL_DIMS_MAX 3

/* define a short help message for users who've got no clue.
 */
#define REJUTIL_USAGE "\
 rejutil: A command-line utility for quasirandom schedule generation.\n\
 Copyright (C) 2015 Bradley Worley <geekysuavo@gmail.com>.\n\
 Released under the GNU General Public License, ver. 2.0.\n\
\n\
 Usage:\n\
  %s density N1 [N2 [N3]] densfunc\n\
\n\
 The rejection utility permits the creation of generalized quasirandom\n\
 sampling schedules based on an arbitrary density equation. The equation\n\
 specified in denfunc will be used to construct a sampling schedule on a\n\
 one-, two- or three-dimensional grid, having a global sampling density\n\
 equal to D.\n\
\n\
 For more information on how to use and/or cite the rejection utility,\n\
 please consult the manual page for rejutil(1).\n\
"

