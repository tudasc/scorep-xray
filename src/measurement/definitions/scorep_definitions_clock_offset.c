/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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
 *
 */


/**
 * @file
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_ClockOffset*  clock_offset_head = NULL;
static SCOREP_ClockOffset** clock_offset_tail = &clock_offset_head;


/**
 * Add a clock sync point into the local definitions
 */
void
SCOREP_AddClockOffset( uint64_t time,
                       int64_t  offset,
                       double   stddev )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_AnyHandle new_handle =
        SCOREP_Memory_AllocForDefinitions( NULL, sizeof( SCOREP_ClockOffset ) );
    SCOREP_ClockOffset* new_clock_offset =
        SCOREP_MEMORY_DEREF_LOCAL( new_handle, SCOREP_ClockOffset* );
    memset( new_clock_offset, 0, sizeof( *new_clock_offset ) );
    new_clock_offset->next = NULL;

    new_clock_offset->time   = time;
    new_clock_offset->offset = offset;
    new_clock_offset->stddev = stddev;

    *clock_offset_tail = new_clock_offset;
    clock_offset_tail  = &new_clock_offset->next;

    SCOREP_Definitions_Unlock();
}


void
SCOREP_GetFirstClockSyncPair( int64_t*  offset1,
                              uint64_t* timestamp1,
                              int64_t*  offset2,
                              uint64_t* timestamp2 )
{
    UTILS_BUG_ON( clock_offset_head == NULL || clock_offset_head->next == NULL,
                  "Requesting the first clock sync pair without having at least 2 offsets" );

    *offset1    = clock_offset_head->offset;
    *timestamp1 = clock_offset_head->time;
    *offset2    = clock_offset_head->next->offset;
    *timestamp2 = clock_offset_head->next->time;

    UTILS_BUG_ON( *timestamp1 >= *timestamp2,
                  "Out of order clock sync pairs: %" PRIu64 " >= %" PRIu64,
                  *timestamp1, *timestamp2 );
}


void
SCOREP_GetLastClockSyncPair( int64_t*  offset1,
                             uint64_t* timestamp1,
                             int64_t*  offset2,
                             uint64_t* timestamp2 )
{
    UTILS_BUG_ON( clock_offset_head == NULL || clock_offset_head->next == NULL,
                  "Requesting the last clock sync pair without having at least 2 offsets" );

    SCOREP_ClockOffset* previous = clock_offset_head;
    SCOREP_ClockOffset* current  = previous->next;

    while ( current->next )
    {
        previous = current;
        current  = current->next;
    }

    *offset1    = previous->offset;
    *timestamp1 = previous->time;
    *offset2    = current->offset;
    *timestamp2 = current->time;

    UTILS_BUG_ON( *timestamp1 >= *timestamp2,
                  "Out of order clock sync pairs: %" PRIu64 " >= %" PRIu64,
                  *timestamp1, *timestamp2 );
}

void
SCOREP_ForAllClockOffsets( void ( * cb )( SCOREP_ClockOffset*,
                                          void* ),
                           void*    userData )
{
    for ( SCOREP_ClockOffset* clock_offset = clock_offset_head;
          clock_offset;
          clock_offset = clock_offset->next )
    {
        cb( clock_offset, userData );
    }
}
