
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

/* include the binary search tree header. */
#include "bst.h"

/* bstalloc(): allocate a binary search tree node pointer.
 *
 * returns:
 *  pointer to a newly allocated, initialized tree node.
 */ 
bst_t *bstalloc (void) {
  /* declare required variables:
   *  @t: pointer to the allocated, initialized tree.
   */
  bst_t *t;

  /* allocate a pointer to a tree node. */
  t = (bst_t*) malloc(sizeof(bst_t));
  if (!t)
    return NULL;

  /* initialize the allocated tree node. */
  bstinit(t);

  /* return the initialized tree node pointer. */
  return t;
}

/* bstinit(): initialize a binary search tree node to default values.
 *
 * arguments:
 *  @t: pointer to the tree node to initialize.
 */
void bstinit (bst_t *t) {
  /* return if the pointer is null. */
  if (!t)
    return;

  /* initialize the parent and child node pointers. */
  t->up = NULL;
  t->left = NULL;
  t->right = NULL;

  /* initialize the color and value. */
  t->color = BST_BLACK;
  t->value = 0;
  t->n = 0;
}

/* bstfree(): free the allocated pointers belonging to a binary search tree.
 *
 * arguments:
 *  @t: pointer to the tree to free.
 */
void bstfree (bst_t *t) {
  /* return if the pointer is null. */
  if (!t)
    return;

  /* free the child nodes. */
  bstfree(t->left);
  bstfree(t->right);

  /* free the node pointer. */
  free(t);
  t = NULL;
}

/* bst_node_upup(): return a pointer to the tree node that is two tree
 * levels above the current node.
 *
 * arguments:
 *  @t: pointer to the current tree node.
 *
 * returns:
 *  pointer to the twice-parent node of @t, or null if none exists.
 */
bst_t *bst_node_upup (bst_t *t) {
  /* return the pointer if it's reachable (defined). */
  if (t && t->up)
    return t->up->up;

  /* return a null pointer. */
  return NULL;
}

/* bst_node_upover(): return a pointer to the tree node that is at the
 * same tree level as the current node's parent.
 *
 * arguments:
 *  @t: pointer to the current tree node.
 *
 * returns:
 *  pointer to the requested node, or null if none exists.
 */
bst_t *bst_node_upover (bst_t *t) {
  /* declare required variables:
   *  @tuu: tree node pointer two levels up.
   */
  bst_t *tuu;

  /* get the node two levels up. */
  tuu = bst_node_upup(t);

  /* return if the node was unreachable. */
  if (!tuu)
    return NULL;

  /* return the node opposite the current parent node. */
  if (t->up == tuu->left)
    return tuu->right;
  else
    return tuu->left;
}

bst_t *bst_node_insert (bst_t *t, unsigned int val) {
  /* determine which action to take. */
  if (val < t->value) {
    /* check if a left child exists. */
    if (t->left) {
      /* yes. traverse the left child. */
      return bst_node_insert(t->left, val);
    }
    else {
      /* no. allocate a new left child. */
      t->left = bstalloc();
      t->left->value = val;
      t->left->up = t;

      /* return the new node. */
      return t->left;
    }
  }
  else if (val > t->value) {
    /* check if a right child exists. */
    if (t->right) {
      /* yes. traverse the right child. */
      return bst_node_insert(t->right, val);
    }
    else {
      /* no. allocate a new right child. */
      t->right = bstalloc();
      t->right->value = val;
      t->right->up = t;

      /* return the new node. */
      return t->right;
    }
  }

  /* the value was not unique. return null. */
  return NULL;
}

/* bstinsert(): insert a unique value into a binary search tree. if the value
 * already exists in the tree, the tree remains unaltered.
 *
 * arguments:
 *  @t: pointer to the tree to modify.
 *  @val: value to insert into the tree.
 *
 * returns:
 *  pointer to the root node of the modified tree.
 */
bst_t *bstinsert (bst_t *t, unsigned int val) {
  /* declare required variables:
   *  @tnew: new node pointer.
   */
  bst_t *tnew;

  /* check if the tree exists. */
  if (!t) {
    /* allocate and return a new tree. */
    tnew = bstalloc();
    tnew->value = val;
    return tnew;
  }

  /* insert the value into the existing tree. */
  tnew = bst_node_insert(t, val);

  /* increment the tree size, if necessary. */
  if (tnew)
    t->n++;

  /* return the modified tree. */
  return t;
}

/* bstsort(): traverse a binary search tree in order to write its values
 * out into a linear tuple pointer, whose values will be sorted.
 *
 * arguments:
 *  @t: pointer to the tree to traverse.
 *  @tout: pointer to the tuple to fill, should be initialized as empty.
 */
void bstsort (bst_t *t, tuple_t *tout) {
  /* return if the pointer is null. */
  if (!t)
    return;

  /* traverse the left sub-tree. */
  bstsort(t->left, tout);

  /* append the current value. */
  tupappend(tout, t->value);

  /* traverse the right sub-tree. */
  bstsort(t->right, tout);
}

