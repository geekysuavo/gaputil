
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
#ifndef __NUSUTILS_BST_H__
#define __NUSUTILS_BST_H__

/* include the tuple header. */
#include "tup.h"

/* define constants used to define tree node coloring.
 */
#define BST_BLACK  0
#define BST_RED    1

/* bst_node_t: type definition of a pointer to a search tree node.
 */
typedef struct bst bst_t;

/* bst_t: type definition of a binary search tree data structure.
 */
struct bst {
  /* @up: pointer to the parent node.
   * @left: pointer to the left child node.
   * @right: pointer to the right child node.
   */
  bst_t *up, *left, *right;

  /* @value: data contained by the current tree node.
   * @color: coloring (red/black) of the current tree node.
   * @n: total number of unique elements (root only).
   */
  unsigned int value, color, n;
};

/* function declarations: */

bst_t *bstalloc (void);

void bstinit (bst_t *t);

void bstfree (bst_t *t);

bst_t *bstinsert (bst_t *t, unsigned int val);

void bstsort (bst_t *t, tuple_t *tout);

#endif /* !__NUSUTILS_BST_H__ */

