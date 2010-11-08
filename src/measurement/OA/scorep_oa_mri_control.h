/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
 * @file        scorep_oa_mri_control.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of OA measurement configuration and application control functionality
 *
 * @status alpha
 */


#include "scorep_utility/SCOREP_Utils.h"


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

typedef enum
{
    SCOREP_OA_MRI_SYMBOLS_REGIONS,
    SCOREP_OA_MRI_SYMBOLS_METRICS,
    SCOREP_OA_MRI_CONTEXTS,
    SCOREP_OA_MRI_MEASUREMENTS_ALL,
    SCOREP_OA_MRI_MEASUREMENTS_MPI,
    SCOREP_OA_MRI_MEASUREMENTS_COUNTER
} scorep_oa_mri_measurement_return_header_type;                   ///@TODO move to return measurements or to scorep_oa_mri_control.c

SCOREP_Error_Code
scorep_oa_mri_receive_and_process_requests
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
);

void
scorep_oa_mri_set_appl_control
(
    scorep_oa_mri_app_control_type command,
    uint8_t                        file_id,     ///@TODO get rid of line number and file ID, should be OA_Phase name instead or region handle
    uint8_t                        region_line
);


scorep_oa_mri_app_control_type
scorep_oa_mri_get_appl_control
(
);

char*
scorep_oa_mri_get_appl_control_string
(
);

void
scorep_oa_mri_return_summary_data
(
);

void
scorep_oa_mri_noop
(
);

SCOREP_Error_Code
scorep_oa_mri_parse
(
    char* buffer
);

int
yyparse
(
    void
);

void
scorep_oa_mri_set_mpiprofiling
(
    int value
);


void
scorep_oa_mri_set_profiling
(
    int value
);

void
scorep_oa_mri_setphase
(
    int32_t file_id,
    int32_t rfl
);




#endif /* SCOREP_OA_MRI_CONTROL_H */
