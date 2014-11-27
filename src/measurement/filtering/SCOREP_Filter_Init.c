/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 * Implements the filter initialization interface for the filtering system.
 */

#include <config.h>

#include <SCOREP_Filter.h>
#include <UTILS_Error.h>

#include <stdlib.h>

/* **************************************************************************************
   Variable and type definitions
****************************************************************************************/

#include "scorep_filter_confvars.inc.c"

/* **************************************************************************************
   Initialization of filtering system
****************************************************************************************/

void
SCOREP_Filter_Initialize( void )
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

    SCOREP_ErrorCode err = SCOREP_Filter_ParseFile( scorep_filter_file_name );
    if ( err != SCOREP_SUCCESS )
    {
        UTILS_ERROR( err,
                     "Error while parsing filter file. Abort." );
        exit( EXIT_FAILURE );
    }
}

void
SCOREP_Filter_Finalize( void )
{
    SCOREP_Filter_FreeRules();
}
