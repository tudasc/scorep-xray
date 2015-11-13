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
 * Copyright (c) 2009-2015,
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
 */


/**
 * @file
 *
 *
 */

#include <config.h>
#include <scorep_clock_synchronization.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Timer_Ticks.h>

/* *INDENT-OFF* */
extern void scorep_interpolate_epoch(uint64_t* epochBegin, uint64_t* epochEnd);
/* *INDENT-ON*  */

void
SCOREP_SynchronizeClocks( void )
{
    // We assume that all cores use the same clock.
    SCOREP_AddClockOffset( SCOREP_Timer_GetClockTicks(), 0, 0 );
}


void
SCOREP_GetGlobalEpoch( uint64_t* globalEpochBegin, uint64_t* globalEpochEnd )
{
    scorep_interpolate_epoch( globalEpochBegin, globalEpochEnd );
}
