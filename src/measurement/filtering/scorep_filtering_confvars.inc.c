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
 */

#include <SCOREP_Config.h>
#include <UTILS_Debug.h>

/**
 * Contains the file name of the filter file. The value is set in the configuration
 * system through the variable SCOREP_FILTERING_FILE
 */
static char* scorep_filtering_file_name;

/**
   Array of configuration variables for filtering tracing.
 */
static const SCOREP_ConfigVariable scorep_filtering_confvars[] = {
    {
        "file",
        SCOREP_CONFIG_TYPE_PATH,
        &scorep_filtering_file_name,
        "scorep.filter",
        "",
        "A file name which contain the filter rules",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};

SCOREP_ErrorCode
SCOREP_Filtering_Register( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                        "Register config variables for filtering system" );
    return SCOREP_ConfigRegister( "filtering", scorep_filtering_confvars );
}
