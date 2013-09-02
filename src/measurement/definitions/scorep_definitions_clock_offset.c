/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       src/measurement/definitions/scorep_definitions_clock_offset.c
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <definitions/SCOREP_Definitions.h>


#include <assert.h>
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
#include <scorep_types.h>
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
    assert( clock_offset_head );
    assert( clock_offset_head->next );
    *offset1    = clock_offset_head->offset;
    *timestamp1 = clock_offset_head->time;
    *offset2    = clock_offset_head->next->offset;
    *timestamp2 = clock_offset_head->next->time;
    assert( *timestamp2 > *timestamp1 );
}


void
SCOREP_GetLastClockSyncPair( int64_t*  offset1,
                             uint64_t* timestamp1,
                             int64_t*  offset2,
                             uint64_t* timestamp2 )
{
    assert( clock_offset_head );
    assert( clock_offset_head->next );
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
    assert( *timestamp2 > *timestamp1 );
}

void
SCOREP_ForAllClockOffsets( void  ( * cb )( SCOREP_ClockOffset*,
                                           void* ),
                           void* userData )
{
    for ( SCOREP_ClockOffset* clock_offset = clock_offset_head;
          clock_offset;
          clock_offset = clock_offset->next )
    {
        cb( clock_offset, userData );
    }
}
