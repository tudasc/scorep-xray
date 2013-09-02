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
 * @file       src/measurement/paradigm/scorep_unify_mpp_mockup.c
 *
 *
 */


#include <config.h>
#include <stdlib.h>
#include <scorep_unify.h>
#include <UTILS_Debug.h>
#include <UTILS_Error.h>


void
SCOREP_Unify_Mpp( void )
{
    UTILS_FATAL( "SCOREP_Unify_Mpp() called in non-mpi build" );
}
