/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 fortran user adapter functions.
 */

#include <config.h>

#include "SCOREP_Pomp_Fortran.h"
#include "SCOREP_Pomp_Common.h"

/*
 * Fortran wrappers calling the C versions
 */

void
FSUB( POMP2_Begin )( POMP2_Region_handle_fortran* regionHandle,
                     const char*                  ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Begin( SCOREP_POMP_F2C_REGION( regionHandle ), ctc_string );
    }
}

void
FSUB( POMP2_End )( POMP2_Region_handle_fortran* regionHandle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_End( SCOREP_POMP_F2C_REGION( regionHandle ) );
    }
}

void
FSUB( POMP2_Init )( void )
{
    POMP2_Init();
}

void
FSUB( POMP2_Finalize )( void )
{
    POMP2_Finalize();
}

void
FSUB( POMP2_On )( void )
{
    scorep_pomp_is_tracing_on = 1;
}

void
FSUB( POMP2_Off )( void )
{
    scorep_pomp_is_tracing_on = 0;
}
