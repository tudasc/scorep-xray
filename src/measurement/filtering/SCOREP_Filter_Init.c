/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file       SCOREP_Filter_Init.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * Implements the filter initialization interface for the filtering system.
 */

#include <config.h>
#include <SCOREP_Config.h>
#include <SCOREP_Filter.h>
#include <UTILS_Debug.h>

/* **************************************************************************************
   Variable and type definitions
****************************************************************************************/

#include "scorep_filter_confvars.inc.c"

/* **************************************************************************************
   Initialization of filtering system
****************************************************************************************/

void
SCOREP_Filter_Initialize()
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                        "Initialize selective tracing" );

    /* Check whether a configuraion file name was specified */
    if ( scorep_filter_file_name == NULL || *scorep_filter_file_name == '\0' )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                            "No configuration file for filtering specified.\n"
                            "Disable filtering." );
        return;
    }

    /* Parse configuration file */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                        "Reading filter configuration file %s.",
                        scorep_filter_file_name );

    SCOREP_Error_Code err = SCOREP_Filter_ParseFile( scorep_filter_file_name );
    if ( err != SCOREP_SUCCESS )
    {
        UTILS_ERROR( err,
                     "Error while parsing filter file.\n"
                     "Disable filtering." );
        return;
    }
}

void
SCOREP_Filter_Finalize()
{
    SCOREP_Filter_FreeRules();
}
