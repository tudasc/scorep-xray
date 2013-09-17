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
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
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
FSUB( SCOREP_F_EnableRecording )( void )
{
    /* Assert that the adapter and management system are initialized */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* (Re)start recording */
    SCOREP_EnableRecording();
}

void
FSUB( SCOREP_F_DisableRecording )( void )
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
