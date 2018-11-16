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

#include <SCOREP_Subsystem.h>

#include "scorep_user_selective_confvars.inc.c"

#include "scorep_user_topology_confvars.inc.c"

static size_t user_subsystem_id;


/** Registers the required configuration variables of the user adapter
    to the measurement system. Currently, it registers no variables.
 */
static SCOREP_ErrorCode
user_subsystem_register( size_t subsystem_id )
{
    user_subsystem_id = subsystem_id;

    SCOREP_ConfigRegister( "selective", scorep_selective_confvars );
    SCOREP_ConfigRegister( "topology", scorep_user_topology_confvars );

    return SCOREP_SUCCESS;
}

/* The initialization struct for the MPI adapter */
const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter =
{
    .subsystem_name     = "USER (config variables only)",
    .subsystem_register = &user_subsystem_register,
};
