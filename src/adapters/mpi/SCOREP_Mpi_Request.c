/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013, 2020-2022,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2018, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2019, 2022,
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
#include <UTILS_Mutex.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>

#include <stddef.h>
#include <mpi.h>

/**
 * @name Waiting functions
 * @{
 */

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Wait
 */
int
PMPI_Wait( MPI_Request* request,
           MPI_Status*  status );

/**
 * Measurement wrapper for MPI_Wait
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Wait( MPI_Request* request,
          MPI_Status*  status )
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
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAIT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_WAIT ] );
        }
    }

    if ( status == MPI_STATUS_IGNORE )
    {
        status = scorep_mpi_get_status_array( 1 );
    }

    scorep_mpi_save_request_array( request, 1 );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Wait( request, status );
    SCOREP_EXIT_WRAPPED_REGION();

    scorep_mpi_check_all_or_none( 1, REQUESTS_COMPLETED, status );

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Waitall
 */
int
PMPI_Waitall( int          count,
              MPI_Request* requests,
              MPI_Status*  array_of_statuses );

/**
 * Measurement wrapper for MPI_Waitall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Waitall( int          count,
             MPI_Request* requests,
             MPI_Status*  array_of_statuses )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    int       i;
    int       return_val;

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

    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( count );
    }

    scorep_mpi_save_request_array( requests, count );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Waitall( count, requests, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();

    scorep_mpi_check_all_or_none( count, REQUESTS_COMPLETED, array_of_statuses );

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Waitany
 */
int
PMPI_Waitany( int          count,
              MPI_Request* requests,
              int*         index,
              MPI_Status*  status );

