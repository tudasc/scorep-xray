/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 *
 */

#include <config.h>
#include "scorep_clock_synchronization.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_Timer_Ticks.h>

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
SCOREP_BeginEpoch( void )
{
    assert( !scorep_epoch_begin_set );
    scorep_epoch_begin     = SCOREP_Timer_GetClockTicks();
    scorep_epoch_begin_set = true;
}


uint64_t
SCOREP_GetBeginEpoch( void )
{
    assert( scorep_epoch_begin_set );
    return scorep_epoch_begin;
}


void
SCOREP_EndEpoch( void )
{
    assert( scorep_epoch_begin_set );
    assert( !scorep_epoch_end_set );
    scorep_epoch_end = SCOREP_Timer_GetClockTicks();
    assert( scorep_epoch_end > scorep_epoch_begin );
    scorep_epoch_end_set = true;
}


uint64_t
SCOREP_GetEndEpoch( void )
{
    assert( scorep_epoch_end_set );
    return scorep_epoch_end;
}


void
scorep_interpolate_epoch( uint64_t* epochBegin, uint64_t* epochEnd )
{
    // transform "worker" scorep_epoch_(begin|end) to "master" time.

    if ( scorep_epoch_interpolated )
    {
        *epochBegin = scorep_epoch_begin;
        *epochEnd   = scorep_epoch_end;
        return;
    }

    assert( scorep_epoch_begin_set );
    assert( scorep_epoch_end_set );

    {
        uint64_t timestamp1, timestamp2;
        int64_t  offset1, offset2;
        SCOREP_GetFirstClockSyncPair( &offset1, &timestamp1, &offset2, &timestamp2 );
        scorep_epoch_begin = scorep_interpolate( scorep_epoch_begin, offset1, timestamp1, offset2, timestamp2 );
    }

    {
        uint64_t timestamp1, timestamp2;
        int64_t  offset1, offset2;
        SCOREP_GetLastClockSyncPair( &offset1, &timestamp1, &offset2, &timestamp2 );
        scorep_epoch_end = scorep_interpolate( scorep_epoch_end, offset1, timestamp1, offset2, timestamp2 );
    }

    assert( scorep_epoch_end > scorep_epoch_begin );

    *epochBegin = scorep_epoch_begin;
    *epochEnd   = scorep_epoch_end;

    scorep_epoch_interpolated = true;
}


static uint64_t
scorep_interpolate( uint64_t workerTime, int64_t offset1, uint64_t workerTime1, int64_t offset2, uint64_t workerTime2 )
{
    // Without the casts we get non-deterministic results from time to time.
    // There might be a better way to do it though.
    double interpolated_time = workerTime + ( offset2 - offset1 ) / ( double )( workerTime2 - workerTime1 ) * ( ( double )workerTime - workerTime1 ) + offset1;
    assert( interpolated_time > 0 );
    return interpolated_time;
}
