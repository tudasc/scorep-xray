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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_CLOCK_OFFSET_H
#define SCOREP_PRIVATE_DEFINITIONS_CLOCK_OFFSET_H


/**
 * @file       src/measurement/definitions/scorep_definitions_clock_offset.h
 *
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


/* Not really a definition, but we need to store them somewhere */
typedef struct SCOREP_ClockOffset SCOREP_ClockOffset;
struct SCOREP_ClockOffset
{
    SCOREP_ClockOffset* next;

    uint64_t            time;
    int64_t             offset;
    double              stddev;
};


void
SCOREP_AddClockOffset( uint64_t time,
                       int64_t  offset,
                       double   stddev );


void
SCOREP_GetFirstClockSyncPair( int64_t*  offset1,
                              uint64_t* timestamp1,
                              int64_t*  offset2,
                              uint64_t* timestamp2 );


void
SCOREP_GetLastClockSyncPair( int64_t*  offset1,
                             uint64_t* timestamp1,
                             int64_t*  offset2,
                             uint64_t* timestamp2 );

/** Call cb for each clock offset */
void
SCOREP_ForAllClockOffsets( void ( * cb )( SCOREP_ClockOffset*,
                                          void* ),
                           void* userData );


#endif /* SCOREP_PRIVATE_DEFINITIONS_CLOCK_OFFSET_H */
