/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       src/measurement/paradigm/scorep_clock_synchronization_mpp_disabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <scorep_clock_synchronization.h>

#include <definitions/SCOREP_Definitions.h>
#include <SCOREP_Timing.h>


/* *INDENT-OFF* */
extern void scorep_interpolate_epoch(uint64_t* epochBegin, uint64_t* epochEnd);
/* *INDENT-ON*  */

void
SCOREP_SynchronizeClocks()
{
    // We assume that all cores use the same clock.
    SCOREP_AddClockOffset( SCOREP_GetClockTicks(), 0, 0 );
}


void
SCOREP_GetGlobalEpoch( uint64_t* globalEpochBegin, uint64_t* globalEpochEnd )
{
    scorep_interpolate_epoch( globalEpochBegin, globalEpochEnd );
}
