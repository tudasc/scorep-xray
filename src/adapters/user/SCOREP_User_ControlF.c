/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#define SCOREP_F_EnableRecording_U  SCOREP_F_ENABLERECORDING
#define SCOREP_F_DisableRecording_U SCOREP_F_DISABLERECORDING
#define SCOREP_F_RecordingEnabled_U SCOREP_F_RECORDINGENABLED
#define SCOREP_F_EnableRecording_L  scorep_f_enablerecording
#define SCOREP_F_DisableRecording_L scorep_f_disablerecording
#define SCOREP_F_RecordingEnabled_L scorep_f_recordingenabled

void
FSUB( SCOREP_F_EnableRecording )()
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SCOREP_EnableRecording();
}

void
FSUB( SCOREP_F_DisableRecording )()
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Stop recording */
    SCOREP_DisableRecording();
}

void
FSUB( SCOREP_F_RecordingEnabled )( int* enabled )
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Return value */
    *enabled = ( int )SCOREP_RecordingEnabled();
}
