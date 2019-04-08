/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 * @ingroup    SHMEM_Wrapper
 *
 * @brief Contains the initialization of the SHMEM subsystem.
 */

#include <config.h>

#include "scorep_shmem_internal.h"

#define SCOREP_DEBUG_MODULE_NAME SHMEM
#include <UTILS_Debug.h>

#include <SCOREP_Config.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Events.h>


#include "scorep_shmem_confvars.inc.c"


/**
 * SHMEM subsystem ID
 */
static size_t subsystem_id;

/**
 * Flag to indicate whether event generation is turned on or off. If
 * it is set to TRUE, events are generated. If it is set to false, no
 * events are generated.
 */
bool scorep_shmem_generate_events = false;

/**
 * Flag to indicate whether an RmaOpCompleteRemote record should be
 * written or not. If it is set to TRUE, record will be generated.
 * If it is set to FALSE, no event record is generated.
 */
bool scorep_shmem_write_rma_op_complete_record = false;

SCOREP_AllocMetric* scorep_shmem_allocations_metric = 0;

SCOREP_AttributeHandle scorep_shmem_memory_alloc_size_attribute   = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_shmem_memory_dealloc_size_attribute = SCOREP_INVALID_ATTRIBUTE;

/**
 * Implementation of the adapter_register function of the @ref
 * SCOREP_Subsystem struct for the initialization process of the SHMEM
 * adapter.
 */
static SCOREP_ErrorCode
shmem_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY();

    subsystem_id = subsystemId;


    return SCOREP_ConfigRegister( "shmem",
                                  scorep_shmem_confvars );
}

/**
 * Implementation of the subsystem_init function of the @ref
 * SCOREP_Subsystem struct for the initialization process of the SHMEM
 * adapter.
 */
static SCOREP_ErrorCode
shmem_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_SHMEM,
        SCOREP_PARADIGM_CLASS_MPP,
        SCOREP_SHMEM_NAME,
        SCOREP_PARADIGM_FLAG_RMA_ONLY );
    SCOREP_Paradigms_SetStringProperty( SCOREP_PARADIGM_SHMEM,
                                        SCOREP_PARADIGM_PROPERTY_RMA_WINDOW_TEMPLATE,
                                        "Active set ${id}" );

    scorep_shmem_register_regions();

    if ( scorep_shmem_memory_recording )
    {
        SCOREP_AllocMetric_New( "Process memory usage (SHMEM)",
                                &scorep_shmem_allocations_metric );

        scorep_shmem_memory_alloc_size_attribute =
            SCOREP_AllocMetric_GetAllocationSizeAttribute();
        scorep_shmem_memory_dealloc_size_attribute =
            SCOREP_AllocMetric_GetDeallocationSizeAttribute();
    }


    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
shmem_subsystem_begin( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_SHMEM_EVENT_GEN_ON();

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
shmem_subsystem_init_mpp( void )
{
    /* Determine SHMEM rank and number of PEs */
    scorep_shmem_rank_and_size();

    /* Define communicator and RMA window for the group of all PEs */
    scorep_shmem_setup_comm_world();

    return SCOREP_SUCCESS;
}


static void
shmem_subsystem_end( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_SHMEM_EVENT_GEN_OFF();

    if ( scorep_shmem_memory_recording )
    {
        SCOREP_AllocMetric_ReportLeaked( scorep_shmem_allocations_metric );
    }

    /* Destroy all RmaWin in the master thread. */
    scorep_shmem_close_pe_group();
}

/**
 * Implementation of the adapter_finalize function of the @ref
 * SCOREP_Subsystem struct for the initialization process of the SHMEM
 * adapter.
 */
static void
shmem_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    scorep_shmem_teardown_comm_world();

    if ( scorep_shmem_memory_recording )
    {
        SCOREP_AllocMetric_Destroy( scorep_shmem_allocations_metric );
    }

    UTILS_DEBUG_EXIT();
}

/**
 * Implementation of the subsystem_init_location function of the @ref
 * SCOREP_Subsystem struct for the initialization process of the SHMEM
 * adapter.
 */
static SCOREP_ErrorCode
shmem_subsystem_init_location( struct SCOREP_Location* locationData,
                               struct SCOREP_Location* parent )
{
    UTILS_DEBUG_ENTRY();
    return SCOREP_SUCCESS;
}

/**
 * Implementation of the adapter_finalize_location function of the
 * @ref SCOREP_Subsystem struct for the initialization process of the
 * SHMEM adapter.
 */
static void
shmem_subsystem_finalize_location( struct SCOREP_Location* locationData )
{
    UTILS_DEBUG_ENTRY();
}

/**
 * Define the group of all SHMEM locations.
 */
static SCOREP_ErrorCode
shmem_subsystem_pre_unify( void )
{
    UTILS_DEBUG_ENTRY();

    /* Unify the SHMEM communicators. */
    scorep_shmem_define_shmem_group();

    return SCOREP_SUCCESS;
}

/**
 * Unify the SHMEM communicators.
 */
static SCOREP_ErrorCode
shmem_subsystem_post_unify( void )
{
    UTILS_DEBUG_ENTRY();

    /* Unify the SHMEM communicators. */
    scorep_shmem_finalize_shmem_mapping();

    return SCOREP_SUCCESS;
}

/**
 * Implementation of the adapter_deregister function of the @ref
 * SCOREP_Subsystem struct for the initialization process of the SHMEM
 * adapter.
 */
static void
shmem_subsystem_deregister( void )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_FinalizeMppMeasurement();
#if SHMEM_HAVE_DECL( SHMEM_FINALIZE )
    UTILS_DEBUG( "Calling shmem_finalize" );
    CALL_SHMEM( shmem_finalize )();
#endif
}

/** The initialization struct for the SHMEM adapter */
const SCOREP_Subsystem SCOREP_Subsystem_ShmemAdapter =
{
    .subsystem_name              = "SHMEM",
    .subsystem_register          = &shmem_subsystem_register,
    .subsystem_begin             = &shmem_subsystem_begin,
    .subsystem_init_mpp          = &shmem_subsystem_init_mpp,
    .subsystem_end               = &shmem_subsystem_end,
    .subsystem_finalize          = &shmem_subsystem_finalize,
    .subsystem_init              = &shmem_subsystem_init,
    .subsystem_init_location     = &shmem_subsystem_init_location,
    .subsystem_finalize_location = &shmem_subsystem_finalize_location,
    .subsystem_pre_unify         = &shmem_subsystem_pre_unify,
    .subsystem_post_unify        = &shmem_subsystem_post_unify,
    .subsystem_deregister        = &shmem_subsystem_deregister
};
