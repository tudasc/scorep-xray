/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 * @brief Memory adapter interface support to the measurement system.
 *
 * This file contains memory adapter initialization and finalization functions.
 */

#include <config.h>

#include "scorep_memory_mgmt.h"
#include "scorep_memory_attributes.h"

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Location.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_AllocMetric.h>

#define SCOREP_DEBUG_MODULE_NAME MEMORY
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "scorep_memory_confvars.inc.c"


/* *INDENT-OFF* */
static void register_memory_regions( void );
/* *INDENT-ON* */

static size_t memory_subsystem_id;

SCOREP_AllocMetric* scorep_memory_metric = NULL;


static SCOREP_ErrorCode
memory_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG( "Register environment variables" );

    memory_subsystem_id = subsystemId;

    return SCOREP_ConfigRegister( "memory", scorep_memory_confvars );
}


static SCOREP_ErrorCode
memory_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    if ( scorep_memory_recording || SCOREP_IsUnwindingEnabled() )
    {
        register_memory_regions();
    }

    if ( scorep_memory_recording )
    {
        SCOREP_AllocMetric_New( "Process memory usage", &scorep_memory_metric );
        scorep_memory_attributes_init();
    }

    UTILS_DEBUG_EXIT();
    return SCOREP_SUCCESS;
}


static void
memory_subsystem_end( void )
{
    UTILS_DEBUG_ENTRY();

    if ( scorep_memory_recording )
    {
        SCOREP_AllocMetric_ReportLeaked( scorep_memory_metric );
    }

    UTILS_DEBUG_EXIT();
}

static void
memory_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    if ( scorep_memory_recording )
    {
        SCOREP_AllocMetric_Destroy( scorep_memory_metric );
    }

    UTILS_DEBUG_EXIT();
}


/* this symbol is used in the instrumentation checker */
SCOREP_RegionHandle scorep_memory_regions[ SCOREP_MEMORY_REGION_SENTINEL ];

static void
register_memory_regions( void )
{
#define SCOREP_MEMORY_REGION( NAME, TYPE, name ) \
    scorep_memory_regions[ SCOREP_MEMORY_ ## NAME ] = \
        SCOREP_Definitions_NewRegion( name, \
                                      NULL, \
                                      SCOREP_Definitions_NewSourceFile( "MEMORY" ), \
                                      SCOREP_INVALID_LINE_NO, \
                                      SCOREP_INVALID_LINE_NO, \
                                      SCOREP_PARADIGM_MEMORY, \
                                      SCOREP_REGION_ ## TYPE );
    SCOREP_MEMORY_REGIONS
}


/* Implementation of the memory adapter initialization/finalization struct */
const SCOREP_Subsystem SCOREP_Subsystem_MemoryAdapter =
{
    .subsystem_name     = "MEMORY",
    .subsystem_register = &memory_subsystem_register,
    .subsystem_init     = &memory_subsystem_init,
    .subsystem_end      = &memory_subsystem_end,
    .subsystem_finalize = &memory_subsystem_finalize
};
