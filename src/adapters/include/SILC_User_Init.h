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

#ifndef SILC_USER_INIT_H
#define SILC_USER_INIT_H

/** @file SILC_User_Init.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file contains the declaration of the initialization struct of the user
    adapter.
 */


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
