/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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
 *  @file
 *
 *  @brief Contains the implementation of the control functions for C.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include "SCOREP_User_Init.h"
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>

void
SCOREP_User_EnableRecording( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        /* (Re)start recording */
        SCOREP_EnableRecording();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_User_DisableRecording( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        /* Stop recording */
        SCOREP_DisableRecording();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

bool
SCOREP_User_RecordingEnabled( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    bool ret = false;
    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        ret = SCOREP_RecordingEnabled();
    }

    /* Return value */
    SCOREP_IN_MEASUREMENT_DECREMENT();

    return ret;
}
