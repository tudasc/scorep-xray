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


#ifndef SILC_OA_MRI_CONTROL_H
#define SILC_OA_MRI_CONTROL_H


/**
 * @file        silc_oa_mri_control.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of OA measurement configuration and application control functionality
 *
 * @status ALPHA
 */


#include "SILC_Types.h"
#include "SILC_Error.h"



typedef enum
{
    SILC_OA_MRI_STATUS_UNDEFINED,
    SILC_OA_MRI_STATUS_SUSPENDED_TERMINATION,
    SILC_OA_MRI_STATUS_SUSPENDED_BEGINNING,
    SILC_OA_MRI_STATUS_SUSPENDED_END,
    SILC_OA_MRI_STATUS_SUSPENDED_INITIALIZATION,
    SILC_OA_MRI_STATUS_RUNNING_TO_END,
    SILC_OA_MRI_STATUS_RUNNING_TO_BEGINNING,
    SILC_OA_MRI_EXEC_REQUEST_TERMINATE,
    SILC_OA_MRI_EXEC_REQUEST_RUN_TO_BEGINNING,
    SILC_OA_MRI_EXEC_REQUEST_RUN_TO_END
} silc_oa_mri_app_control_type;

typedef enum
{
    SILC_OA_MRI_SYMBOLS_REGIONS,
    SILC_OA_MRI_SYMBOLS_METRICS,
    SILC_OA_MRI_CONTEXTS,
    SILC_OA_MRI_MEASUREMENTS_ALL,
    SILC_OA_MRI_MEASUREMENTS_MPI,
    SILC_OA_MRI_MEASUREMENTS_COUNTER
} silc_oa_mri_measurement_return_header_type;                   ///@TODO move to return measurements or to silc_oa_mri_control.c

SILC_Error_Code
silc_oa_mri_receive_and_process_requests
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
);

void
silc_oa_mri_set_appl_control
(
    silc_oa_mri_app_control_type command,
    uint8_t                      file_id,       ///@TODO get rid of line number and file ID, should be OA_Phase name instead or region handle
    uint8_t                      region_line
);


silc_oa_mri_app_control_type
silc_oa_mri_get_appl_control
(
);

char*
silc_oa_mri_get_appl_control_string
(
);

void
silc_oa_mri_return_summary_data
(
);

void
silc_oa_mri_noop
(
);

SILC_Error_Code
silc_oa_mri_parse
(
    char* buffer
);

int
yyparse
(
    void
);

void
silc_oa_mri_set_mpiprofiling
(
    int value
);


void
silc_oa_mri_set_profiling
(
    int value
);

void
silc_oa_mri_setphase
(
    int32_t file_id,
    int32_t rfl
);




#endif /* SILC_OA_MRI_CONTROL_H */
