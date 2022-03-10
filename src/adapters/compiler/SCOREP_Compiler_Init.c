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
 * Copyright (c) 2009-2015, 2021-2022,
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
 *
 * @brief Compiler adapter interface support to the measurement system.
 *
 * This file contains compiler adapter initialization and finalization functions
 * which are common for all compiler adapters.
 */

#include <config.h>

#include <SCOREP_Subsystem.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

/**
   The adapter initialize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SCOREP_ErrorCode
scorep_compiler_subsystem_init( void );


/* Implementation of the compiler adapter initialization/finalization struct */
const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter =
{
    .subsystem_name = "COMPILER",
    .subsystem_init = &scorep_compiler_subsystem_init,
};
