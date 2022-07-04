/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016, 2019-2020, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief   Implementation of I/O profiling events.
 *
 */

#include <config.h>
#include <scorep_profile_io.h>
#include <SCOREP_Profile.h>
#include <scorep_profile_location.h>
#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <scorep_status.h>
#include <SCOREP_IoManagement.h>

#include <UTILS_Atomic.h>
#include <UTILS_Mutex.h>

typedef struct io_paradigm_node
{
    /** The I/O bytes read metric for this I/O paradigm level,
     *  indexed with SCOREP_IoOperationMode
     */
    SCOREP_MetricHandle      io_bytes_metric[ SCOREP_IO_OPERATION_MODE_FLUSH ];
    /** Possible children of this paradigm */
    struct io_paradigm_node* children[ SCOREP_INVALID_IO_PARADIGM_TYPE ];
} io_paradigm_node;

static io_paradigm_node io_paradigm_root;
static UTILS_Mutex      io_paradigm_mutex;

/* *******************************************************************************
 * External interface
 * ******************************************************************************/

void
scorep_profile_io_init( void )
{
    UTILS_ASSERT( SCOREP_IO_OPERATION_MODE_READ < SCOREP_IO_OPERATION_MODE_FLUSH
                  && SCOREP_IO_OPERATION_MODE_WRITE < SCOREP_IO_OPERATION_MODE_FLUSH
                  && SCOREP_IO_OPERATION_MODE_FLUSH == 2 );

    io_paradigm_root.io_bytes_metric[ SCOREP_IO_OPERATION_MODE_READ ] =
        SCOREP_Definitions_NewMetric( "io_bytes_read",
                                      "I/O bytes read",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    io_paradigm_root.io_bytes_metric[ SCOREP_IO_OPERATION_MODE_WRITE ] =
        SCOREP_Definitions_NewMetric( "io_bytes_written",
                                      "I/O bytes written",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );
}

static io_paradigm_node*
ensure_io_paradigm( io_paradigm_node*     ioParadigmNode,
                    SCOREP_IoParadigmType ioParadigm )
{
    UTILS_ASSERT( ioParadigmNode );
    UTILS_BUG_ON( ioParadigm >= SCOREP_INVALID_IO_PARADIGM_TYPE, "invalid I/O paradigm passed" );

    io_paradigm_node* child =
        UTILS_Atomic_LoadN_void_ptr( &ioParadigmNode->children[ ioParadigm ],
                                     UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    if ( child == NULL )
    {
        UTILS_MutexLock( &io_paradigm_mutex );
        child = UTILS_Atomic_LoadN_void_ptr( &ioParadigmNode->children[ ioParadigm ],
                                             UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        if ( child == NULL )
        {
            /* not really per-location object, but MISC should survive location death */
            child = SCOREP_Memory_AllocForMisc( sizeof( *child ) );
            memset( child, 0, sizeof( *child ) );

            child->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_READ ] =
                SCOREP_Definitions_NewMetric( SCOREP_IoMgmt_GetParadigmName( ioParadigm ),
                                              "I/O bytes read",
                                              SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                              SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                              SCOREP_METRIC_VALUE_UINT64,
                                              SCOREP_METRIC_BASE_DECIMAL,
                                              0,
                                              "bytes",
                                              SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                              ioParadigmNode->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_READ ] );

            child->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_WRITE ] =
                SCOREP_Definitions_NewMetric( SCOREP_IoMgmt_GetParadigmName( ioParadigm ),
                                              "I/O bytes written",
                                              SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                              SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                              SCOREP_METRIC_VALUE_UINT64,
                                              SCOREP_METRIC_BASE_DECIMAL,
                                              0,
                                              "bytes",
                                              SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                              ioParadigmNode->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_WRITE ] );

            /* announce final node */
            UTILS_Atomic_StoreN_void_ptr( &ioParadigmNode->children[ ioParadigm ],
                                          child,
                                          UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
        }
        UTILS_MutexUnlock( &io_paradigm_mutex );
    }

    return UTILS_Atomic_LoadN_void_ptr( &ioParadigmNode->children[ ioParadigm ],
                                        UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
}

static io_paradigm_node*
trigger_io_operation_recursively( SCOREP_Location*       thread,
                                  SCOREP_IoHandleHandle  handle,
                                  SCOREP_IoOperationMode mode,
                                  uint64_t               bytesResult )
{
    io_paradigm_node* io_paradigm;
    if ( handle == SCOREP_INVALID_IO_HANDLE )
    {
        /* Stop recursion */
        io_paradigm = &io_paradigm_root;
    }
    else
    {
        /* Recurse into parent */
        io_paradigm_node* parent_io_paradigm = trigger_io_operation_recursively(
            thread,
            SCOREP_IoHandleHandle_GetParentHandle( handle ),
            mode, bytesResult );
        io_paradigm = ensure_io_paradigm( parent_io_paradigm, SCOREP_IoHandleHandle_GetIoParadigm( handle ) );
    }

    SCOREP_Profile_TriggerInteger( thread, io_paradigm->io_bytes_metric[ mode ], bytesResult );

    return io_paradigm;
}

void
scorep_profile_io_operation_complete( SCOREP_Location*       thread,
                                      uint64_t               timestamp,
                                      SCOREP_IoHandleHandle  handle,
                                      SCOREP_IoOperationMode mode,
                                      uint64_t               bytesResult,
                                      uint64_t               matchingId )
{
    if ( bytesResult == SCOREP_IO_UNKOWN_TRANSFER_SIZE
         || mode >= SCOREP_IO_OPERATION_MODE_FLUSH )
    {
        return;
    }

    trigger_io_operation_recursively( thread, handle, mode, bytesResult );
}
