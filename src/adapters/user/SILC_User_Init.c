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

/** @file SILC_User_Init.c
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file contains the implmentation of the initialization functions of the user
    adapter.
 */

#include "SILC_User_Init.h"
#include "SILC_Types.h"
#include "SILC_Error.h"

extern void
silc_user_init_regions();
extern void
silc_user_final_regions();
extern void
silc_user_init_metric();

int8_t silc_user_is_initialized = 0;


/** Registers the required configuration variables of the user adapter
    to the measurement system. Currently, it registers no variables.
 */
SILC_Error_Code
silc_user_register()
{
    return SILC_SUCCESS;
}

/** Initializes the user adapter.
 */
SILC_Error_Code
silc_user_init()
{
    if ( silc_user_is_initialized == 0 )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        silc_user_is_initialized = 1;

        silc_user_init_regions();
        silc_user_init_metric();
    }
    return SILC_SUCCESS;
}

/** Initializes the location specific data of the user adapter */
SILC_Error_Code
silc_user_init_location()
{
    return SILC_SUCCESS;
}

/** Finalizes the location specific data of the user adapter.
 */
void
silc_user_final_location()
{
}

/** Finalizes the user adapter.
 */
void
silc_user_finalize()
{
    if ( silc_user_is_initialized == 1 )
    {
        /*  Set the intialization flag to indicate that the adapter is not initialized */
        silc_user_is_initialized = 0;
        silc_user_final_regions();
    }
}

/** Deregisters the user adapter.
 */
void
silc_user_deregister()
{
}

SILC_Adapter SILC_User_Adapter =
{
    SILC_ADAPTER_USER,
    "user",
    &silc_user_register,
    &silc_user_init,
    &silc_user_init_location,
    &silc_user_final_location,
    &silc_user_finalize,
    &silc_user_deregister
};
