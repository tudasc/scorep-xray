/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 *  @file SCOREP_User_Control.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *  @ingroup    SCOREP_User_External
 *
 *  @brief Contain the implementation of the control functions for Fortran.
 */

#include <config.h>
#include "SCOREP_User_Init.h"
#include "SCOREP_RuntimeManagement.h"
#include "SCOREP_Fortran_Wrapper.h"

#define SCOREP_User_EnableRecordingF_U  SCOREP_USER_ENABLERECORDINGF
#define SCOREP_User_DisableRecordingF_U SCOREP_USER_DISABLERECORDINGF
#define SCOREP_User_RecordingEnabledF_U SCOREP_USER_RECORDINGENABLEDF
#define SCOREP_User_EnableRecordingF_L  scorep_user_enablerecordingf
#define SCOREP_User_DisableRecordingF_L scorep_user_disablerecordingf
#define SCOREP_User_RecordingEnabledF_L scorep_user_recordingenabledf

void
FSUB( SCOREP_User_EnableRecordingF )()
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SCOREP_EnableRecording();
}

void
FSUB( SCOREP_User_DisableRecordingF )()
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Stop recording */
    SCOREP_DisableRecording();
}

void
FSUB( SCOREP_User_RecordingEnabledF )( int* enabled )
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Return value */
    *enabled = ( int )SCOREP_RecordingEnabled();
}
