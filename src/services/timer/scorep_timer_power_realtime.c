/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file       scorep_timer_power_realtime.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * power_realtime timer.
 */


#include <config.h>
#include "SCOREP_Timing.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/systemcfg.h>

void
SCOREP_Timer_Initialize( void )
{
    return;
}


uint64_t
SCOREP_GetClockTicks( void )
{
    timebasestruct_t t;
    read_real_time( &t, TIMEBASE_SZ );
    time_base_to_time( &t, TIMEBASE_SZ );
    return ( uint64_t )t.tb_high * UINT64_C( 1000000000 ) + ( uint64_t )t.tb_low;
}


uint64_t
SCOREP_GetClockResolution( void )
{
    return UINT64_C( 1000000000 );
}


bool
SCOREP_ClockIsGlobal( void )
{
    return false;
}
