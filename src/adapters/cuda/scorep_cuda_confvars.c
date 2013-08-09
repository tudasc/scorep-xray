/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       src/adapters/cuda/scorep_cuda_confvars.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CUDA
#include <UTILS_Debug.h>


#include <SCOREP_Config.h>
#include <SCOREP_Subsystem.h>


#include "scorep_cuda.h"


#include "scorep_cuda_confvars.inc.c"


/** Registers the required configuration variables of the CUDA adapter
    to the measurement system.
 */
static SCOREP_ErrorCode
scorep_cuda_register( size_t subsystem_id )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegisterCond( "cuda",
                                      scorep_cuda_configs,
                                      HAVE_BACKEND_CUDA_SUPPORT );
}

SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter =
{
    .subsystem_name              = "CUDA (config variables only)",
    .subsystem_register          = &scorep_cuda_register,
    .subsystem_init              = NULL,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = NULL,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};
