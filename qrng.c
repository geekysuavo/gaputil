
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

/* include the qrng header. */
#include "qrng.h"

/* define the size of each qrng_t state array.
 */
#define QRNG_MAX 1000

/* qrngalloc(): allocate memory for (and initialize) a quasirandom number
 * generator structure.
 *
 * arguments:
 *  @g: pointer to the generator to allocate.
 *  @n: number of elements in the qrng.
 *
 * returns:
 *  integer indicating whether allocation (and initialization) succeeded (1)
 *  or failed (0).
 */
int qrngalloc (qrng_t *g, unsigned int n) {
  /* declare required variables:
   *  @i, @j: state indices.
   */
  unsigned int i, j;

  /* ensure the tuple pointer is valid. */
  if (!g)
    return 0;

  /* allocate the iterate array. */
  g->x = (double*) calloc(n, sizeof(double));
  if (!g->x)
    return 0;

  /* allocate the base array. */
  g->bv = (unsigned int*) calloc(n, sizeof(unsigned int));
  if (!g->bv)
    return 0;

  /* allocate the state array. */
  g->sv = (unsigned int**) calloc(n, sizeof(unsigned int*));
  if (!g->sv)
    return 0;

  /* allocate the individual states. */
  for (i = 0; i < n; i++) {
    /* allocate the state. */
    g->sv[i] = (unsigned int*) calloc(QRNG_MAX, sizeof(unsigned int));
    if (!g->sv[i])
      return 0;

    /* initialize the first state value. */
    g->sv[i][0] = 1;
  }

  /* initialize the first base. */
  g->bv[0] = 2;

  /* loop over the remaining bases. */
  for (i = 1; i < n; i++) {
    /* initialize the current base. */
    g->bv[i] = g->bv[i - 1] + 1;

    /* loop until a valid prime is identified. */
    for (j = 0; j < i; j++) {
      if (g->bv[i] % g->bv[j] == 0) {
        g->bv[i]++;
        j = 0;
      }
    }
  }

  /* store the generator size. */
  g->n = n;

  /* return success. */
  return 1;
}

/* qrngfree(): free allocated memory from a quasirandom number generator.
 *
 * arguments:
 *  @g: pointer to the generator to free.
 */
void qrngfree (qrng_t *g) {
  /* declare required variables:
   *  @i: general loop counter.
   */
  unsigned int i;

  /* ensure the pointer is valid. */
  if (!g || g->n == 0)
    return;

  /* free the iterate array. */
  if (g->x) {
    free(g->x);
    g->x = NULL;
  }

  /* free the base array. */
  if (g->bv) {
    free(g->bv);
    g->bv = NULL;
  }

  /* free the states. */
  for (i = 0; i < g->n; i++) {
    free(g->sv[i]);
    g->sv[i] = NULL;
  }

  /* free the state array. */
  free(g->sv);
  g->sv = NULL;

  /* initialize the size. */
  g->n = 0;
}

/* qrngeval(): evaluate the next term in a quasirandom sequence.
 *
 * arguments:
 *  @g: pointer to the generator structure to use.
 */
void qrngeval (qrng_t *g) {
  /* declare required variables:
   *  @i: general state counter.
   *  @k: "bit" loop counter.
   *  @kpow: bit multiplier.
   */
  unsigned int i, k;
  double kpow;

  /* initialize the outputs. */
  memset(g->x, 0, g->n * sizeof(double));

  /* loop over the states. */
  for (i = 0; i < g->n; i++) {
    /* initialize the multiplier. */
    kpow = 1.0 / ((double) g->bv[i]);

    /* loop over the bits in the state. */
    for (k = 0; k < QRNG_MAX; k++) {
      /* update the current state's output value. */
      g->x[i] += ((double) g->sv[i][k]) * kpow;

      /* update the state's bit multiplier. */
      kpow /= ((double) g->bv[i]);
    }
  }

  /* loop over each state. */
  for (i = 0; i < g->n; i++) {
    /* loop over the "bits" of the state. */
    for (k = 0; k < QRNG_MAX; k++) {
      /* increment the current bit. */
      g->sv[i][k]++;

      /* check the current bit. */
      if (g->sv[i][k] >= g->bv[i]) {
        /* reset the bit. */
        g->sv[i][k] = 0;
      }
      else {
        /* keep the bit and break the loop. */
        break;
      }
    }
  }
}

