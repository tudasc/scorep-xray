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
 * @file
 *
 *
 */


/**
 * Contains the file name of the filter file. The value is set in the configuration
 * system through the variable SCOREP_FILTERING_FILE
 */
static char* scorep_filter_file_name;

/**
   Array of configuration variables for filtering tracing.
 */
SCOREP_ConfigVariable scorep_filter_configs[] = {
    {
        "file",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_filter_file_name,
        NULL,
        "",
        "A file name which contain the filter rules",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};

SCOREP_ErrorCode
SCOREP_Filter_Register( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                        "Register config variables for filtering system" );
    return SCOREP_ConfigRegister( "filtering", scorep_filter_configs );
}
