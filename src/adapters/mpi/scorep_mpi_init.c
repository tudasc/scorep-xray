/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013, 2020-2022,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016, 2019, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017, 2019, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Contains the initialization function implementations for the
 * measurement system.
 */

#include <config.h>

#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include <SCOREP_Config.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_AllocMetric.h>
#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Location.h>
#include <SCOREP_Definitions.h>

#include "SCOREP_Mpi.h"
#include "scorep_mpi_fortran.h"
#include "scorep_mpi_communicator.h"
#include "scorep_mpi_communicator_mgmt.h"
#include "scorep_mpi_request_mgmt.h"
#include "scorep_mpi_io_mgmt.h"

#include <stdlib.h>

/**
   Stores the value of the Fortran MPI constant MPI_STATUS_SIZE. It is used for
   Fortran-C conversions.

   Always keep this, even without MPI Fortran support. Is used in
   check-instrumentation.sh and also in Scalasca.
 */
int scorep_mpi_status_size = 0;

/**
   Flag to indicate whether event generation is turned on or off. If
   it is set to true, events are generated. If it is set to false, no
   events are generated.
   Default true, such that threads which are spawned after initialization
   also record events.
   If no thread-local storage specifier is availble we only support
   MPI_THREAD_SERIALIZED.
 */
bool THREAD_LOCAL_STORAGE_SPECIFIER scorep_mpi_generate_events = true;

bool scorep_mpi_mountinfo_exists = false;

SCOREP_AllocMetric*    scorep_mpi_allocations_metric            = NULL;
SCOREP_AttributeHandle scorep_mpi_memory_alloc_size_attribute   = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_mpi_memory_dealloc_size_attribute = SCOREP_INVALID_ATTRIBUTE;

#include "scorep_mpi_confvars.inc.c"

size_t scorep_mpi_subsystem_id;

static void
enable_derived_groups( void )
{
    /* See derived groups in enum scorep_mpi_groups. */
    #define ENABLE_DERIVED_GROUP( G1, G2 ) \
    if ( ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_##G1 ) && ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_##G2 ) ) \
    { \
        scorep_mpi_enabled |= SCOREP_MPI_ENABLED_##G1##_##G2; \
    }

    ENABLE_DERIVED_GROUP( CG, ERR );
    ENABLE_DERIVED_GROUP( CG, EXT );
    ENABLE_DERIVED_GROUP( CG, MISC );
    ENABLE_DERIVED_GROUP( IO, ERR );
    ENABLE_DERIVED_GROUP( IO, MISC );
    ENABLE_DERIVED_GROUP( RMA, ERR );
    ENABLE_DERIVED_GROUP( RMA, EXT );
    ENABLE_DERIVED_GROUP( RMA, MISC );
    ENABLE_DERIVED_GROUP( TYPE, EXT );
    ENABLE_DERIVED_GROUP( TYPE, MISC );

    #undef ENABLE_DERIVED_GROUP

    /* Enable REQUEST group if any of its depending groups is enabled */
    uint64_t enable_request_mask = SCOREP_MPI_ENABLED_CG   |
                                   SCOREP_MPI_ENABLED_COLL |
                                   SCOREP_MPI_ENABLED_EXT  |
                                   SCOREP_MPI_ENABLED_IO   |
                                   SCOREP_MPI_ENABLED_P2P  |
                                   SCOREP_MPI_ENABLED_RMA;
    if ( scorep_mpi_enabled & enable_request_mask )
    {
        scorep_mpi_enabled |= SCOREP_MPI_ENABLED_REQUEST;
    }
}

static void
deprecate_xnonblock( void )
{
    /* Plan for future versions:
     * 9.0: Issue a deprecation warning when the 'xnonblock' flag is given.
     *      Extended non-blocking communication event are always enabled
     * >9.0: Remove the flag entirely and make it an error.
     */
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_XNONBLOCK )
    {
        int rank;
        PMPI_Comm_rank( MPI_COMM_WORLD, &rank );
        if ( rank == 0 )
        {
            UTILS_DEPRECATED( "Including the 'xnonblock' flag in SCOREP_MPI_ENABLE_GROUPS is deprecated and has no effect.\n"
                              "Extended non-blocking communication events are always enabled.\n" );
        }
    }
}

/**
   Implementation of the adapter_register function of the @ref
   SCOREP_Subsystem struct for the initialization process of the MPI
   adapter.
 */
static SCOREP_ErrorCode
mpi_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    /* Communicate via an undefined reference link error that the
     * application is not a MPI application. */
    extern void
    scorep_hint_No_MPI_startup_symbols_found_in_application( void );

    scorep_hint_No_MPI_startup_symbols_found_in_application();

    scorep_mpi_subsystem_id = subsystem_id;

    SCOREP_ConfigRegister( "mpi", scorep_mpi_confvars );
    SCOREP_ConfigRegister( "topology", scorep_mpi_topology_confvars );

    return SCOREP_SUCCESS;
}

/**
   Implementation of the subsystem_init function of the @ref
   SCOREP_Subsystem struct for the initialization process of the MPI
   adapter.
 */
