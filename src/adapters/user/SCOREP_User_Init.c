/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
#include "scorep_utility/SCOREP_Error.h"

extern void
scorep_user_init_regions();
extern void
scorep_user_final_regions();
extern void
scorep_user_init_metric();
extern void
scorep_user_final_metric();

int8_t scorep_user_is_initialized = 0;


/** Registers the required configuration variables of the user adapter
    to the measurement system. Currently, it registers no variables.
 */
SCOREP_Error_Code
scorep_user_register()
{
    return SCOREP_SUCCESS;
}

/** Initializes the user adapter.
 */
SCOREP_Error_Code
scorep_user_init()
{
    if ( scorep_user_is_initialized == 0 )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        scorep_user_is_initialized = 1;

        scorep_user_init_regions();
        scorep_user_init_metric();
    }
    return SCOREP_SUCCESS;
}

/** Initializes the location specific data of the user adapter */
SCOREP_Error_Code
scorep_user_init_location()
{
    return SCOREP_SUCCESS;
}

/** Finalizes the location specific data of the user adapter.
 */
void
scorep_user_final_location( void* location )
{
}

/** Finalizes the user adapter.
 */
void
scorep_user_finalize()
{
    if ( scorep_user_is_initialized == 1 )
    {
        /*  Set the intialization flag to indicate that the adapter is not initialized */
        scorep_user_is_initialized = 0;
        scorep_user_final_metric();
        scorep_user_final_regions();
    }
}

/** Deregisters the user adapter.
 */
void
scorep_user_deregister()
{
}

SCOREP_Adapter SCOREP_User_Adapter =
{
    SCOREP_ADAPTER_USER,
    "user",
    &scorep_user_register,
    &scorep_user_init,
    &scorep_user_init_location,
    &scorep_user_final_location,
    &scorep_user_finalize,
    &scorep_user_deregister
};
