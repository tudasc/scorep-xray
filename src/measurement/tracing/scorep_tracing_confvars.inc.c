/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
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
#include <UTILS_Error.h>

#include <stdbool.h>
#include <stdint.h>

bool     scorep_tracing_use_sion;
uint64_t scorep_tracing_max_procs_per_sion_file;
/* Need to initialize variable, as it is not guaranteed that it will be set by
 * the config system, if unwinding is not supported.
 */
bool scorep_tracing_convert_calling_context = false;


/** @brief Measurement system configure variables */
static const SCOREP_ConfigVariable scorep_tracing_confvars[] = {
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
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_tracing_max_procs_per_sion_file,
        NULL,
        "1K",
        "Maximum number of processes that share one sion file (must be >\2400)",
        "All processes are than evenly distributed over the number of needed "
        "files to fulfill this constraint. E.g., having 4 processes and setting "
        "the maximum to 3 would result in 2 files each holding 2 processes."
    },
    SCOREP_CONFIG_TERMINATOR
};

static const SCOREP_ConfigVariable scorep_tracing_calling_context_confvars[] = {
    {
        "convert_calling_context_events",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_tracing_convert_calling_context,
        NULL,
        "false",
        "Write calling context information as a sequence of Enter/Leave events to trace",
        "When recording the calling context of events (instrumented or sampled) "
        "than these could be stored in the trace either as the new CallingContext "
        "records from OTF2 or they could be converted to the legacy Enter/Leave "
        "records.  This can be controlled with this variable, where the former is "
        "the false value.\n"
        "This is only in effect if SCOREP_ENABLING_UNWINDING is on.\n"
        "Note that enabling this will result in an increase of records per "
        "event and also of the loss of the source code locations.\n"
        "This option exists only for backwards compatibility for tools, which "
        "cannot handle the new OTF2 records. This option my thus be removed in "
        "future releases."
    },
    SCOREP_CONFIG_TERMINATOR
};

SCOREP_ErrorCode
SCOREP_Tracing_Register( void )
{
    SCOREP_ErrorCode ret;
    ret = SCOREP_ConfigRegister( "tracing", scorep_tracing_confvars );
    if ( SCOREP_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Can't register tracing config variables" );
    }

    return SCOREP_ConfigRegisterCond( "tracing",
                                      scorep_tracing_calling_context_confvars,
                                      HAVE_BACKEND_UNWINDING_SUPPORT );
}
