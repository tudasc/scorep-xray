/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * power_realtime timer.
 */


#include <config.h>
#include "SCOREP_Timing.h"

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
