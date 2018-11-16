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

#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include <SCOREP_Subsystem.h>

#include "scorep_mpi_confvars.inc.c"

static size_t mpi_subsystem_id;

/**
   Implementation of the adapter_register function of the @ref SCOREP_Subsystem struct
   for the initialization process of the MPI adapter.
 */
static SCOREP_ErrorCode
mpi_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    mpi_subsystem_id = subsystem_id;

    SCOREP_ConfigRegisterCond( "mpi",
                               scorep_mpi_confvars,
                               HAVE_BACKEND_MPI_SUPPORT );
    SCOREP_ConfigRegisterCond( "topology", scorep_mpi_topology_confvars,
                               HAVE_BACKEND_MPI_SUPPORT );
    return SCOREP_SUCCESS;
}

/* The initialization struct for the MPI adapter */
const SCOREP_Subsystem SCOREP_Subsystem_MpiAdapter =
{
    .subsystem_name     = "MPI (config variables only)",
    .subsystem_register = &mpi_subsystem_register,
};
