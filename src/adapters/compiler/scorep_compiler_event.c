/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>
#include <UTILS_Mutex.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Filtering.h>

/* Compile as many APIs as possible. */

#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_NEEDS_ADDR2LINE )
#define SCOREP_COMPILER_CYG_PROFILE_FUNC_ENTER __cyg_profile_func_enter
#define SCOREP_COMPILER_CYG_PROFILE_FUNC_EXIT  __cyg_profile_func_exit
#include "scorep_compiler_event_cyg_profile_func.inc.c"

#define SCOREP_COMPILER_CYG_PROFILE_FUNC_ENTER __pat_tp_func_entry
#define SCOREP_COMPILER_CYG_PROFILE_FUNC_EXIT  __pat_tp_func_return
#include "scorep_compiler_event_cyg_profile_func.inc.c"
#endif /* SCOREP_COMPILER_INSTRUMENTATION_NEEDS_ADDR2LINE */

#include "scorep_compiler_event_func_trace.inc.c"

#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_GCC_PLUGIN )
#include "scorep_compiler_event_gcc_plugin.inc.c"
#endif /* SCOREP_COMPILER_INSTRUMENTATION_GCC_PLUGIN  */

#include "scorep_compiler_event_vt_intel.inc.c"

// TODO extract filtering commonalities?
