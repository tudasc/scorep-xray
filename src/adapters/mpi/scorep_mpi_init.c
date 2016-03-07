/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
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
#include "SCOREP_Mpi.h"
#include "scorep_mpi_communicator.h"
#include "scorep_mpi_communicator_mgmt.h"
#include "scorep_mpi_request.h"

#include <stdlib.h>

void
scorep_mpi_request_finalize( void );

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUS_SIZE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_status_size.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUS_SIZE \
    F77_FUNC_( scorep_fortran_get_mpi_status_size, \
               SCOREP_FORTRAN_GET_MPI_STATUS_SIZE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_BOTTOM
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_bottom.
 */
#define SCOREP_FORTRAN_GET_MPI_BOTTOM \
    F77_FUNC_( scorep_fortran_get_mpi_bottom, \
               SCOREP_FORTRAN_GET_MPI_BOTTOM )

/**
 * @def SCOREP_FORTRAN_GET_MPI_IN_PLACE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_in_place.
 */
#define SCOREP_FORTRAN_GET_MPI_IN_PLACE \
    F77_FUNC_( scorep_fortran_get_mpi_in_place, \
               SCOREP_FORTRAN_GET_MPI_IN_PLACE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_status_ignore.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE \
    F77_FUNC_( scorep_fortran_get_mpi_status_ignore, \
               SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_statuses_ignore.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE \
    F77_FUNC_( scorep_fortran_get_mpi_statuses_ignore, \
               SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_UNWEIGHTED
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_unweighted.
 */
#define SCOREP_FORTRAN_GET_MPI_UNWEIGHTED \
    F77_FUNC_( scorep_fortran_get_mpi_unweighted, \
               SCOREP_FORTRAN_GET_MPI_UNWEIGHTED )

/**
   Stores the value of the Fortran MPI constant MPI_STATUS_SIZE. It is used for
   Fortran-C conversions.
 */
int scorep_mpi_status_size = 0;

/**
   address of fortran variable used as reference for MPI_BOTTOM
 */
void* scorep_mpi_fortran_bottom = NULL;

/**
   address of fortran variable used as reference for MPI_IN_PLACE
 */
void* scorep_mpi_fortran_in_place = NULL;

/**
   address of fortran variable used as reference for MPI_STATUS_IGNORE
 */
void* scorep_mpi_fortran_status_ignore = NULL;

/**
   address of fortran variable used as reference for MPI_STATUSES_IGNORE
 */
void* scorep_mpi_fortran_statuses_ignore = NULL;

/**
   address of fortran variable used as reference for MPI_UNWEIGHTED
 */
void* scorep_mpi_fortran_unweighted = NULL;


SCOREP_AllocMetric*    scorep_mpi_allocations_metric            = NULL;
SCOREP_AttributeHandle scorep_mpi_memory_alloc_size_attribute   = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_mpi_memory_dealloc_size_attribute = SCOREP_INVALID_ATTRIBUTE;

/**
   External fortran function to retrieve the constant value
   MPI_STATUS_SIZE defined in Fortran MPI. It is used for Fortran-C
   conversions.
   @param mpi_status_size Address of a variable where the value is to
   be stored.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUS_SIZE( int* status_size );

/**
   External fortran function to trigger a callback which sets MPI_BOTTOM.
 */
extern void
SCOREP_FORTRAN_GET_MPI_BOTTOM( void );

/**
   External fortran function to trigger a callback which sets MPI_IN_PLACE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_IN_PLACE( void );

/**
   External fortran function to trigger a callback which sets MPI_STATUS_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE( void );

/**
   External fortran function to trigger a callback which sets
   MPI_STATUSES_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE( void );

/**
   External fortran function to trigger a callback which sets
   MPI_STATUSES_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_UNWEIGHTED( void );

#include "scorep_mpi_confvars.inc.c"

static size_t mpi_subsystem_id;

/**
   Implementation of the adapter_register function of the @ref
   SCOREP_Subsystem struct for the initialization process of the MPI
   adapter.
 */
static SCOREP_ErrorCode
mpi_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    mpi_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegister( "mpi", scorep_mpi_confvars );
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
    SCOREP_FORTRAN_GET_MPI_STATUS_SIZE( &scorep_mpi_status_size );
#if HAVE( MPI_BOTTOM )
    SCOREP_FORTRAN_GET_MPI_BOTTOM();
#endif
#if HAVE( MPI_IN_PLACE )
    SCOREP_FORTRAN_GET_MPI_IN_PLACE();
#endif
#if HAVE( MPI_STATUS_IGNORE )
    SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE();
#endif
#if HAVE( MPI_STATUSES_IGNORE )
    SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE();
#endif
#if HAVE( MPI_UNWEIGHTED )
    SCOREP_FORTRAN_GET_MPI_UNWEIGHTED();
#endif

    scorep_mpi_win_init();
    scorep_mpi_register_regions();

    if ( scorep_mpi_memory_recording )
    {
        SCOREP_AllocMetric_New( "Process memory usage (MPI)",
                                &scorep_mpi_allocations_metric );

        scorep_mpi_memory_alloc_size_attribute =
            SCOREP_AllocMetric_GetAllocationSizeAttribute();
        scorep_mpi_memory_dealloc_size_attribute =
            SCOREP_AllocMetric_GetDeallocationSizeAttribute();
    }

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

#if !defined( SCOREP_MPI_NO_HOOKS )
    scorep_mpiprofile_init();
#endif

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
    scorep_mpi_request_finalize();
    scorep_mpi_comm_finalize();

    if ( scorep_mpi_memory_recording )
    {
        SCOREP_AllocMetric_Destroy( scorep_mpi_allocations_metric );
    }

    UTILS_DEBUG_EXIT();
}

/**
   Define the group of all MPI locations.
 */
static SCOREP_ErrorCode
mpi_subsystem_pre_unify( void )
{
    UTILS_DEBUG_ENTRY();

    /* Define the group of all MPI locations. */
    scorep_mpi_unify_define_mpi_locations();

    return SCOREP_SUCCESS;
}

/**
   Unify the MPI communicators.
 */
static SCOREP_ErrorCode
mpi_subsystem_post_unify( void )
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

/* The initialization struct for the MPI adapter */
const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter =
{
    .subsystem_name       = "MPI",
    .subsystem_register   = &mpi_subsystem_register,
    .subsystem_init       = &mpi_subsystem_init,
    .subsystem_begin      = &mpi_subsystem_begin,
    .subsystem_init_mpp   = &mpi_subsystem_init_mpp,
    .subsystem_end        = &mpi_subsystem_end,
    .subsystem_finalize   = &mpi_subsystem_finalize,
    .subsystem_pre_unify  = &mpi_subsystem_pre_unify,
    .subsystem_post_unify = &mpi_subsystem_post_unify,
    .subsystem_deregister = &mpi_subsystem_deregister
};

/**
   Flag to indicate whether event generation is turned on or off. If
   it is set to true, events are generated. If it is set to false, no
   events are generated.
 */
bool scorep_mpi_generate_events = false;

/**
 * @internal
 * Data structure to store request info for effective request tracking
 */
struct scorep_mpi_request_hash
    scorep_mpi_request_table[ SCOREP_MPI_REQUEST_TABLE_SIZE ] = {
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */ /* 128 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  32 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   2 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   4 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   6 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*   8 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  10 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  12 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  14 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  16 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  18 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  20 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  22 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  24 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  26 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  28 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE }, /*  30 */
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE },
    { 0, 0, 0, SCOREP_MPI_REQUEST_BLOCK_SIZE } /*  32 */ /* 256 */
};


void
scorep_mpi_request_finalize( void )
{
    struct scorep_mpi_request_block* block;
    int                              i;

    /* free request blocks */

    for ( i = 0; i < SCOREP_MPI_REQUEST_TABLE_SIZE; ++i )
    {
        while ( scorep_mpi_request_table[ i ].head_block )
        {
            block                                    = scorep_mpi_request_table[ i ].head_block;
            scorep_mpi_request_table[ i ].head_block = scorep_mpi_request_table[ i ].head_block->next;
            free( block );
        }
    }
}
