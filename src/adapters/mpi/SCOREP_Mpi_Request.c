/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for request handling
 */

#include <config.h>
/* We do wrap deprecated functions here, but we don't want warnings */
#include "SCOREP_Mpi.h"
#include "scorep_mpi_communicator.h"
#include "scorep_mpi_request_mgmt.h"
#include <UTILS_Error.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>

/**
 * @name Helper functions
 * @{
 */

/**
 * internal array of statuses
 */
static MPI_Status* scorep_mpi_status_array = NULL;

/**
 * size of internal status array
 */
static int scorep_mpi_status_array_size = 0;

/**
 * Get a pointer to a status array of at least 'size' statuses
 * @param  size minimal requested size
 * @return pointer to status array
 */
static MPI_Status*
scorep_mpi_get_status_array( int size )
{
    if ( ( scorep_mpi_status_array_size == 0 )
         && ( size > 0 ) )
    {
        /* -- never used: initialize -- */
        scorep_mpi_status_array = malloc( size * sizeof( MPI_Status ) );
        if ( scorep_mpi_status_array == NULL )
        {
            UTILS_FATAL( "Allocation of %zu bytes for internal MPI status array failed!",
                         size * sizeof( MPI_Status ) );
        }
        scorep_mpi_status_array_size = size;
    }
    else
    if ( size > scorep_mpi_status_array_size )
    {
        /* -- not enough room: expand -- */
        scorep_mpi_status_array = realloc( scorep_mpi_status_array, size * sizeof( MPI_Status ) );
        if ( scorep_mpi_status_array == NULL )
        {
            UTILS_FATAL( "Re-allocation of %zu bytes for internal MPI status array failed!",
                         size * sizeof( MPI_Status ) );
        }
        scorep_mpi_status_array_size = size;
    }
    return scorep_mpi_status_array;
}

/**
 * @}
 * @name Waiting functions
 * @{
 */

