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
 *  @status     alpha
 *  @file       SCOREP_User_Init.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 *  This file contains the implmentation of the initialization functions of the user
 *  adapter.
 */


#include <config.h>


#include "SCOREP_User_Init.h"
#include "SCOREP_Types.h"
#include <SCOREP_Location.h>
#include "scorep_utility/SCOREP_Error.h"


#include "scorep_user.h"


int8_t scorep_user_is_initialized = 0;


size_t scorep_user_subsystem_id;


/** Registers the required configuration variables of the user adapter
    to the measurement system. Currently, it registers no variables.
 */
static SCOREP_Error_Code
scorep_user_register( size_t subsystem_id )
{
    scorep_user_subsystem_id = subsystem_id;

    return scorep_selective_register();
}

/** Initializes the user adapter.
 */
static SCOREP_Error_Code
scorep_user_init()
{
    if ( scorep_user_is_initialized == 0 )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        scorep_user_is_initialized = 1;

        scorep_selective_init();
        scorep_user_init_regions();
        scorep_user_init_metric();
    }
    return SCOREP_SUCCESS;
}

/** Initializes the location specific data of the user adapter */
static SCOREP_Error_Code
scorep_user_init_location( SCOREP_Location* locationData )
{
    return SCOREP_SUCCESS;
}

/** Finalizes the location specific data of the user adapter.
 */
static void
scorep_user_final_location( SCOREP_Location* locationData )
{
}

/** Finalizes the user adapter.
 */
static void
scorep_user_finalize()
{
    if ( scorep_user_is_initialized == 1 )
    {
        /*  Set the intialization flag to indicate that the adapter is finalized */
        scorep_user_is_initialized = 2;
        scorep_user_final_metric();
        scorep_user_final_regions();
    }
    scorep_selective_finalize();
}

/** Deregisters the user adapter.
 */
static void
scorep_user_deregister()
{
}

const SCOREP_Subsystem SCOREP_User_Adapter =
{
    "user",
    &scorep_user_register,
    &scorep_user_init,
    &scorep_user_init_location,
    &scorep_user_final_location,
    &scorep_user_finalize,
    &scorep_user_deregister
};
