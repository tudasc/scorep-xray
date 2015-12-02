/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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

#include <config.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>


#include "scorep_compiler_confvars.inc.c"


/* Implementation of the compiler adapter initialization/finalization struct */
const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter =
{
    .subsystem_name       = "COMPILER (config variables only)",
    .subsystem_register   = &compiler_subsystem_register,
    .subsystem_deregister = &compiler_subsystem_deregister,
};
