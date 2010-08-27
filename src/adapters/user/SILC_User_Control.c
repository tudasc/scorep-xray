/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 *  @file       SILC_User_Control.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  @brief Contain the implementation of the control functions for C.
 */

#include <config.h>
#include "SILC_User_Functions.h"
#include "SILC_User_Init.h"
#include "SILC_RuntimeManagement.h"

void
SILC_User_EnableRecording()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SILC_EnableRecording();
}

void
SILC_User_DisableRecording()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* Stop recording */
    SILC_DisableRecording();
}

bool
SILC_User_RecordingEnabled()
{
    /* Assert that the adapter and management system are initialized */
    SILC_USER_ASSERT_INITIALIZED;

    /* Return value */
    return SILC_RecordingEnabled();
}
