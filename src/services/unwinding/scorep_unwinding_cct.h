/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_UNWINDING_CCT_H
#define SCOREP_UNWINDING_CCT_H

#include "scorep_unwinding_mgmt.h"

/* Splay tree based on: */
/*
                An implementation of top-down splaying
                    D. Sleator <sleator@cs.cmu.edu>
                            March 1992

   "Splay trees", or "self-adjusting search trees" are a simple and
   efficient data structure for storing an ordered set.  The data
   structure consists of a binary tree, without parent pointers, and no
   additional fields.  It allows searching, insertion, deletion,
   deletemin, deletemax, splitting, joining, and many other operations,
   all with amortized logarithmic performance.  Since the trees adapt to
   the sequence of requests, their performance on real access patterns is
   typically even better.  Splay trees are described in a number of texts
   and papers [1,2,3,4,5].

   The code here is adapted from simple top-down splay, at the bottom of
   page 669 of [3].  It can be obtained via anonymous ftp from
   spade.pc.cs.cmu.edu in directory /usr/sleator/public.

   The chief modification here is that the splay operation works even if the
   item being splayed is not in the tree, and even if the tree root of the
   tree is NULL.  So the line:

                              t = splay(i, t);

   causes it to search for item with key i in the tree rooted at t.  If it's
   there, it is splayed to the root.  If it isn't there, then the node put
   at the root is the last one before NULL that would have been reached in a
   normal binary search for i.  (It's a neighbor of i in the tree.)  This
   allows many other operations to be easily implemented, as shown below.

   [1] "Fundamentals of data structures in C", Horowitz, Sahni,
       and Anderson-Freed, Computer Science Press, pp 542-547.
   [2] "Data Structures and Their Algorithms", Lewis and Denenberg,
       Harper Collins, 1991, pp 243-251.
   [3] "Self-adjusting Binary Search Trees" Sleator and Tarjan,
       JACM Volume 32, No 3, July 1985, pp 652-686.
   [4] "Data Structure and Algorithm Analysis", Mark Weiss,
       Benjamin Cummins, 1992, pp 119-130.
   [5] "Data Structures, Algorithms, and Performance", Derick Wood,
       Addison-Wesley, 1993, pp 367-375.
 */

static inline int
calling_context_compare_node( uint64_t                                    ip,
                              SCOREP_RegionHandle                         region,
                              scorep_unwinding_calling_context_tree_node* node )
{
    int ret = ( region > node->region ) - ( region < node->region );
    if ( ret )
    {
        return ret;
    }
    return ( ip > node->ip ) - ( ip < node->ip );
}

static scorep_unwinding_calling_context_tree_node*
calling_context_splay( scorep_unwinding_calling_context_tree_node* root,
                       uint64_t                                    ip,
                       SCOREP_RegionHandle                         region )
{
    scorep_unwinding_calling_context_tree_node  sentinel;
    scorep_unwinding_calling_context_tree_node* left;
    scorep_unwinding_calling_context_tree_node* right;
    scorep_unwinding_calling_context_tree_node* node;

    if ( root == NULL )
    {
        return root;
    }

    sentinel.left = sentinel.right = NULL;
    left          = right = &sentinel;

    for (;; )
    {
        if ( calling_context_compare_node( ip, region, root ) < 0 )
        {
            if ( root->left == NULL )
            {
                break;
            }
            if ( calling_context_compare_node( ip, region, root->left ) < 0 )
            {
                node        = root->left;                    /* rotate right */
                root->left  = node->right;
                node->right = root;
                root        = node;
                if ( root->left == NULL )
                {
                    break;
                }
            }
            right->left = root;                               /* link right */
            right       = root;
            root        = root->left;
        }
        else if ( calling_context_compare_node( ip, region, root ) > 0 )
        {
            if ( root->right == NULL )
            {
                break;
            }
            if ( calling_context_compare_node( ip, region, root->right ) > 0 )
            {
                node        = root->right;                   /* rotate left */
                root->right = node->left;
                node->left  = root;
                root        = node;
                if ( root->right == NULL )
                {
                    break;
                }
            }
            left->right = root;                              /* link left */
            left        = root;
            root        = root->right;
        }
        else
        {
            break;
        }
    }

    left->right = root->left;                                /* assemble */
    right->left = root->right;
    root->left  = sentinel.right;
    root->right = sentinel.left;

    return root;
}

/**
 * Translate current stack to a calling context tree representation.
 *
 * @param[inout] unwindContext  The unwind context where to start
 * @param instructionStack      Current stack of this location
 * @param instructionStackDepth Depth of current stack
 *
 * @return calling context tree representation of current stack
 */
static void
calling_context_descent( SCOREP_Location*                             location,
                         scorep_unwinding_calling_context_tree_node** unwindContext,
                         uint32_t*                                    unwindDistance,
                         uint64_t                                     ip,
                         SCOREP_RegionHandle                          region )
{
    ( *unwindDistance )++;

    scorep_unwinding_calling_context_tree_node* child = ( *unwindContext )->children;
    while ( child )
    {
        int compare_result = calling_context_compare_node( ip, region, child );
        if ( compare_result < 0 )
        {
            child = child->left;
        }
        else if ( compare_result > 0 )
        {
            child = child->right;
        }
        else
        {
            *unwindContext = child;
            return;
        }
    }

    /* Slow path */

    /* Allocate memory for a new child */
    child         = SCOREP_Location_AllocForMisc( location, sizeof( *child ) );
    child->handle =
        SCOREP_Definitions_NewCallingContext( ip,
                                              region,
                                              SCOREP_INVALID_SOURCE_CODE_LOCATION,
                                              ( *unwindContext )->handle );
    child->ip       = ip;
    child->region   = region;
    child->children = NULL;
    child->left     = NULL;
    child->right    = NULL;

    /* Insert new child to parent's splay-tree of children */
    ( *unwindContext )->children = calling_context_splay( ( *unwindContext )->children, ip, region );
    if ( ( *unwindContext )->children )
    {
        int compare_result = calling_context_compare_node( ip, region, ( *unwindContext )->children );
        /* compare_result is valid, as this is not the first child */
        if ( compare_result < 0 )
        {
            child->right       = ( *unwindContext )->children;
            child->left        = child->right->left;
            child->right->left = NULL;
        }
        else if ( compare_result > 0 )
        {
            child->left        = ( *unwindContext )->children;
            child->right       = child->left->right;
            child->left->right = NULL;
        }
    }
    ( *unwindContext )->children = child;

    /* The new allocated child is our new node now */
    *unwindContext = ( *unwindContext )->children;
}

#endif /* SCOREP_UNWINDING_CCT_H */
