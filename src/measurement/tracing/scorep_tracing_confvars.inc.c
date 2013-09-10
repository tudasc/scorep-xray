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


bool        scorep_tracing_use_sion;
uint64_t    scorep_tracing_nlocations_per_sion_file;
static bool scorep_tracing_compress;


/** @brief Measurement system configure variables */
static SCOREP_ConfigVariable scorep_tracing_config_variables[] = {
    {
        "use_sion",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_tracing_use_sion,
        NULL,
        "false",
        "Whether or not to use libsion as OTF2 substrate",
        ""
    },
    {
        "nlocations_per_sion_file",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_tracing_nlocations_per_sion_file,
        NULL,
        "1024",
        "Number of locations that share one sion file (must be > 0)",
        ""
    },
    {
        "compress",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_tracing_compress,
        NULL,
        "false",
        "Whether or not to compress traces with libz",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};

SCOREP_ErrorCode
SCOREP_Tracing_Register( void )
{
    SCOREP_ErrorCode ret;
    ret = SCOREP_ConfigRegister( "tracing", scorep_tracing_config_variables );
    if ( SCOREP_SUCCESS != ret )
    {
        UTILS_ERROR( ret, "Can't register tracing config variables" );
    }

    return ret;
}
