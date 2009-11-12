/** @file SILC_User_Init.c
    @author Daniel Lorenz

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
    silc_user_init_regions();
    return SILC_SUCCESS;
}

/** Initializes the location specific data of the user adapter */
SILC_Error_Code
silc_user_init_location()
{
    /* Set the intialization flag to indicate that the adapter is initialized */
    silc_user_is_initialized = 1;

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
    silc_user_final_regions();
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
    "user adapter",
    &silc_user_register,
    &silc_user_init,
    &silc_user_init_location,
    &silc_user_final_location,
    &silc_user_finalize,
    &silc_user_deregister
};
