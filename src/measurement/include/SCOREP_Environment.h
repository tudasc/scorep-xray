/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
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


#ifndef SCOREP_INTERNAL_ENVIRONMENT_H
#define SCOREP_INTERNAL_ENVIRONMENT_H


#include <stdbool.h>
#include <stdint.h>

UTILS_BEGIN_C_DECLS


void
SCOREP_RegisterAllConfigVariables( void );


//bool
//SCOREP_Env_CoreEnvironmentVariablesInitialized( void );


/*
 * General measurement setup
 */

bool
SCOREP_Env_RunVerbose( void );

uint64_t
SCOREP_Env_GetTotalMemory( void );

uint64_t
SCOREP_Env_GetPageSize( void );

const char*
SCOREP_Env_GetExperimentDirectory( void );

bool
SCOREP_Env_DoOverwriteExperimentDirectory( void );

const char*
SCOREP_Env_GetMachineName( void );

const char*
SCOREP_Env_GetExecutable( void );

bool
SCOREP_Env_UseSystemTreeSequence( void );

/*
 * Tracing setup
 */

bool
SCOREP_Env_DoTracing( void );


/*
 * Profiling setup
 */

bool
SCOREP_Env_DoProfiling( void );


/*
 * Calling context setup
 */

bool
SCOREP_Env_DoUnwinding( void );

/*
 * Enforce creation of config files
 */
bool
SCOREP_Env_DoForceCfgFiles( void );

UTILS_END_C_DECLS


#endif /* SCOREP_INTERNAL_ENVIRONMENT_H */
