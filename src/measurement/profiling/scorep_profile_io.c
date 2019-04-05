/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016, 2019,
 * Technische Universitaet Dresden, Germany
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
#include <SCOREP_Mutex.h>

struct scorep_profile_io_paradigm
{
    SCOREP_IoParadigmType              io_paradigm;
    /** Parent of this I/O paradigm */
    struct scorep_profile_io_paradigm* parent;
    /** The I/O bytes read metric for this I/O paradigm level,
     *  indexed with SCOREP_IoOperationMode
     */
    SCOREP_MetricHandle                io_bytes_metric[ 2 ];
    /** Possible children of this paradigm */
    struct scorep_profile_io_paradigm* children[ SCOREP_INVALID_IO_PARADIGM_TYPE ];
};

static struct scorep_profile_io_paradigm io_paradigm_root;
static SCOREP_Mutex                      io_paradigm_mutex = SCOREP_INVALID_MUTEX;

/* *******************************************************************************
 * External interface
 * ******************************************************************************/

void
scorep_profile_io_init( void )
{
    SCOREP_MutexCreate( &io_paradigm_mutex );

    io_paradigm_root.io_paradigm                                      = SCOREP_INVALID_IO_PARADIGM_TYPE;
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

void
scorep_profile_io_finalize( void )
{
    SCOREP_MutexDestroy( &io_paradigm_mutex );
}

void
scorep_profile_io_init_location( SCOREP_Profile_LocationData* location )
{
    location->current_io_paradigm = &io_paradigm_root;
}

void
scorep_profile_io_paradigm_enter( SCOREP_Location*      thread,
                                  SCOREP_IoParadigmType ioParadigm )
{
    UTILS_BUG_ON( ioParadigm >= SCOREP_INVALID_IO_PARADIGM_TYPE, "invalid I/O paradigm passed" );

    SCOREP_Profile_LocationData*       location    = scorep_profile_get_profile_data( thread );
    struct scorep_profile_io_paradigm* io_paradigm = location->current_io_paradigm;

    if ( io_paradigm->children[ ioParadigm ] == NULL )
    {
        SCOREP_MutexLock( io_paradigm_mutex );
        if ( io_paradigm->children[ ioParadigm ] == NULL )
        {
            /* not really per-location object, but MISC should survive location death */
            io_paradigm->children[ ioParadigm ] =
                SCOREP_Memory_AllocForMisc( sizeof( *io_paradigm ) );
            memset( io_paradigm->children[ ioParadigm ], 0, sizeof( *io_paradigm ) );

            io_paradigm->children[ ioParadigm ]->io_paradigm = ioParadigm;
            io_paradigm->children[ ioParadigm ]->parent      = io_paradigm;

            io_paradigm->children[ ioParadigm ]->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_READ ] =
                SCOREP_Definitions_NewMetric( SCOREP_IoMgmt_GetParadigmName( ioParadigm ),
                                              "I/O bytes read",
                                              SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                              SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                              SCOREP_METRIC_VALUE_UINT64,
                                              SCOREP_METRIC_BASE_DECIMAL,
                                              0,
                                              "bytes",
                                              SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                              io_paradigm->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_READ ] );

            io_paradigm->children[ ioParadigm ]->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_WRITE ] =
                SCOREP_Definitions_NewMetric( SCOREP_IoMgmt_GetParadigmName( ioParadigm ),
                                              "I/O bytes written",
                                              SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                              SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                              SCOREP_METRIC_VALUE_UINT64,
                                              SCOREP_METRIC_BASE_DECIMAL,
                                              0,
                                              "bytes",
                                              SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                              io_paradigm->io_bytes_metric[ SCOREP_IO_OPERATION_MODE_WRITE ] );
        }
        SCOREP_MutexUnlock( io_paradigm_mutex );
    }

    location->current_io_paradigm = io_paradigm->children[ ioParadigm ];
}

void
scorep_profile_io_paradigm_leave( SCOREP_Location*      thread,
                                  SCOREP_IoParadigmType ioParadigm )
{
    UTILS_BUG_ON( ioParadigm >= SCOREP_INVALID_IO_PARADIGM_TYPE, "invalid I/O paradigm passed" );

    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );
    if ( location->current_io_paradigm->io_paradigm != ioParadigm )
    {
        UTILS_WARNING( "leaving the wrong I/O context" );
        return;
    }

    location->current_io_paradigm = location->current_io_paradigm->parent;
}

void
scorep_profile_io_operation_complete( SCOREP_Location*       thread,
                                      uint64_t               timestamp,
                                      SCOREP_IoHandleHandle  handle,
                                      SCOREP_IoOperationMode mode,
                                      uint64_t               bytesResult,
                                      uint64_t               matchingId )
{
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );
    scorep_profile_node*         node     = scorep_profile_get_current_node( location );
    if ( location->current_io_paradigm->io_paradigm == SCOREP_INVALID_IO_PARADIGM_TYPE )
    {
        UTILS_WARNING( "completed I/O operation outside I/O paradigm context" );
        return;
    }

    if ( bytesResult == SCOREP_IO_UNKOWN_TRANSFER_SIZE
         || mode > SCOREP_IO_OPERATION_MODE_WRITE )
    {
        return;
    }

    struct scorep_profile_io_paradigm* io_paradigm = location->current_io_paradigm;
    while ( io_paradigm )
    {
        SCOREP_Profile_TriggerInteger( thread, io_paradigm->io_bytes_metric[ mode ], bytesResult );
        io_paradigm = io_paradigm->parent;
    }
}
