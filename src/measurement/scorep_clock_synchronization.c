/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
 * @file       src/measurement/scorep_clock_synchronization.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "scorep_clock_synchronization.h"

#include "scorep_definitions.h"
#include <SCOREP_Timing.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* *INDENT-OFF* */
static uint64_t scorep_interpolate(uint64_t workerTime, int64_t offset1, uint64_t workerTime1, int64_t offset2, uint64_t workerTime2);
/* *INDENT-ON*  */


static uint64_t scorep_epoch_begin = 0;
static uint64_t scorep_epoch_end          = 0;
static bool     scorep_epoch_begin_set    = false;
static bool     scorep_epoch_end_set      = false;
static bool     scorep_epoch_interpolated = false;

void
SCOREP_BeginEpoch()
{
    assert( !scorep_epoch_begin_set );
    scorep_epoch_begin     = SCOREP_GetClockTicks();
    scorep_epoch_begin_set = true;
}


void
SCOREP_EndEpoch()
{
    assert( !scorep_epoch_end );
    scorep_epoch_end     = SCOREP_GetClockTicks();
    scorep_epoch_end_set = true;
}


void
scorep_interpolate_epoch( uint64_t* epochBegin, uint64_t* epochEnd )
{
    if ( scorep_epoch_interpolated )
    {
        *epochBegin = scorep_epoch_begin;
        *epochEnd   = scorep_epoch_end;
        return;
    }

    // transform "worker" scorep_epoch_(begin|end) to "master" time.
    assert( scorep_epoch_begin_set );
    assert( scorep_epoch_end_set );
    uint64_t timestamp1 = 1; // dummy initialization to prevent devision by 0
    uint64_t timestamp2 = 2;
    int64_t  offset1, offset2;

    SCOREP_GetFirstClockSyncPair( &offset1, &timestamp1, &offset2, &timestamp2 );
    scorep_epoch_begin = scorep_interpolate( scorep_epoch_begin, offset1, timestamp1, offset2, timestamp2 );

    SCOREP_GetLastClockSyncPair( &offset1, &timestamp1, &offset2, &timestamp2 );
    scorep_epoch_end = scorep_interpolate( scorep_epoch_end, offset1, timestamp1, offset2, timestamp2 );

    assert( scorep_epoch_end > scorep_epoch_begin );

    *epochBegin = scorep_epoch_begin;
    *epochEnd   = scorep_epoch_end;

    scorep_epoch_interpolated = true;
}


static uint64_t
scorep_interpolate( uint64_t workerTime, int64_t offset1, uint64_t workerTime1, int64_t offset2, uint64_t workerTime2 )
{
    return workerTime + ( offset2 - offset1 ) / ( workerTime2 - workerTime1 ) * ( workerTime - workerTime1 ) + offset1;
}
