/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_unwinding_region.h"

#include <SCOREP_Unwinding.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Config.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME UNWINDING
#include <UTILS_Debug.h>

#include <unistd.h>
#include <string.h>

typedef int ( * region_compare )( uint64_t                 key,
                                  scorep_unwinding_region* node );

/** Compare functions used when inserting a new region */
static int
compare_start( uint64_t                 key,
               scorep_unwinding_region* node )
{
    return ( key > node->start ) - ( key < node->start );
}

/** Compare functions used when searching a region */
static int
compare_incl( uint64_t                 key,
              scorep_unwinding_region* node )
{
    if ( key < node->start )
    {
        return -1;
    }
    if ( key >= node->end )
    {
        return 1;
    }
    return 0;
}

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

static scorep_unwinding_region*
splay( scorep_unwinding_region* root,
       uint64_t                 key,
       region_compare           compare )
{
    scorep_unwinding_region  sentinel;
    scorep_unwinding_region* left;
    scorep_unwinding_region* right;
    scorep_unwinding_region* node;

    if ( root == NULL )
    {
        return root;
    }

    sentinel.left = sentinel.right = NULL;
    left          = right = &sentinel;

    for (;; )
    {
        if ( compare( key, root ) < 0 )
        {
            if ( root->left == NULL )
            {
                break;
            }
            if ( compare( key, root->left ) < 0 )
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
        else if ( compare( key, root ) > 0 )
        {
            if ( root->right == NULL )
            {
                break;
            }
            if ( compare( key, root->right ) > 0 )
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

scorep_unwinding_region*
scorep_unwinding_region_insert( SCOREP_Unwinding_CpuLocationData* unwindData,
                                uint64_t                          start,
                                uint64_t                          end,
                                const char*                       name )
{
    if ( unwindData == NULL )
    {
        return NULL;
    }

    size_t                   len = strlen( name );
    scorep_unwinding_region* new = SCOREP_Memory_AllocForMisc( sizeof( *new ) + len );
    memset( new, 0, sizeof( *new ) );
    new->start = start;
    new->end   = end;
    memcpy( new->name, name, len + 1 );
    new->left = new->right = NULL;

    if ( unwindData->known_regions == NULL )
    {
        unwindData->known_regions = new;
        return new;
    }

    unwindData->known_regions = splay( unwindData->known_regions, start, compare_start );
    if ( start < unwindData->known_regions->start )
    {
        new->left                       = unwindData->known_regions->left;
        new->right                      = unwindData->known_regions;
        unwindData->known_regions->left = NULL;
        unwindData->known_regions       = new;
    }
    else if ( start > unwindData->known_regions->start )
    {
        new->right                       = unwindData->known_regions->right;
        new->left                        = unwindData->known_regions;
        unwindData->known_regions->right = NULL;
        unwindData->known_regions        = new;
    }
    else
    {
        UTILS_BUG( "Region already known: %s@[%" PRIx64 ", %" PRIx64 ") "
                   "existing: %s@[%" PRIx64 ", %" PRIx64 ")",
                   name, start, end,
                   unwindData->known_regions->name,
                   unwindData->known_regions->start,
                   unwindData->known_regions->end );
    }

    return new;
}

scorep_unwinding_region*
scorep_unwinding_region_find( SCOREP_Unwinding_CpuLocationData* unwindData,
                              uint64_t                          addr )
{
    if ( unwindData == NULL || unwindData->known_regions == NULL )
    {
        return NULL;
    }

    unwindData->known_regions = splay( unwindData->known_regions, addr, compare_incl );
    if ( compare_incl( addr, unwindData->known_regions ) == 0 )
    {
        return unwindData->known_regions;
    }

    return NULL;
}

void
scorep_unwinding_region_clear( SCOREP_Unwinding_CpuLocationData* unwindData,
                               scorep_unwinding_region_cleanup   cleanup,
                               void*                             arg )
{
    while ( unwindData && unwindData->known_regions )
    {
        scorep_unwinding_region* node = NULL;
        if ( unwindData->known_regions->left == NULL )
        {
            node = unwindData->known_regions->right;
        }
        else
        {
            node = splay( unwindData->known_regions->left,
                          unwindData->known_regions->start,
                          compare_start );
            node->right = unwindData->known_regions->right;
        }
        if ( cleanup )
        {
            cleanup( unwindData->known_regions, arg );
        }
        unwindData->known_regions = node;
    }
}
