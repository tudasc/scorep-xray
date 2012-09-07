/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
#include <UTILS_Debug.h>


#include <SCOREP_Config.h>
#include <SCOREP_Subsystem.h>


#include "scorep_cuda.h"


#define SCOREP_DEBUG_MODULE_NAME CUDA


#include "scorep_cuda_confvars.inc.c"


SCOREP_Subsystem SCOREP_Cuda_Adapter =
{
    "CUDA (config variables only)",
    &scorep_cuda_register,
    NULL,
    NULL,
    NULL,
    NULL,
    &scorep_cuda_deregister
};
