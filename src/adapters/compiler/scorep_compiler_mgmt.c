/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2021-2024,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 */

#include <config.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_RuntimeManagement.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>


#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_PLUGIN )
    #if HAVE( SCOREP_COMPILER_INSTRUMENTATION_XRAY_PLUGIN )
        #include "xray-plugin/scorep_xray_plugin.h"
    #endif
    #include "scorep_compiler_mgmt_plugin.inc.c"
#endif /* SCOREP_COMPILER_INSTRUMENTATION_PLUGIN */

#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_NEEDS_ADDR2LINE )
#include "scorep_compiler_mgmt_func_addr_hash.inc.c"
#endif /* HAVE( SCOREP_COMPILER_INSTRUMENTATION_NEEDS_ADDR2LINE ) */


static SCOREP_ErrorCode
compiler_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();
#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_PLUGIN )
    #if HAVE( SCOREP_COMPILER_INSTRUMENTATION_XRAY_PLUGIN )
    // XRay plugin creates regions at runtime, registers them itseld and does not use begin and end object files,
    // therefore don't use default registration mechanism
        initXRayPlugin();
    #else
        plugin_register_regions();
#endif
#endif /* SCOREP_COMPILER_INSTRUMENTATION_PLUGIN */

#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_NEEDS_ADDR2LINE )
    func_addr_hash_register_obj_close_cb();
#endif /* HAVE( SCOREP_COMPILER_INSTRUMENTATION_NEEDS_ADDR2LINE ) */

    UTILS_DEBUG_EXIT();
    return SCOREP_SUCCESS;
}

static void compiler_subsystem_finalize( void ){
#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_XRAY_PLUGIN )
    UTILS_DEBUG_ENTRY();
    finalizeXRayPlugin(1);
    UTILS_DEBUG_EXIT();
#endif
}


/* Implementation of the compiler adapter initialization/finalization struct */
const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter =
{
    .subsystem_name = "COMPILER",
    .subsystem_init = &compiler_subsystem_init,
    .subsystem_finalize = &compiler_subsystem_finalize,
};
