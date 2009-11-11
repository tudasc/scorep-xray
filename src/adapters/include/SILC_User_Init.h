/** @file SILC_User_Init.h
    @author Daniel Lorenz

    This file contains the declaration of the initialization struct of the user
    adapter.
 */

#ifndef SILC_USER_INIT_H
#define SILC_USER_INIT_H

#include "SILC_Adapter.h"
#include "SILC_RuntimeManagement.h"

/** @ingroup SILC_User_External
    @{
 */

/** The struct which contains the initialization functions for the user adapter.
 */
extern SILC_Adapter SILC_User_Adapter;

/** @internal
    Flag to indicate whether the adapter was initialized. If it is set to zero it is not
    initialized. if it is set to non-zero it is initialized
 */
extern int8_t silc_user_is_initialized;


/** @def SILC_USER_ASSERT_INITIALIZED
    Checks if the adapter is already initialized. If not, the measurement system is
    initialized which should initialized the adapter.
 */
#define SILC_USER_ASSERT_INITIALIZED \
    if ( !silc_user_is_initialized ) { SILC_InitMeasurement(); }

/** @} */

#endif /* SILC_USER_INIT_H */
