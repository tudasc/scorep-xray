/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
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


static bool scorep_tracing_use_sion;
uint64_t    scorep_tracing_max_procs_per_sion_file;
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
        "max_procs_per_sion_file",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_tracing_max_procs_per_sion_file,
        NULL,
        "1024",
        "Maximum number of processes that share one sion file (must be > 0)",
        "All processes are than evenly distributed over the number of needed "
        "files to fulfill this constraint. E.g., having 4 processes and setting "
        "the maximum to 3 would result in 2 files each holding 2 processes."
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
