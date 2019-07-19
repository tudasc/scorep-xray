/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2019,
 * RWTH Aachen University, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup MPI_Wrapper
 */

#include <config.h>

#include "scorep_mpi_rma_request.h"

#include <inttypes.h>

#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>

#include <UTILS_Error.h>

#define SCOREP_SKIPLIST_TYPE scorep_mpi_rma_request
#include <scorep_skiplist.h>

/**
 * @internal
 * @brief Head node of the RMA request list
 */
static scorep_mpi_rma_request_skiplist_t* rma_requests;

/**
 * @internal
 * @brief Mutex for rma_requests
 */
static SCOREP_Mutex rma_request_mutex;

/**
 * @internal
 * @brief Reset values to a defined state
 *
 * @param payload Pointer to payload structure to reset
 *
 * @note Dispite of being a static function, this function is part of the
 * SKIPLIST functionality for the type #scorep_mpi_rma_request and therefore
 * retains the @c scorep_ prefix.
 */
static void
scorep_mpi_rma_request_reset_payload( scorep_mpi_rma_request* payload )
{
    payload->window            = SCOREP_INVALID_RMA_WINDOW;
    payload->target            = -1;
    payload->matching_id       = 0;
    payload->mpi_handle        = MPI_REQUEST_NULL;
    payload->completion_type   = SCOREP_MPI_RMA_REQUEST_SEPARATE_COMPLETION;
    payload->completed_locally = false;
    payload->schedule_removal  = false;
}

/**
 * @internal
 * @brief Print payload information
 *
 * @param prefix String prefix to the output
 * @param request Pointer to request data to print
 *
 * @note Dispite of being a static function, this function is part of the
 * SKIPLIST functionality for the type #scorep_mpi_rma_request and therefore
 * retains the @c scorep_ prefix.
 */
static void
scorep_mpi_rma_request_print_payload( const char* prefix, const scorep_mpi_rma_request* request )
{
    UTILS_DEBUG( "%s: win=%" PRIu32 " target=%i match=%" PRIu64 " req=%lu compl=%i", prefix,
                 request->window, request->target, request->matching_id,
                 request->mpi_handle, request->completion_type );
}

void
scorep_mpi_rma_request_init( void )
{
    SCOREP_MutexCreate( &rma_request_mutex );
    unsigned int height = 6;

    // Create a skiplist for 2^6 elements
    rma_requests = scorep_mpi_rma_request_create_list( height,
                                                       ( SCOREP_Skiplist_Guard )SCOREP_MutexLock,
                                                       ( SCOREP_Skiplist_Guard )SCOREP_MutexUnlock,
                                                       ( SCOREP_Skiplist_GuardObject )rma_request_mutex );
}

void
scorep_mpi_rma_request_finalize( void )
{
    if ( !scorep_mpi_rma_request_is_empty( rma_requests ) )
    {
        UTILS_WARNING( "Request tracking not completed successfully for all RMA operations." );
    }
    scorep_mpi_rma_request_destroy_list( rma_requests );

    SCOREP_MutexDestroy( &rma_request_mutex );
}

