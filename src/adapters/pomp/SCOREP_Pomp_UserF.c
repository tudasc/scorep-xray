/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       SCOREP_Pomp_UserF.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 fortran user adapter functions and initialization.
 */

#include <config.h>

#include "SCOREP_Pomp_Fortran.h"
#include "SCOREP_Pomp_Common.h"

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */

void FSUB(POMP2_Begin)( POMP2_Region_handle_fortran* regionHandle,
                        const char*                  ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Begin(SCOREP_POMP_F2C_REGION( regionHandle ), ctc_string );
    }
}

void FSUB(POMP2_End)( POMP2_Region_handle_fortran* regionHandle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_End(SCOREP_POMP_F2C_REGION( regionHandle ));
    }
}
