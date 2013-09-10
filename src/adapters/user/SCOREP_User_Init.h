/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_USER_INIT_H
#define SCOREP_USER_INIT_H

/** @file

    This file contains the declaration of the initialization struct of the user
    adapter.
 */


#include "SCOREP_Subsystem.h"
#include "SCOREP_RuntimeManagement.h"
#include "SCOREP_Mutex.h"
#include "SCOREP_Hashtab.h"

/** @ingroup SCOREP_User_External
    @{
 */

/** The struct which contains the initialization functions for the user adapter.
 */
extern const SCOREP_Subsystem SCOREP_Subsystem_UserAdapter;

/** @internal
    Flag to indicate whether the adapter was initialized. If it is set to zero it is not
    initialized. if it is set to non-zero it is initialized
 */
extern int8_t scorep_user_is_initialized;

/**
   Mutex to avoid parallel assignments to the same user metric.
 */
extern SCOREP_Mutex scorep_user_metric_mutex;

/**
   Mutex for @ref scorep_user_file_table.
 */
extern SCOREP_Mutex scorep_user_file_table_mutex;

/**
   Mutex to avoid parallel assignement of region handles to the same region.
 */
extern SCOREP_Mutex scorep_user_region_mutex;

/**
    @internal
    Hash table for mapping regions names to the User adapte region structs.
    Needed for the fortran regions which can not be initialized in declaration. We can
    not determine by the handle value whether we initialized the region already. Thus, we need
    to lookup the name in a extra datastructure.
 */
extern SCOREP_Hashtab* scorep_user_region_table;

/** @def SCOREP_USER_ASSERT_INITIALIZED
    Checks if the adapter is already initialized. If not, the measurement system is
    initialized which should initialized the adapter.
 */
#define SCOREP_USER_ASSERT_INITIALIZED \
    if ( scorep_user_is_initialized != 1 ) { \
        if ( scorep_user_is_initialized == 0 ) { SCOREP_InitMeasurement(); } \
        else { return; } \
    }


#define SCOREP_USER_ASSERT_NOT_FINALIZED \
    if ( scorep_user_is_initialized == 2 ) { return; }

/** @} */

#endif /* SCOREP_USER_INIT_H */