static int
scorep_mpi_rma_request_cmp( const scorep_mpi_rma_request* listElement,
                            const scorep_mpi_rma_request* searchElement )
{
    scorep_mpi_rma_request_print_payload( "List element", listElement );
    scorep_mpi_rma_request_print_payload( "Search element", searchElement );

    // Test corner cases
    if ( listElement->window == SCOREP_INVALID_RMA_WINDOW && searchElement->window != SCOREP_INVALID_RMA_WINDOW )
    {
        return -1;
    }
    if ( listElement->window != SCOREP_INVALID_RMA_WINDOW && searchElement->window == SCOREP_INVALID_RMA_WINDOW )
    {
        return 1;
    }

    // Test window handle value
    if ( listElement->window < searchElement->window )
    {
        return -1;
    }
    if ( listElement->window > searchElement->window )
    {
        return 1;
    }

    // Test target value
    if ( listElement->target < searchElement->target )
    {
        return -1;
    }
    if ( listElement->target > searchElement->target )
    {
        return 1;
    }

    if ( listElement->mpi_handle == MPI_REQUEST_NULL && searchElement->mpi_handle != MPI_REQUEST_NULL )
    {
        return -1;
    }
    if ( listElement->mpi_handle != MPI_REQUEST_NULL && searchElement->mpi_handle == MPI_REQUEST_NULL )
    {
        return 1;
    }

    // Test MPI request handle
    if ( listElement->mpi_handle < searchElement->mpi_handle )
    {
        return -1;
    }
    if ( listElement->mpi_handle > searchElement->mpi_handle )
    {
        return 1;
    }

    // Test completion flag
    if ( listElement->completion_type != searchElement->completion_type )
    {
        if ( listElement->completion_type == SCOREP_MPI_RMA_REQUEST_SEPARATE_COMPLETION )
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    return 0;
}


scorep_mpi_rma_request*
scorep_mpi_rma_request_create( SCOREP_RmaWindowHandle       window,
                               int                          target,
                               MPI_Request                  mpi_handle,
                               SCOREP_Mpi_RmaCompletionType completion_type,
                               SCOREP_MpiRequestId          matching_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_mpi_rma_request_node* new_node =
        scorep_mpi_rma_request_create_node( rma_requests );

    // Set values for element
    new_node->payload.window          = window;
    new_node->payload.target          = target;
    new_node->payload.mpi_handle      = mpi_handle,
    new_node->payload.completion_type = completion_type;
    new_node->payload.matching_id     = matching_id;

    scorep_mpi_rma_request_print_list_node( "New", new_node );

    scorep_mpi_rma_request_insert_node( rma_requests, new_node );

    UTILS_DEBUG_EXIT();

    return &( new_node->payload );
}

scorep_mpi_rma_request*
scorep_mpi_rma_request_find( SCOREP_RmaWindowHandle       window,
                             int                          target,
                             MPI_Request                  mpiHandle,
                             SCOREP_Mpi_RmaCompletionType completionType )
{
    UTILS_DEBUG_ENTRY();

    scorep_mpi_rma_request search_info;
    scorep_mpi_rma_request_reset_payload( &search_info );

    search_info.window          = window;
    search_info.target          = target;
    search_info.mpi_handle      = mpiHandle;
    search_info.completion_type = completionType;

    scorep_mpi_rma_request_print_payload( "Search", &search_info );

    scorep_mpi_rma_request_node* list_element =
        scorep_mpi_rma_request_lower_bound( rma_requests, &search_info );

    scorep_mpi_rma_request_print_list_node( "Found", list_element );

    if ( scorep_mpi_rma_request_cmp( &( list_element->payload ), &search_info ) == 0 )
    {
        UTILS_DEBUG_EXIT();
        return &( list_element->payload );
    }
    else
    {
        UTILS_DEBUG_EXIT();
        return NULL;
    }
}


void
scorep_mpi_rma_request_remove( SCOREP_RmaWindowHandle       window,
                               int                          target,
                               MPI_Request                  mpiHandle,
                               SCOREP_Mpi_RmaCompletionType completionType )
{
    UTILS_DEBUG_ENTRY();

    scorep_mpi_rma_request search_info;
    scorep_mpi_rma_request_reset_payload( &search_info );

    search_info.window          = window;
    search_info.target          = target;
    search_info.mpi_handle      = mpiHandle;
    search_info.completion_type = completionType;

    scorep_mpi_rma_request_print_payload( "Search", &search_info );

    scorep_mpi_rma_request_node* element =
        scorep_mpi_rma_request_lower_bound( rma_requests, &search_info );

    scorep_mpi_rma_request_print_list_node( "Lower bound", element );

    if ( scorep_mpi_rma_request_cmp( &( element->payload ), &search_info ) != 0 )
    {
        UTILS_WARNING( "Unable to find RMA request to remove." );
        return;
    }

    scorep_mpi_rma_request_remove_node( rma_requests, element );

    UTILS_DEBUG_EXIT();
}

void
scorep_mpi_rma_request_remove_by_ptr( scorep_mpi_rma_request* request )
{
    // The starting address of the request payload is the same as for the node.
    // Thus we can cast the pointer and call scorep_mpi_rma_request_remove_node().

    scorep_mpi_rma_request_remove_node( rma_requests, ( scorep_mpi_rma_request_node* )request );
}

void
scorep_mpi_rma_request_foreach_on_window( SCOREP_RmaWindowHandle     window,
                                          scorep_mpi_rma_request_cb* callback )
{
    UTILS_DEBUG_ENTRY();

    scorep_mpi_rma_request search_info;
    scorep_mpi_rma_request_reset_payload( &search_info );

    search_info.window = window;

    scorep_mpi_rma_request_print_payload( "Search", &search_info );

    scorep_mpi_rma_request_node* list_element =
        scorep_mpi_rma_request_lower_bound( rma_requests, &search_info );

    scorep_mpi_rma_request_print_list_node( "Lower bound", list_element );

    if ( list_element != NULL
         && list_element->payload.window != search_info.window
         && list_element->next[ 0 ]
         && list_element->next[ 0 ]->payload.window == search_info.window )
    {
        list_element = list_element->next[ 0 ];
    }

    while ( list_element != NULL
            && list_element->payload.window == window )
    {
        scorep_mpi_rma_request_print_list_node( "Processing", list_element );

        ( *callback )( &( list_element->payload ) );
        if ( list_element->payload.schedule_removal )
        {
            scorep_mpi_rma_request_print_list_node( "Removing", list_element );
            scorep_mpi_rma_request_node* removal_candidate = list_element;
            list_element = list_element->next[ 0 ];
            scorep_mpi_rma_request_remove_node( rma_requests, removal_candidate );
        }
        else
        {
            list_element = list_element->next[ 0 ];
        }
    }

    UTILS_DEBUG_EXIT();
}

void
scorep_mpi_rma_request_foreach_to_target( SCOREP_RmaWindowHandle     window,
                                          int                        target,
                                          scorep_mpi_rma_request_cb* callback )
{
    UTILS_DEBUG_ENTRY();

    scorep_mpi_rma_request search_info;
    scorep_mpi_rma_request_reset_payload( &search_info );

    search_info.window = window;
    search_info.target = target;

    scorep_mpi_rma_request_print_payload( "Search", &search_info );

    scorep_mpi_rma_request_node* list_element =
        scorep_mpi_rma_request_lower_bound( rma_requests, &search_info );

    scorep_mpi_rma_request_print_list_node( "Lower bound", list_element );

    if ( list_element != NULL
         && list_element->payload.window != search_info.window
         && list_element->next[ 0 ]
         && list_element->next[ 0 ]->payload.window == search_info.window )
    {
        list_element = list_element->next[ 0 ];
    }

    while ( list_element != NULL
            && list_element->payload.window == window
            && list_element->payload.target == target )
    {
        scorep_mpi_rma_request_print_list_node( "Processing", list_element );

        ( *callback )( &( list_element->payload ) );
        if ( list_element->payload.schedule_removal )
        {
            scorep_mpi_rma_request_print_list_node( "Removing", list_element );
            scorep_mpi_rma_request_node* removal_candidate = list_element;
            list_element = list_element->next[ 0 ];
            scorep_mpi_rma_request_remove_node( rma_requests, removal_candidate );
        }
        else
        {
            list_element = list_element->next[ 0 ];
        }
    }

    UTILS_DEBUG_EXIT();
}

/**
 * OTF2 allows the completion of multiple RMA operation events using a single
 * completion record. For active-target synchronization, this can be
 * accomplished by assigning all RMA operations that implicitly complete
 * remotely upon their local completion (e.g., MPI_Get) one maching id and all
 * RMA operations that do not (e.g., MPI_Put) another. This way, completion of
 * all pending operations in active-target synchronization can be modeled using
 * two records only.
 *
 * For passive-target synchronization, completion can be enforced by the user
 * on a more fine-grained level. We therefore need to track independent
 * matching ids for RMA operations to different targets in addition to the
 * distinction above.
 *
 * To enable a seemless integration of request-based and implicit operations,
 * all matching ids are obtained from @a scorep_mpi_request_id().
 */

void
scorep_mpi_rma_request_write_standard_completion_and_remove( scorep_mpi_rma_request* request )
{
    if ( !request->completed_locally )
    {
        SCOREP_RmaOpCompleteNonBlocking( request->window, request->matching_id );
        request->completed_locally = true;

        if ( request->completion_type == SCOREP_MPI_RMA_REQUEST_COMBINED_COMPLETION )
        {
            SCOREP_RmaOpCompleteRemote( request->window, request->matching_id );
        }
    }

    request->schedule_removal = true;
}

void
scorep_mpi_rma_request_write_standard_completion( scorep_mpi_rma_request* request )
{
    if ( !request->completed_locally )
    {
        SCOREP_RmaOpCompleteNonBlocking( request->window, request->matching_id );
        request->completed_locally = true;

        if ( request->completion_type == SCOREP_MPI_RMA_REQUEST_COMBINED_COMPLETION )
        {
            SCOREP_RmaOpCompleteRemote( request->window, request->matching_id );
            request->schedule_removal = true;
        }
    }
}

void
scorep_mpi_rma_request_write_full_completion( scorep_mpi_rma_request* request )
{
    if ( !request->completed_locally )
    {
        SCOREP_RmaOpCompleteNonBlocking( request->window, request->matching_id );
        request->completed_locally = true;
    }

    SCOREP_RmaOpCompleteRemote( request->window, request->matching_id );
    request->schedule_removal = true;
}