static SCOREP_ErrorCode
mpi_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_MPI,
        SCOREP_PARADIGM_CLASS_MPP,
        "MPI",
        SCOREP_PARADIGM_FLAG_NONE );
    SCOREP_Paradigms_SetStringProperty( SCOREP_PARADIGM_MPI,
                                        SCOREP_PARADIGM_PROPERTY_COMMUNICATOR_TEMPLATE,
                                        "Comm ${id}" );
    SCOREP_Paradigms_SetStringProperty( SCOREP_PARADIGM_MPI,
                                        SCOREP_PARADIGM_PROPERTY_RMA_WINDOW_TEMPLATE,
                                        "Win ${id}" );
    /* Set Fortran constants */
    scorep_mpi_fortran_init();

    /*
     * Order is important!
     *
     * `scorep_mpi_win_init` may disable a feature, thus needs to run before
     * `enable_derived_groups`.
     *
     * `scorep_mpi_register_regions` already requires the derived groups, thus
     * needs to run after `enable_derived_groups`.
     */
    scorep_mpi_win_init();
    enable_derived_groups();
    scorep_mpi_register_regions();

    if ( scorep_mpi_memory_recording )
    {
        SCOREP_AllocMetric_New( "MPI Memory",
                                &scorep_mpi_allocations_metric );

        scorep_mpi_memory_alloc_size_attribute =
            SCOREP_AllocMetric_GetAllocationSizeAttribute();
        scorep_mpi_memory_dealloc_size_attribute =
            SCOREP_AllocMetric_GetDeallocationSizeAttribute();
    }

    scorep_mpi_io_init();

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
mpi_subsystem_begin( void )
{
    SCOREP_MPI_EVENT_GEN_ON();
    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
mpi_subsystem_init_mpp( void )
{
    /* initialize communicator management and register MPI_COMM_WORLD*/
    scorep_mpi_comm_init();

#if !defined( SCOREP_MPI_NO_RMA )
    scorep_mpi_win_init();
#endif

    deprecate_xnonblock();

    return SCOREP_SUCCESS;
}

static void
mpi_subsystem_end( void )
{
    /* Prevent all further events */
    SCOREP_MPI_EVENT_GEN_OFF();

    if ( scorep_mpi_memory_recording )
    {
        SCOREP_AllocMetric_ReportLeaked( scorep_mpi_allocations_metric );
    }
}

/**
   Implementation of the adapter_finalize function of the @ref
   SCOREP_Subsystem struct for the initialization process of the MPI
   adapter.
 */
static void
mpi_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    /* Prevent all further events */
    scorep_mpi_enabled = 0;

    /* Finalize sub-systems */
    scorep_mpi_win_finalize();
    scorep_mpi_comm_finalize();

    if ( scorep_mpi_memory_recording )
    {
        SCOREP_AllocMetric_Destroy( scorep_mpi_allocations_metric );
    }

    scorep_mpi_io_finalize();

    UTILS_DEBUG_EXIT();
}

/**
   Unify the MPI communicators.
 */
static SCOREP_ErrorCode
mpi_subsystem_pre_unify( void )
{
    UTILS_DEBUG_ENTRY();

    /* Unify the MPI communicators. */
    scorep_mpi_unify_communicators();

    return SCOREP_SUCCESS;
}

/**
   Implementation of the adapter_deregister function of the @ref
   SCOREP_Subsystem struct for the initialization process of the MPI
   adapter.
 */
static void
mpi_subsystem_deregister( void )
{
    int res;

    UTILS_DEBUG_ENTRY();

    /* Finalize MPI.
       MPICH somehow creates some extra processes/threads. If
       PMPI_Finalize is called from the exit handler of SCOREP, these
       processes also try to execute MPI_Finalize.  This causes
       errors, thus, we test if the call to PMPI_Finalize is save.
     */
    PMPI_Initialized( &res );
    if ( res )
    {
        PMPI_Finalized( &res );
        if ( !res )
        {
            SCOREP_FinalizeMppMeasurement();
            PMPI_Finalize();
        }
    }
}

static SCOREP_ErrorCode
mpi_init_location( struct SCOREP_Location* newLocation,
                   struct SCOREP_Location* parentLocation )
{
    UTILS_DEBUG_ENTRY();

    if ( SCOREP_Location_GetType( newLocation ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return SCOREP_SUCCESS;
    }

    scorep_mpi_req_mgmt_location_data* storage = SCOREP_Location_AllocForMisc( newLocation,
                                                                               sizeof( *storage ) );

    // 0-initialize object
    memset( storage, 0, sizeof( *storage ) );

    if ( SCOREP_Location_GetId( newLocation ) == 0 )
    {
        storage->req_arr_size = SCOREP_Memory_GetPageSize() / sizeof( MPI_Request );
        storage->req_arr      = SCOREP_Location_AllocForMisc( newLocation,
                                                              SCOREP_Memory_GetPageSize() );

        storage->status_arr_size = SCOREP_Memory_GetPageSize() / sizeof( MPI_Status );
        storage->status_arr      = SCOREP_Location_AllocForMisc( newLocation,
                                                                 SCOREP_Memory_GetPageSize() );
    }

    SCOREP_Location_SetSubsystemData( newLocation,
                                      scorep_mpi_subsystem_id,
                                      storage );

    return SCOREP_SUCCESS;
}

/* The initialization struct for the MPI adapter */
const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter =
{
    .subsystem_name          = "MPI",
    .subsystem_register      = &mpi_subsystem_register,
    .subsystem_init          = &mpi_subsystem_init,
    .subsystem_init_location = &mpi_init_location,
    .subsystem_begin         = &mpi_subsystem_begin,
    .subsystem_init_mpp      = &mpi_subsystem_init_mpp,
    .subsystem_end           = &mpi_subsystem_end,
    .subsystem_finalize      = &mpi_subsystem_finalize,
    .subsystem_pre_unify     = &mpi_subsystem_pre_unify,
    .subsystem_deregister    = &mpi_subsystem_deregister
};
