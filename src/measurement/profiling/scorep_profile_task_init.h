/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_PROFILE_TASK_INIT_H
#define SCOREP_PROFILE_TASK_INIT_H

/**
 * @file
 * @brief   Declaration of functions related to task handling that are used by other
 *          modules inside he profiling.
 *
 */

#include <stdint.h>
#include <scorep_profile_node.h>
#include <SCOREP_Profile_Tasking.h>


/**
   Returns whether the measured program used tasks. This function does return a
   valid result only after the parallel regions are done.
 */
int32_t
scorep_profile_has_tasks( void );


/**
   Initializes the tasking subsystem
 */
void
scorep_profile_task_initialize( void );

/**
   Updates tasks statistics at an exit of the parallel region.
   @param thread Location data of the current location.
 */
void
scorep_profile_update_task_metrics( SCOREP_Profile_LocationData* location );

#endif /* SCOREP_PROFILE_TASK_INIT_H */
