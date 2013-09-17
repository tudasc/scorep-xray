/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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
 *
 */



/**
 * @file
 *
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>

#include "SCOREP_Mpi_Reg.h"

#include "scorep_mpi_confvars.inc.c"

static size_t scorep_mpi_subsystem_id;

/**
   Implementation of the adapter_register function of the @ref SCOREP_Subsystem struct
   for the initialization process of the MPI adapter.
 */
static SCOREP_ErrorCode
scorep_mpi_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_mpi_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegisterCond( "mpi",
                                      scorep_mpi_configs,
                                      HAVE_BACKEND_MPI_SUPPORT );
}

/* The initialization struct for the MPI adapter */
const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter =
{
    .subsystem_name              = "MPI (config variables only)",
    .subsystem_register          = &scorep_mpi_register,
    .subsystem_init              = NULL,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = NULL,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};
