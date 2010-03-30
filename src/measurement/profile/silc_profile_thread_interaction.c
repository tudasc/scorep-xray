/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_profile_thread_interaction.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include "silc_profile_thread_interaction.h"

#include <SILC_Memory.h>
#include <SILC_Utils.h>
#include "silc_thread.h"

extern silc_profile_node*
silc_profile_get_current_node( SILC_Thread_LocationData* thread );

/*----------------------------------------------------------------------------------------
   Constructors / destriuctors
   ----------------------------------------------------------------------------------------*/

/** Creates a new child node of given type and data without metrics*/
silc_profile_node*
silc_profile_create_node_without_metrics( silc_profile_node*     parent,
                                          silc_profile_node_type type,
                                          uint64_t               data )
{
    int i;

    /* Reserve space for the node record and dense metrics */
    silc_profile_node* node = ( silc_profile_node* )
                              SILC_Memory_AllocForProfile( sizeof( silc_profile_node ) );

    /* Initialize values */
    node->dense_metrics       = NULL;
    node->callpath_handle     = SILC_INVALID_CALLPATH;
    node->parent              = parent;
    node->first_child         = NULL;
    node->next_sibling        = NULL;
    node->first_double_sparse = NULL;
    node->first_int_sparse    = NULL;
    node->count               = 0; /* Is increased to one during SILC_Profile_Enter() */
    node->first_enter_time    = 0;
    node->last_exit_time      = 0;
    node->node_type           = type;
    node->type_specific_data  = data;

    return node;
}

/** Creates a new child node of type 'thread fork' */
inline silc_profile_node*
silc_profile_create_fork_node( silc_profile_node* parent )
{
    return silc_profile_create_node_without_metrics( parent,
                                                     silc_profile_node_thread_fork,
                                                     0 );
}

/*----------------------------------------------------------------------------------------
   Interface implementation
   ----------------------------------------------------------------------------------------*/

/**
 * Allocate and initialize a valid SILC_Profile_LocationData object.
 *
 */
SILC_Profile_LocationData*
SILC_Profile_CreateLocationData()
{
    /* Create location data structure */
    SILC_Profile_LocationData* data =
        SILC_Memory_AllocForProfile( sizeof( SILC_Profile_LocationData ) );
    data->current_node  = NULL;
    data->root_node     = NULL;
    data->fork_node     = NULL;
    data->creation_node = NULL;

    return data;
}


void
SILC_Profile_DeleteLocationData( SILC_Profile_LocationData* profileLocationData )
{
    if ( profileLocationData )
    {
        // clean up
    }
}


void
SILC_Profile_OnThreadCreation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData )
{
}

void
SILC_Profile_OnThreadActivation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData )
{
    SILC_Profile_LocationData* thread_data    = NULL;
    SILC_Profile_LocationData* parent_data    = NULL;
    silc_profile_node*         root           = NULL;
    silc_profile_node*         node           = NULL;
    silc_profile_node*         creation_point = NULL;

    SILC_PROFILE_ASSURE_INITIALIZED;
    SILC_ASSERT( locationData != NULL );

    /* Get root node of the thread */
    thread_data = SILC_Thread_GetProfileLocationData( locationData );
    if ( thread_data == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Thread activated which was not created." );
        SILC_PROFILE_STOP;
        return;
    }
    root = thread_data->root_node;
    SILC_ASSERT( root != NULL );

    /* Find creation point if available */
    if ( parentLocationData != NULL )
    {
        parent_data = SILC_Thread_GetProfileLocationData( parentLocationData );
        if ( parent_data != NULL )
        {
            creation_point = parent_data->fork_node;
        }
    }

    /* Check wether such a activation node already exists */
    node = root->first_child;
    while ( ( node != NULL ) ||
            ( ( node->node_type == silc_profile_node_thread_start ) &&
              ( creation_point == ( silc_profile_node* )( int )node->type_specific_data ) ) )
    {
        node = node->next_sibling;
    }

    /* Create new node if no exists */
    if ( node == NULL )
    {
        node = silc_profile_create_node_without_metrics( root,
                                                         silc_profile_node_thread_start,
                                                         ( int )creation_point );
        node->next_sibling = root->first_child;
        root->first_child  = node;
    }

    /* Now node points to the starting point of the thread.
       Make it the current node of the thread. */
    thread_data->current_node = node;
}


void
SILC_Profile_OnThreadDectivation( SILC_Thread_LocationData* locationData,
                                  SILC_Thread_LocationData* parentLocationData )
{
    /* Remove the current node. */
    SILC_Thread_GetProfileLocationData( locationData )->current_node = NULL;
}


void
SILC_Profile_OnLocationCreation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData )
{
    SILC_Profile_LocationData* parent_data = NULL;
    SILC_Profile_LocationData* thread_data = NULL;
    silc_profile_node*         node        = NULL;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Create thread root node
       Reserve space for a node without dense metrics */
    node = silc_profile_create_node_without_metrics( NULL, silc_profile_node_thread_root,
                                                     SILC_Thread_GetLocationId( locationData ) );

    /* Update thread location data */
    thread_data = SILC_Thread_GetProfileLocationData( locationData );
    SILC_ASSERT( thread_data != NULL );
    thread_data->root_node = node;

    if ( parentLocationData != NULL )
    {
        parent_data                = SILC_Thread_GetProfileLocationData( parentLocationData );
        thread_data->creation_node = parent_data->fork_node;
    }

    /* Add it to the profile node list */
    if ( parent_data == NULL )
    {
        /* It is the initial thread. Insert as first new root node */
        node->next_sibling           = silc_profile.first_root_node;
        silc_profile.first_root_node = node;
    }
    else
    {
        /* Append after parent root node */
        node->next_sibling                   = parent_data->root_node->next_sibling;
        parent_data->root_node->next_sibling = node;
    }
}

void
SILC_Profile_OnFork( SILC_Thread_LocationData* threadData,
                     size_t                    maxChildThreads )
{
    silc_profile_node* node  = NULL;
    silc_profile_node* child = NULL;
    int                i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Get current node */
    node = silc_profile_get_current_node( threadData );
    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Current region unknown." );
        SILC_PROFILE_STOP;
        return;
    }

    /* Find or create the child node */

    /* Check wether the node has children */
    child = node->first_child;
    if ( child == NULL )
    {
        node->first_child = silc_profile_create_fork_node( node );
        node              = node->first_child;
    }
    /* At least one child exists: Search if the region was entered before */
    else
    {
        while ( ( child->next_sibling != NULL ) ||
                ( ( child->node_type != silc_profile_node_thread_fork ) ) )
        {
            child = child->next_sibling;
        }
        /* No matching region found: Create a new sibling */
        if ( child->node_type != silc_profile_node_thread_fork )
        {
            child->next_sibling = silc_profile_create_fork_node( node );
            node                = child->next_sibling;
        }
        /* Call path was already visited */
        else
        {
            node = child;
        }
    }
    /* Now node points to the newly entered region */

    /* Store current fork node */
    SILC_Thread_GetProfileLocationData( threadData )->fork_node = node;
}
