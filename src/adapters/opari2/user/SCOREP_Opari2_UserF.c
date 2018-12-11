/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Implementation of the OPARI2 fortran user adapter functions.
 */

#include <config.h>

#include <SCOREP_InMeasurement.h>

#include "SCOREP_Opari2_User_Fortran.h"
#include "SCOREP_Opari2_Region_Info.h"

/*
 * Fortran wrappers calling the C versions
 */

void
FSUB( POMP2_Begin )( POMP2_Region_handle_fortran* regionHandle,
                     const char*                  ctc_string,
                     scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Begin( SCOREP_POMP_F2C_REGION( regionHandle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_End )( POMP2_Region_handle_fortran* regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_End( SCOREP_POMP_F2C_REGION( regionHandle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Init )( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Init();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Finalize )( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Finalize();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_On )( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    scorep_opari2_recording_on = 1;
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Off )( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    scorep_opari2_recording_on = 0;
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
