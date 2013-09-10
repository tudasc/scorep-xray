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
 * Copyright (c) 2009-2013,
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
#include "SCOREP_RuntimeManagement.h"

void
SCOREP_User_EnableRecording( void )
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SCOREP_EnableRecording();
}

void
SCOREP_User_DisableRecording( void )
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Stop recording */
    SCOREP_DisableRecording();
}

bool
SCOREP_User_RecordingEnabled( void )
{
    /* Assert that the adapter and management system are initialized */
    if ( scorep_user_is_initialized != 1 )
    {
        return false;
    }

    /* Return value */
    return SCOREP_RecordingEnabled();
}
