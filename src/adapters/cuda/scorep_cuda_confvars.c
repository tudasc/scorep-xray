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

#define SCOREP_DEBUG_MODULE_NAME CUDA
#include <UTILS_Debug.h>


#include <SCOREP_Config.h>
#include <SCOREP_Subsystem.h>

#include "scorep_cuda_confvars.inc.c"


/** Registers the required configuration variables of the CUDA adapter
    to the measurement system.
 */
static SCOREP_ErrorCode
cuda_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegisterCond( "cuda",
                                      scorep_cuda_confvars,
                                      HAVE_BACKEND_CUDA_SUPPORT );
}

const SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter =
{
    .subsystem_name     = "CUDA (config variables only)",
    .subsystem_register = &cuda_subsystem_register,
};
