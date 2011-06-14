/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 *  @file       SCOREP_User_Control.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  @brief Contain the implementation of the control functions for C.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include "SCOREP_User_Init.h"
#include "SCOREP_RuntimeManagement.h"

void
SCOREP_User_EnableRecording()
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SCOREP_EnableRecording();
}

void
SCOREP_User_DisableRecording()
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Stop recording */
    SCOREP_DisableRecording();
}

bool
SCOREP_User_RecordingEnabled()
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Return value */
    return SCOREP_RecordingEnabled();
}
