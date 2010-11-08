/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_USER_INIT_H
#define SCOREP_USER_INIT_H

/** @file SCOREP_User_Init.h
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file contains the declaration of the initialization struct of the user
    adapter.
 */


#include "SCOREP_Adapter.h"
#include "SCOREP_RuntimeManagement.h"

/** @ingroup SCOREP_User_External
    @{
 */

/** The struct which contains the initialization functions for the user adapter.
 */
extern SCOREP_Adapter SCOREP_User_Adapter;

/** @internal
    Flag to indicate whether the adapter was initialized. If it is set to zero it is not
    initialized. if it is set to non-zero it is initialized
 */
extern int8_t scorep_user_is_initialized;


/** @def SCOREP_USER_ASSERT_INITIALIZED
    Checks if the adapter is already initialized. If not, the measurement system is
    initialized which should initialized the adapter.
 */
#define SCOREP_USER_ASSERT_INITIALIZED \
    if ( !scorep_user_is_initialized ) { SCOREP_InitMeasurement(); }

/** @} */

#endif /* SCOREP_USER_INIT_H */
