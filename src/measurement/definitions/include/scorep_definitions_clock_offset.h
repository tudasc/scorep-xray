/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_CLOCK_OFFSET_H
#define SCOREP_PRIVATE_DEFINITIONS_CLOCK_OFFSET_H


/**
 * @file
 *
 *
 */


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
