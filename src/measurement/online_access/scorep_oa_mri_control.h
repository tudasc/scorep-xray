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
 * Enum specifing possible execution control statuses of the application
 */
typedef enum
{
    SCOREP_OA_MRI_STATUS_UNDEFINED,
    SCOREP_OA_MRI_STATUS_SUSPENDED_TERMINATION,
    SCOREP_OA_MRI_STATUS_SUSPENDED_BEGINNING,
    SCOREP_OA_MRI_STATUS_SUSPENDED_END,
    SCOREP_OA_MRI_STATUS_SUSPENDED_INITIALIZATION,
    SCOREP_OA_MRI_STATUS_RUNNING_TO_END,
    SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING,
    SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE,
    SCOREP_OA_MRI_EXEC_REQUEST_RUN_TO_BEGINNING,
    SCOREP_OA_MRI_EXEC_REQUEST_RUN_TO_END
} scorep_oa_mri_app_control_type;

/**
 * Listens for the MRI commands from external tol on the specified connection
 *
 * @param connection a connection handler to listen for MRI commands
 */
void
scorep_oa_mri_receive_and_process_requests
(
    int connection
);

/*----------------------------------------------------------------------------------------
   Getters
   -------------------------------------------------------------------------------------*/


/**
 * Returns current application execution control status
 */
scorep_oa_mri_app_control_type
scorep_oa_mri_get_appl_control
(
);

/*----------------------------------------------------------------------------------------
   Configuration of execution control
   -------------------------------------------------------------------------------------*/
/**
 * Sets application execution control status
 *
 * @param command new execution control status
 */
void
scorep_oa_mri_set_appl_control
(
    scorep_oa_mri_app_control_type command
);

/**
 * Sets Online Access phase
 *
 * @param handle a region handle to set as OA phase
 */
void
scorep_oa_mri_set_phase
(
    SCOREP_RegionHandle handle
);

/*----------------------------------------------------------------------------------------
   Configuration of measurements
   -------------------------------------------------------------------------------------*/
/**
 * Empty operation
 */
void
scorep_oa_mri_noop
(
);

/**
 * Enable/Disable MPI profiling
 *
 * @param value MPI profiling status (0 - OFF, 1 - ON)
 */
void
scorep_oa_mri_set_mpiprofiling
(
    int value
);

/**
 * Adds a metric specified by Periscope code to the Score-P measurement configuration requests
 *
 * @param metric_code Persicope metric code to add
 */
void
scorep_oa_mri_add_metric_by_code
(
    int metric_code
);

/**
 * Adds a metric specified by name to the Score-P measurement configuration requests
 *
 * @param metric_code Persicope metric code to add
 */
void
scorep_oa_mri_add_metric_by_name
(
    char* metric_name
);

/**
 * Initializes metric request handling module
 */
void
scorep_oa_mri_begin_request
(
);

/**
 * Submits metric requests to measurement core
 */
void
scorep_oa_mri_end_request
(
);

/*----------------------------------------------------------------------------------------
   Retrieval of measurements
   -------------------------------------------------------------------------------------*/

/**
 * Returns requested measurements to the exeternal tool
 *
 * @param connection a connection handle to send measurements over
 */
void
scorep_oa_mri_return_summary_data
(
    int connection
);

#endif /* SCOREP_OA_MRI_CONTROL_H */
