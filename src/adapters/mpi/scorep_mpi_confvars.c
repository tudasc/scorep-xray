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
 * @file       src/adapters/mpi/scorep_mpi_confvars.c
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
#include <stdbool.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>
#include <SCOREP_Location.h>

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
const SCOREP_Subsystem SCOREP_Mpi_Adapter =
{
    "MPI (config variables only)",
    &scorep_mpi_register,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