#if HAVE( DECL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Wait
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Wait( MPI_Request* request,
          MPI_Status*  status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    int                 return_val;
    MPI_Status          mystatus;
    scorep_mpi_request* orig_req;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAIT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAIT ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

    if ( status == MPI_STATUS_IGNORE )
    {
        status = &mystatus;
    }

    orig_req = scorep_mpi_request_get( *request );
    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Wait( request, status );
    SCOREP_EXIT_WRAPPED_REGION();

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( orig_req, status, start_time_stamp );
    }
  #endif

    scorep_mpi_check_request( orig_req, status );

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAIT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAIT ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Waitall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Waitall( int          count,
             MPI_Request* requests,
             MPI_Status*  array_of_statuses )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    scorep_mpi_request* orig_req;
    int                 i;
    int                 return_val;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITALL ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

  #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( count );
    }
  #endif

    scorep_mpi_save_request_array( requests, count );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Waitall( count, requests, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();

    for ( i = 0; i < count; i++ )
    {
        orig_req = scorep_mpi_saved_request_get( i );

    #if !defined( SCOREP_MPI_NO_HOOKS )
        if ( SCOREP_IS_MPI_HOOKS_ON )
        {
            SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( orig_req, &( array_of_statuses[ i ] ), start_time_stamp );
        }
    #endif

        scorep_mpi_check_request( orig_req, &( array_of_statuses[ i ] ) );
    }
    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITALL ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Waitany
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Waitany( int          count,
             MPI_Request* requests,
             int*         index,
             MPI_Status*  status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xnb_active                 = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XNONBLOCK );
    int                 return_val;
    scorep_mpi_request* orig_req;
    MPI_Status          mystatus;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITANY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITANY ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

    if ( status == MPI_STATUS_IGNORE )
    {
        status = &mystatus;
    }

    scorep_mpi_save_request_array( requests, count );
    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Waitany( count, requests, index, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active_for_group && xnb_active )
    {
        int i;

        for ( i = 0; i < count; ++i )
        {
            orig_req = scorep_mpi_saved_request_get( i );

            if ( i == *index )
            {
                  #if !defined( SCOREP_MPI_NO_HOOKS )
                if ( SCOREP_IS_MPI_HOOKS_ON )
                {
                    SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( orig_req, status, start_time_stamp );
                }
          #endif
                scorep_mpi_check_request( orig_req, status );
            }
            else if ( orig_req && ( orig_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE ) )
            {
                SCOREP_MpiRequestTested( orig_req->id );
            }
        }
    }
    else
    {
        orig_req = scorep_mpi_saved_request_get( *index );
      #if !defined( SCOREP_MPI_NO_HOOKS )
        if ( SCOREP_IS_MPI_HOOKS_ON )
        {
            SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( orig_req, status, start_time_stamp );
        }
      #endif
        scorep_mpi_check_request( orig_req, status );
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITANY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITANY ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Waitsome
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Waitsome( int          incount,
              MPI_Request* array_of_requests,
              int*         outcount,
              int*         array_of_indices,
              MPI_Status*  array_of_statuses )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xnb_active                 = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XNONBLOCK );
    int                 return_val;
    int                 i;
    scorep_mpi_request* orig_req;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITSOME ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITSOME ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

  #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( incount );
    }
  #endif

    scorep_mpi_save_request_array( array_of_requests, incount );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Waitsome( incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( event_gen_active_for_group && xnb_active )
    {
        int        j, tmp, cur;
        MPI_Status tmpstat;

        cur = 0;

        for ( i = 0; i < incount; ++i )
        {
            orig_req = scorep_mpi_saved_request_get( i );

            if ( orig_req )
            {
                for ( j = cur; j < *outcount && i != array_of_indices[ j ]; ++j )
                {
                    ;
                }

                if ( j < *outcount )
                {
                    tmpstat = array_of_statuses[ cur ];
                  #if !defined( SCOREP_MPI_NO_HOOKS )
                    if ( SCOREP_IS_MPI_HOOKS_ON )
                    {
                        SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( orig_req, &( array_of_statuses[ cur ] ), start_time_stamp );
                    }
                          #endif
                    scorep_mpi_check_request( orig_req, &( array_of_statuses[ cur ] ) );
                    array_of_statuses[ j ] = tmpstat;

                    tmp                     = array_of_indices[ cur ];
                    array_of_indices[ cur ] = array_of_indices[ j ];
                    array_of_indices[ j ]   = tmp;

                    ++cur;
                }
                else if ( orig_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE )
                {
                    SCOREP_MpiRequestTested( orig_req->id );
                }
            }
        }
    }
    else
    {
        for ( i = 0; i < *outcount; ++i )
        {
            orig_req = scorep_mpi_saved_request_get( array_of_indices[ i ] );
                  #if !defined( SCOREP_MPI_NO_HOOKS )
            if ( SCOREP_IS_MPI_HOOKS_ON )
            {
                SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( orig_req, &( array_of_statuses[ i ] ), start_time_stamp );
            }
          #endif
            scorep_mpi_check_request( orig_req, &( array_of_statuses[ i ] ) );
        }
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITSOME ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAITSOME ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 * @name Test functions
 * @{
 */

#if HAVE( DECL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Test
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Test( MPI_Request* request,
          int*         flag,
          MPI_Status*  status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xtest_active               = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XREQTEST );
    int                 return_val;
    scorep_mpi_request* orig_req;
    MPI_Status          mystatus;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

    if ( status == MPI_STATUS_IGNORE )
    {
        status = &mystatus;
    }
    orig_req = scorep_mpi_request_get( *request );
    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Test( request, flag, status );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( *flag )
    {
          #if !defined( SCOREP_MPI_NO_HOOKS )
        if ( SCOREP_IS_MPI_HOOKS_ON )
        {
            SCOREP_Hooks_Post_MPI_Asynch_Complete( orig_req, status, start_time_stamp );
        }
      #endif
        scorep_mpi_check_request( orig_req, status );
    }
    else if ( orig_req && event_gen_active_for_group && xtest_active )
    {
        SCOREP_MpiRequestTested( orig_req->id );
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Testany
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Testany( int          count,
             MPI_Request* array_of_requests,
             int*         index,
             int*         flag,
             MPI_Status*  status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xtest_active               = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XREQTEST );
    int                 return_val;
    scorep_mpi_request* orig_req;
    MPI_Status          mystatus;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTANY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTANY ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

    if ( status == MPI_STATUS_IGNORE )
    {
        status = &mystatus;
    }
    scorep_mpi_save_request_array( array_of_requests, count );
    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Testany( count, array_of_requests, index, flag, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active_for_group && xtest_active )
    {
        int i;

        for ( i = 0; i < count; ++i )
        {
            orig_req = scorep_mpi_saved_request_get( i );

            if ( *index == i )
            {
          #if !defined( SCOREP_MPI_NO_HOOKS )
                if ( SCOREP_IS_MPI_HOOKS_ON )
                {
                    SCOREP_Hooks_Post_MPI_Asynch_Complete( orig_req, status, start_time_stamp );
                }
          #endif
                scorep_mpi_check_request( orig_req, status );
            }
            else if ( orig_req && ( orig_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE ) )
            {
                SCOREP_MpiRequestTested( orig_req->id );
            }
        }
    }
    else if ( *flag && *index != MPI_UNDEFINED )
    {
        orig_req = scorep_mpi_saved_request_get( *index );
      #if !defined( SCOREP_MPI_NO_HOOKS )
        if ( SCOREP_IS_MPI_HOOKS_ON )
        {
            SCOREP_Hooks_Post_MPI_Asynch_Complete( orig_req, status, start_time_stamp );
        }
      #endif
        scorep_mpi_check_request( orig_req, status );
    }
    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTANY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTANY ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Testall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Testall( int          count,
             MPI_Request* array_of_requests,
             int*         flag,
             MPI_Status*  array_of_statuses )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xtest_active               = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XREQTEST );
    int                 return_val;
    int                 i;
    scorep_mpi_request* orig_req;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTALL ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

  #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( count );
    }
  #endif

    scorep_mpi_save_request_array( array_of_requests, count );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Testall( count, array_of_requests, flag, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( *flag )
    {
        for ( i = 0; i < count; i++ )
        {
            orig_req = scorep_mpi_saved_request_get( i );
          #if !defined( SCOREP_MPI_NO_HOOKS )
            if ( SCOREP_IS_MPI_HOOKS_ON )
            {
                SCOREP_Hooks_Post_MPI_Asynch_Complete( orig_req, &( array_of_statuses[ i ] ), start_time_stamp );
            }
          #endif
            scorep_mpi_check_request( orig_req, &( array_of_statuses[ i ] ) );
        }
    }
    else if ( event_gen_active_for_group && xtest_active )
    {
        int i;

        for ( i = 0; i < count; i++ )
        {
            orig_req = scorep_mpi_saved_request_get( i );
            if ( orig_req && ( orig_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE ) )
            {
                SCOREP_MpiRequestTested( orig_req->id );
            }
        }
    }
    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTALL ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Testsome
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Testsome( int          incount,
              MPI_Request* array_of_requests,
              int*         outcount,
              int*         array_of_indices,
              MPI_Status*  array_of_statuses )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xtest_active               = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XREQTEST );
    int                 return_val;
    int                 i;
    scorep_mpi_request* orig_req;
    uint64_t            start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTSOME ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTSOME ] );
        }
    }

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        start_time_stamp = SCOREP_GetLastTimeStamp();
    }
  #endif

  #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( incount );
    }
  #endif

    scorep_mpi_save_request_array( array_of_requests, incount );
    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Testsome( incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active_for_group && xtest_active )
    {
        int        cur, j, tmp;
        MPI_Status tmpstat;

        cur = 0;

        for ( i = 0; i < incount; ++i )
        {
            orig_req = scorep_mpi_saved_request_get( i );

            if ( orig_req )
            {
                for ( j = cur; j < *outcount && i != array_of_indices[ j ]; ++j )
                {
                    ;
                }

                if ( j < *outcount )
                {
                    tmpstat = array_of_statuses[ cur ];
                  #if !defined( SCOREP_MPI_NO_HOOKS )
                    if ( SCOREP_IS_MPI_HOOKS_ON )
                    {
                        SCOREP_Hooks_Post_MPI_Asynch_Complete( orig_req, &( array_of_statuses[ cur ] ), start_time_stamp );
                    }
                          #endif
                    scorep_mpi_check_request( orig_req, &( array_of_statuses[ cur ] ) );
                    array_of_statuses[ j ] = tmpstat;

                    tmp                     = array_of_indices[ cur ];
                    array_of_indices[ cur ] = array_of_indices[ j ];
                    array_of_indices[ j ]   = tmp;

                    ++cur;
                }
                else if ( orig_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE )
                {
                    SCOREP_MpiRequestTested( orig_req->id );
                }
            }
        }
    }
    else
    {
        for ( i = 0; i < *outcount; ++i )
        {
            orig_req = scorep_mpi_saved_request_get( array_of_indices[ i ] );
                  #if !defined( SCOREP_MPI_NO_HOOKS )
            if ( SCOREP_IS_MPI_HOOKS_ON )
            {
                SCOREP_Hooks_Post_MPI_Asynch_Complete( orig_req, &( array_of_statuses[ i ] ), start_time_stamp );
            }
          #endif
            scorep_mpi_check_request( orig_req, &( array_of_statuses[ i ] ) );
        }
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTSOME ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TESTSOME ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 * @name Persistent requests
 * @{
 */

#if HAVE( DECL_PMPI_START ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Start
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Start( MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xnb_active                 = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XNONBLOCK );
    int       return_val;
    uint64_t  start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_START ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_START ] );
        }
    }

    if ( event_gen_active_for_group )
    {
        #if !defined( SCOREP_MPI_NO_HOOKS )
        if ( SCOREP_IS_MPI_HOOKS_ON )
        {
            start_time_stamp = SCOREP_GetLastTimeStamp();
        }
    #endif

        scorep_mpi_request* req = scorep_mpi_request_get( *request );
        if ( req && ( req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) )
        {
            req->flags |= SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE;
            if ( ( req->request_type == SCOREP_MPI_REQUEST_TYPE_SEND ) && ( req->payload.p2p.dest != MPI_PROC_NULL ) )
            {
                if ( xnb_active )
                {
                    SCOREP_MpiIsend( req->payload.p2p.dest, req->payload.p2p.comm_handle,
                                     req->payload.p2p.tag, req->payload.p2p.bytes, req->id );
                }
                else
                {
                    SCOREP_MpiSend( req->payload.p2p.dest, req->payload.p2p.comm_handle,
                                    req->payload.p2p.tag, req->payload.p2p.bytes );
                }
            }
            else if ( ( req->request_type == SCOREP_MPI_REQUEST_TYPE_RECV ) && xnb_active )
            {
                SCOREP_MpiIrecvRequest( req->id );
            }
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Start( request );
    SCOREP_EXIT_WRAPPED_REGION();

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        SCOREP_Hooks_Post_MPI_Start( request, start_time_stamp, return_val );
    }
  #endif
    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_START ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_START ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Startall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Startall( int          count,
              MPI_Request* array_of_requests )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xnb_active                 = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XNONBLOCK );
    int       return_val;
    int       i;
    uint64_t  start_time_stamp;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_STARTALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_STARTALL ] );
        }
    }

    if ( event_gen_active_for_group )
    {
        #if !defined( SCOREP_MPI_NO_HOOKS )
        if ( SCOREP_IS_MPI_HOOKS_ON )
        {
            start_time_stamp = SCOREP_GetLastTimeStamp();
        }
    #endif

        for ( i = 0; i < count; i++ )
        {
            scorep_mpi_request* req = scorep_mpi_request_get( array_of_requests[ i ] );

            if ( req && ( req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) )
            {
                req->flags |= SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE;
                if ( ( req->request_type == SCOREP_MPI_REQUEST_TYPE_SEND ) && ( req->payload.p2p.dest != MPI_PROC_NULL ) )
                {
                    SCOREP_MpiIsend( req->payload.p2p.dest, req->payload.p2p.comm_handle,
                                     req->payload.p2p.tag, req->payload.p2p.bytes, req->id );
                }
                else if ( ( req->request_type == SCOREP_MPI_REQUEST_TYPE_RECV ) && xnb_active )
                {
                    SCOREP_MpiIrecvRequest( req->id );
                }
            }
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Startall( count, array_of_requests );
    SCOREP_EXIT_WRAPPED_REGION();

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        for ( i = 0; i < count; i++ )
        {
            SCOREP_Hooks_Post_MPI_Start( &array_of_requests[ i ], start_time_stamp, return_val );
        }
    }
  #endif

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_STARTALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_STARTALL ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 * @name Miscelleaneous
 * @{
 */

#if HAVE( DECL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Request_free
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Request_free( MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xnb_active                 = ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XNONBLOCK );
    int                 orig_req_null              = ( *request == MPI_REQUEST_NULL );
    int                 return_val;
    scorep_mpi_request* req;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_FREE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_FREE ] );
        }
    }

    req = scorep_mpi_request_get( *request );
  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        SCOREP_Hooks_Pre_MPI_Request_free( req );
    }
  #endif
    if ( req )
    {
        if ( req->flags & SCOREP_MPI_REQUEST_FLAG_CAN_CANCEL && event_gen_active_for_group && xnb_active )
        {
            MPI_Status status;
            int        cancelled;
            /* -- Must check if request was cancelled and write the
             *    cancel event. Not doing so will confuse the trace
             *    analysis.
             */
            return_val = PMPI_Wait( request, &status );
            PMPI_Test_cancelled( &status, &cancelled );

            if ( cancelled )
            {
                SCOREP_MpiRequestCancelled( req->id );
            }
        }

        if ( ( req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) && ( req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE ) )
        {
            /* mark active requests for deallocation */
            req->flags |= SCOREP_MPI_REQUEST_FLAG_DEALLOCATE;
        }
        else
        {
            /* deallocate inactive requests -*/
            scorep_mpi_request_free( req );
        }
    }

    /* -- We had to call PMPI_Wait for cancelable requests, which already
     *    frees (non-persistent) requests itself and sets them to
     *    MPI_REQUEST_NULL.
     *    As MPI_Request_free does not really like being called with
     *    MPI_REQUEST_NULL, we have to catch this situation here and only
     *    pass MPI_REQUEST_NULL if the application explicitly wanted that
     *    for some reason.
     */
    if ( *request != MPI_REQUEST_NULL || orig_req_null )
    {
        SCOREP_ENTER_WRAPPED_REGION();
        return_val = PMPI_Request_free( request );
        SCOREP_EXIT_WRAPPED_REGION();
    }


    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_FREE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_FREE ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Measurement wrapper for MPI_Cancel
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Cancel( MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    int                 return_val;
    scorep_mpi_request* req;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CANCEL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CANCEL ] );
        }
    }

    /* Mark request as cancelable and check for successful cancellation
     * on request completion or MPI_Request_free.
     * If XNONBLOCK is enabled, there will be a 'cancelled' event
     * instead of a normal completion event in the trace, which can be
     * checked for by the trace analysis.
     */

    req = scorep_mpi_request_get( *request );

    if ( req )
    {
        req->flags |= SCOREP_MPI_REQUEST_FLAG_CAN_CANCEL;
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cancel( request );
    SCOREP_EXIT_WRAPPED_REGION();

  #if !defined( SCOREP_MPI_NO_HOOKS )
    if ( SCOREP_IS_MPI_HOOKS_ON )
    {
        SCOREP_Hooks_Post_MPI_Cancel( req );
    }
  #endif

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CANCEL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CANCEL ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test_cancelled )
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup request
 * Triggers an enter and exit event.
 * It wraps the MPI_Test_cancelled call with enter and exit events.
 */
int
MPI_Test_cancelled( SCOREP_MPI_CONST_DECL MPI_Status* status, int* flag )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST_CANCELLED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST_CANCELLED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Test_cancelled( status, flag );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST_CANCELLED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TEST_CANCELLED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 */
