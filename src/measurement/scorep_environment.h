/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       src/measurement/scorep_environment.h
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


bool
SCOREP_Env_RunVerbose( void );


bool
SCOREP_Env_DoTracing( void );


bool
SCOREP_Env_DoProfiling( void );

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

UTILS_END_C_DECLS


#endif /* SCOREP_INTERNAL_ENVIRONMENT_H */
