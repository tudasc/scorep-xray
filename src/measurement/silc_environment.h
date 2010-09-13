/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_environment.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#ifndef SILC_INTERNAL_ENVIRONMENT_H
#define SILC_INTERNAL_ENVIRONMENT_H


#include <stdbool.h>
#include <stdint.h>


void
SILC_Env_InitializeCoreEnvironmentVariables();


//bool
//SILC_Env_CoreEnvironmentVariablesInitialized();


bool
SILC_Env_RunVerbose();


bool
SILC_Env_DoUnification();


bool
SILC_Env_DoTracing();


bool
SILC_Env_DoProfiling();


uint64_t
SILC_Env_GetTotalMemory();


uint64_t
SILC_Env_GetPageSize();


#endif /* SILC_INTERNAL_ENVIRONMENT_H */
