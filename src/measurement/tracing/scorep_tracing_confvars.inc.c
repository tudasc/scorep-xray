/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