/**
 * Measurement wrapper for MPI_Waitany
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
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
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xtest_active               = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XREQTEST );
    int       return_val;

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

    if ( status == MPI_STATUS_IGNORE )
    {
        status = scorep_mpi_get_status_array( 1 );
    }

    scorep_mpi_save_request_array( requests, count );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Waitany( count, requests, index, status );
    SCOREP_EXIT_WRAPPED_REGION();

    /* array_of_requests contains active handles */
    if ( *index != MPI_UNDEFINED )
    {
        if ( event_gen_active_for_group && xtest_active )
        {
            scorep_mpi_check_some_test_some( count, 1, index, status );
        }
        else
        {
            scorep_mpi_check_some( count, 1, index, status );
        }
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Waitsome
 */
int
PMPI_Waitsome( int          incount,
               MPI_Request* array_of_requests,
               int*         outcount,
               int*         array_of_indices,
               MPI_Status*  array_of_statuses );

/**
 * Measurement wrapper for MPI_Waitsome
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
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
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xtest_active               = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XREQTEST );
    int       return_val;
    int       i;

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

    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( incount );
    }

    scorep_mpi_save_request_array( array_of_requests, incount );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Waitsome( incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();

    /* array_of_requests contains active handles */
    if ( *outcount != MPI_UNDEFINED )
    {
        if ( event_gen_active_for_group && xtest_active )
        {
            scorep_mpi_check_some_test_some( incount, *outcount, array_of_indices, array_of_statuses );
        }
        else
        {
            scorep_mpi_check_some( incount, *outcount, array_of_indices, array_of_statuses );
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Test
 */
int
PMPI_Test( MPI_Request* request,
           int*         flag,
           MPI_Status*  status );

/**
 * Measurement wrapper for MPI_Test
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Test( MPI_Request* request,
          int*         flag,
          MPI_Status*  status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xtest_active               = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XREQTEST );
    int       return_val;

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

    if ( status == MPI_STATUS_IGNORE )
    {
        status = scorep_mpi_get_status_array( 1 );
    }

    scorep_mpi_save_request_array( request, 1 );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Test( request, flag, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active_for_group && xtest_active )
    {
        scorep_mpi_check_all_or_test_all( 1, *flag, status );
    }
    else
    {
        scorep_mpi_check_all_or_none( 1, *flag, status );
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Testall
 */
int
PMPI_Testall( int          count,
              MPI_Request* array_of_requests,
              int*         flag,
              MPI_Status*  array_of_statuses );

/**
 * Measurement wrapper for MPI_Testall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
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
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xtest_active               = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XREQTEST );
    int       return_val;

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

    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( count );
    }

    scorep_mpi_save_request_array( array_of_requests, count );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Testall( count, array_of_requests, flag, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active_for_group && xtest_active )
    {
        scorep_mpi_check_all_or_test_all( count, *flag, array_of_statuses );
    }
    else
    {
        scorep_mpi_check_all_or_none( count, *flag, array_of_statuses );
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Testany
 */
int
PMPI_Testany( int          count,
              MPI_Request* array_of_requests,
              int*         index,
              int*         flag,
              MPI_Status*  status );

/**
 * Measurement wrapper for MPI_Testany
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
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
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xtest_active               = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XREQTEST );
    int       return_val;

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

    if ( status == MPI_STATUS_IGNORE )
    {
        status = scorep_mpi_get_status_array( 1 );
    }
    scorep_mpi_save_request_array( array_of_requests, count );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Testany( count, array_of_requests, index, flag, status );
    SCOREP_EXIT_WRAPPED_REGION();

    /* array_of_requests contains active handles, but none were completed */
    if ( !*flag )
    {
        if ( event_gen_active_for_group && xtest_active )
        {
            scorep_mpi_test_all( count );
        }
    }
    /* array_of_requests contains active handles, and one was completed */
    else if ( *index != MPI_UNDEFINED )
    {
        if ( event_gen_active_for_group && xtest_active )
        {
            scorep_mpi_check_some_test_some( count, 1, index, status );
        }
        else
        {
            scorep_mpi_check_some( count, 1, index, status );
        }
    }
    /* else, array_of_requests contains no active handles */

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Testsome
 */
int
PMPI_Testsome( int          incount,
               MPI_Request* array_of_requests,
               int*         outcount,
               int*         array_of_indices,
               MPI_Status*  array_of_statuses );

/**
 * Measurement wrapper for MPI_Testsome
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
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
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xtest_active               = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XREQTEST );
    int       return_val;

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

    if ( array_of_statuses == MPI_STATUSES_IGNORE )
    {
        /* allocate status array for internal use */
        array_of_statuses = scorep_mpi_get_status_array( incount );
    }

    scorep_mpi_save_request_array( array_of_requests, incount );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Testsome( incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses );
    SCOREP_EXIT_WRAPPED_REGION();

    /* array_of_requests contains active handles */
    if ( *outcount != MPI_UNDEFINED )
    {
        if ( event_gen_active_for_group && xtest_active )
        {
            scorep_mpi_check_some_test_some( incount, *outcount, array_of_indices, array_of_statuses );
        }
        else
        {
            scorep_mpi_check_some( incount, *outcount, array_of_indices, array_of_statuses );
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_START ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Start
 */
int
PMPI_Start( MPI_Request* request );

/**
 * Measurement wrapper for MPI_Start
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Start( MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xnb_active                 = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XNONBLOCK );
    int       return_val;

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
        scorep_mpi_request* scorep_req = scorep_mpi_request_get( *request );

        if ( scorep_req && ( scorep_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) )
        {
            scorep_req->flags |= SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE;
            if ( ( scorep_req->request_type == SCOREP_MPI_REQUEST_TYPE_SEND ) && ( scorep_req->payload.p2p.dest != MPI_PROC_NULL ) )
            {
                if ( xnb_active )
                {
                    SCOREP_MpiIsend( scorep_req->payload.p2p.dest, scorep_req->payload.p2p.comm_handle,
                                     scorep_req->payload.p2p.tag, scorep_req->payload.p2p.bytes, scorep_req->id );
                }
                else
                {
                    SCOREP_MpiSend( scorep_req->payload.p2p.dest, scorep_req->payload.p2p.comm_handle,
                                    scorep_req->payload.p2p.tag, scorep_req->payload.p2p.bytes );
                }
            }
            else if ( ( scorep_req->request_type == SCOREP_MPI_REQUEST_TYPE_RECV ) && xnb_active )
            {
                SCOREP_MpiIrecvRequest( scorep_req->id );
            }
        }
        scorep_mpi_unmark_request( scorep_req );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Start( request );
    SCOREP_EXIT_WRAPPED_REGION();

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Startall
 */
int
PMPI_Startall( int          count,
               MPI_Request* array_of_requests );

/**
 * Measurement wrapper for MPI_Startall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
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
    const int xnb_active                 = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XNONBLOCK );
    int       return_val;
    int       i;

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
        for ( i = 0; i < count; i++ )
        {
            scorep_mpi_request* scorep_req = scorep_mpi_request_get( array_of_requests[ i ] );

            if ( scorep_req && ( scorep_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) )
            {
                scorep_req->flags |= SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE;
                if ( ( scorep_req->request_type == SCOREP_MPI_REQUEST_TYPE_SEND ) && ( scorep_req->payload.p2p.dest != MPI_PROC_NULL ) )
                {
                    SCOREP_MpiIsend( scorep_req->payload.p2p.dest, scorep_req->payload.p2p.comm_handle,
                                     scorep_req->payload.p2p.tag, scorep_req->payload.p2p.bytes, scorep_req->id );
                }
                else if ( ( scorep_req->request_type == SCOREP_MPI_REQUEST_TYPE_RECV ) && xnb_active )
                {
                    SCOREP_MpiIrecvRequest( scorep_req->id );
                }
            }
            scorep_mpi_unmark_request( scorep_req );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Startall( count, array_of_requests );
    SCOREP_EXIT_WRAPPED_REGION();

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
 * @name Partitioned communication
 * @{
 */

#if HAVE( MPI_4_0_SYMBOL_PMPI_PARRIVED ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Parrived )
/**
 * Declaration of PMPI-symbol for MPI_Parrived
 */
int
PMPI_Parrived( MPI_Request request,
               int         partition,
               int*        flag );

/**
 * Measurement wrapper for MPI_Parrived
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * Triggers an enter and exit event.
 * It wraps the MPI_Parrived call with enter and exit events.
 */
int
MPI_Parrived( MPI_Request request, int partition, int* flag )
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
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PARRIVED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PARRIVED ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Parrived( request, partition, flag );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PARRIVED ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PARRIVED ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( MPI_4_0_SYMBOL_PMPI_PREADY ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Pready )
/**
 * Declaration of PMPI-symbol for MPI_Pready
 */
int
PMPI_Pready( int         partition,
             MPI_Request request );

/**
 * Measurement wrapper for MPI_Pready
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * Triggers an enter and exit event.
 * It wraps the MPI_Pready call with enter and exit events.
 */
int
MPI_Pready( int partition, MPI_Request request )
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
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Pready( partition, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif


#if HAVE( MPI_4_0_SYMBOL_PMPI_PREADY_LIST ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Pready_list ) && defined( SCOREP_MPI_PREADY_LIST_PROTO_ARGS )
/**
 * Declaration of PMPI-symbol for MPI_Pready_list
 */
int
PMPI_Pready_list SCOREP_MPI_PREADY_LIST_PROTO_ARGS;

/**
 * Measurement wrapper for MPI_Pready_list
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_StdWithProto.w
 * @note C interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * Triggers an enter and exit event.
 * It wraps the MPI_Pready_list call with enter and exit events.
 */
int
MPI_Pready_list SCOREP_MPI_PREADY_LIST_PROTO_ARGS
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
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_LIST ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_LIST ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Pready_list( length, array_of_partitions, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_LIST ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_LIST ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( MPI_4_0_SYMBOL_PMPI_PREADY_RANGE ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Pready_range )
/**
 * Declaration of PMPI-symbol for MPI_Pready_range
 */
int
PMPI_Pready_range( int         partition_low,
                   int         partition_high,
                   MPI_Request request );

/**
 * Measurement wrapper for MPI_Pready_range
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * Triggers an enter and exit event.
 * It wraps the MPI_Pready_range call with enter and exit events.
 */
int
MPI_Pready_range( int partition_low, int partition_high, MPI_Request request )
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
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_RANGE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_RANGE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Pready_range( partition_low, partition_high, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_RANGE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_PREADY_RANGE ] );
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

/**
 * @}
 * @name Miscelleaneous
 * @{
 */

#if HAVE( MPI_1_0_SYMBOL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Request_free
 */
int
PMPI_Request_free( MPI_Request* request );

/**
 * Measurement wrapper for MPI_Request_free
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Request_free( MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int xnb_active                 = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XNONBLOCK );
    int       orig_req_null              = ( *request == MPI_REQUEST_NULL );
    int       return_val;

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

    scorep_mpi_request* scorep_req = scorep_mpi_request_get( *request );

    if ( scorep_req )
    {
        if ( scorep_req->flags & SCOREP_MPI_REQUEST_FLAG_CAN_CANCEL && event_gen_active_for_group && xnb_active )
        {
            MPI_Status* status = scorep_mpi_get_status_array( 1 );
            int         cancelled;
            /* -- Must check if request was cancelled and write the
             *    cancel event. Not doing so will confuse the trace
             *    analysis.
             */

            return_val = PMPI_Wait( request, status );
            PMPI_Test_cancelled( status, &cancelled );

            if ( cancelled )
            {
                SCOREP_MpiRequestCancelled( scorep_req->id );
            }
        }

        if ( ( scorep_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_PERSISTENT ) && ( scorep_req->flags & SCOREP_MPI_REQUEST_FLAG_IS_ACTIVE ) )
        {
            /* mark active requests for deallocation */
            scorep_req->flags |= SCOREP_MPI_REQUEST_FLAG_DEALLOCATE;
        }
        else
        {
            /* deallocate inactive requests -*/
            scorep_mpi_request_free( scorep_req );
        }
    }
    scorep_mpi_unmark_request( scorep_req );

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Cancel
 */
int
PMPI_Cancel( MPI_Request* request );

/**
 * Measurement wrapper for MPI_Cancel
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Cancel( MPI_Request* request )
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
    scorep_mpi_save_request_array( request, 1 );

    scorep_mpi_request* scorep_req = scorep_mpi_saved_request_get( 0 );

    if ( scorep_req )
    {
        scorep_req->flags |= SCOREP_MPI_REQUEST_FLAG_CAN_CANCEL;
    }

    scorep_mpi_unmark_request( scorep_req );

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cancel( request );
    SCOREP_EXIT_WRAPPED_REGION();

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_REQUEST_GET_STATUS ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Declaration of PMPI-symbol for MPI_Recv
 */

/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 * Triggers an enter and exit event.
 */
int
MPI_Request_get_status( MPI_Request request,
                        int*        flag,
                        MPI_Status* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int           event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int           event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_REQUEST );
    const int           xtest_active               = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_XREQTEST );
    int                 return_val;
    scorep_mpi_request* scorep_req;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_GET_STATUS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_GET_STATUS ] );
        }
    }

    if ( status == MPI_STATUS_IGNORE )
    {
        status = scorep_mpi_get_status_array( 1 );
    }
    scorep_req = scorep_mpi_request_get( request );
    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Request_get_status( request, flag, status );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( *flag )
    {
        scorep_mpi_check_request( scorep_req, status );
        /* mark request as completed */
        if ( scorep_req )
        {
            scorep_req->flags |= SCOREP_MPI_REQUEST_FLAG_IS_COMPLETED;
        }
        scorep_mpi_unmark_request( scorep_req );
    }
    else if ( event_gen_active_for_group && xtest_active )
    {
        scorep_mpi_request_tested( scorep_req );
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_GET_STATUS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_REQUEST_GET_STATUS ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test_cancelled )
/**
 * Declaration of PMPI-symbol for MPI_Test_cancelled
 */
int
PMPI_Test_cancelled( SCOREP_MPI_CONST_DECL MPI_Status* status,
                     int*                              flag );

/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1.0
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
