/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_OA_MRI_CONTROL_H
#define SCOREP_OA_MRI_CONTROL_H


/**
 * @file
 *
 * @brief   Declaration of OA measurement configuration and application control functionality
 *
 */


#include <SCOREP_ErrorCodes.h>
#include "SCOREP_Types.h"

/**
 * Enum specifying possible execution control statuses of the application
 */
typedef enum
{
    SCOREP_OA_MRI_STATUS_UNDEFINED,
    SCOREP_OA_MRI_STATUS_SUSPENDED_BEGINNING,
    SCOREP_OA_MRI_STATUS_SUSPENDED_END,
    SCOREP_OA_MRI_STATUS_SUSPENDED_INITIALIZATION,
    SCOREP_OA_MRI_STATUS_RUNNING_TO_END,
    SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING,
    SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE,
} scorep_oa_mri_app_control_type;

/** Indicates how many iterations of the online access region constitute a 'phase'. Default value: 1 */

extern int scorep_oa_iterations_per_phase;

/**
 * Listens for the MRI commands from external tool on the specified connection
 *
 * @param connection a connection handler to listen for MRI commands
 */
void
scorep_oa_mri_receive_and_process_requests( int connection );

/*----------------------------------------------------------------------------------------
   Getters
   -------------------------------------------------------------------------------------*/


/**
 * Returns current application execution control status
 */
scorep_oa_mri_app_control_type
scorep_oa_mri_get_appl_control( void );

/*----------------------------------------------------------------------------------------
   Configuration of execution control
   -------------------------------------------------------------------------------------*/
/**
 * Sets application execution control status
 *
 * @param command new execution control status
 */
void
scorep_oa_mri_set_appl_control( scorep_oa_mri_app_control_type command );

/**
 * Sets Online Access phase
 *
 * @param handle a region handle to set as OA phase
 */
void
scorep_oa_mri_set_phase( SCOREP_RegionHandle handle );

/**
 * Sets the number of iterations per OA phase
 *
 * @param iterations number of iterations
 */
void
scorep_oa_mri_set_num_iterations( int iterations );

/**
 * Enable/Disable MPI profiling
 *
 * @param value MPI profiling status (0 - OFF, 1 - ON)
 */
void
scorep_oa_mri_set_mpiprofiling( int value );

/*----------------------------------------------------------------------------------------
   Retrieval of measurements
   -------------------------------------------------------------------------------------*/

/**
 * Returns requested measurements to the external tool
 *
 * @param connection a connection handle to send measurements over
 */
void
scorep_oa_mri_return_summary_data( int connection );

#endif /* SCOREP_OA_MRI_CONTROL_H */
