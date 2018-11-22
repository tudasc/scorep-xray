/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SHMEM_H
#define SCOREP_SHMEM_H

/**
 * @file
 * @ingroup    SHMEM_Wrapper
 *
 * @brief It includes all source files for the SHMEM wrappers.
 *        Macros and declarations common to all SHMEM wrappers.
 */

#include <stdbool.h>
#include <shmem.h>

#if HAVE( SHMEM_PROFILING_HEADER )
#include <pshmem.h>
#endif

#include <SCOREP_AllocMetric.h>

#include "scorep_shmem_communicator_mgmt.h"


/** @defgroup SHMEM_Wrapper SCOREP SHMEM wrapper library
 *
 * @{
 */

/** @def SCOREP_SHMEM_IS_EVENT_GEN_ON
 *  Check whether event generation is turned on.
 */
#define SCOREP_SHMEM_IS_EVENT_GEN_ON ( scorep_shmem_generate_events )

/** @def SCOREP_SHMEM_EVENT_GEN_OFF
 *  Turn off event generation for SHMEM adapter. It is used inside the
 *  wrappers and the measurement core to avoid events from SHMEM
 *  function calls.
 */
#define SCOREP_SHMEM_EVENT_GEN_OFF() scorep_shmem_generate_events = false

/** @def SCOREP_SHMEM_EVENT_GEN_ON
 *  Turn on event generation for SHMEM wrappers. See
 *  SCOREP_SHMEM_EVENT_GEN_OFF().
 */
#define SCOREP_SHMEM_EVENT_GEN_ON()  scorep_shmem_generate_events = true

/** @def SCOREP_SHMEM_IS_RMA_OP_COMPLETE_RECORD_NEEDED
 *  Check whether RmaOpCompleteRemote record should be written.
 */
#define SCOREP_SHMEM_IS_RMA_OP_COMPLETE_RECORD_NEEDED ( scorep_shmem_write_rma_op_complete_record )

/** @def SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_OFF
 *  Turn off event generation for RmaOpCompleteRemote record.
 *  It is used inside the wrappers to signalize need for writing a
 *  RmaOpCompleteRemote record (e.g. in a shmem_barrier_all event with
 *  preceding put/get operations).
 */
#define SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_OFF() scorep_shmem_write_rma_op_complete_record = false

/** @def SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON
 *  Turn on event generation for RmaOpCompleteRemote record. See
 *  SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_OFF().
 */
#define SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON()  scorep_shmem_write_rma_op_complete_record = true

#define NO_PROCESSING_ELEMENT                    UINT32_MAX

/**
 * Flag which indicates whether event generation is turned on/off.
 */
extern bool scorep_shmem_generate_events;

/**
 * Flag which indicates whether RmaOpCompleteRemote record should be written or not.
 */
extern bool scorep_shmem_write_rma_op_complete_record;

/**
 * Handle of the interim 'WORLD' window.
 */
extern SCOREP_RmaWindowHandle scorep_shmem_world_window_handle;

/**
 * Handle of the interim 'SELF' window.
 */
extern SCOREP_RmaWindowHandle scorep_shmem_self_window_handle;

/**
 * Number of SHMEM processing elements.
 */
extern size_t scorep_shmem_number_of_pes;

/**
 * Processing element ID
 */
extern int scorep_shmem_my_rank;

/**
 * Matching ID of consecutive RMA operations.
 */
extern uint64_t scorep_shmem_rma_op_matching_id;

/**
 * Determine own SHMEM rank and number of all PEs.
 */
void
scorep_shmem_rank_and_size( void );

/**
 * Define communicator and RMA window for the group of all PEs.
 */
void
scorep_shmem_setup_comm_world( void );

/**
 * Finalize the communicator management.
 */
void
scorep_shmem_teardown_comm_world( void );

/**
 * Unification of locations.
 */
void
scorep_shmem_define_shmem_locations( void );

/**
 * Unification of groups.
 */
void
scorep_shmem_define_shmem_group( void );

/**
 * Finalization of unification.
 */
void
scorep_shmem_finalize_shmem_mapping( void );

/**
 * Record memory usage.
 */
extern bool scorep_shmem_memory_recording;

/**
 * Metric to record memory usage.
 */
extern SCOREP_AllocMetric* scorep_shmem_allocations_metric;


/**
 * Attribute for allocation size.
 */
extern SCOREP_AttributeHandle scorep_shmem_memory_alloc_size_attribute;
extern SCOREP_AttributeHandle scorep_shmem_memory_dealloc_size_attribute;


/** @} */

#endif /* SCOREP_SHMEM_H */
