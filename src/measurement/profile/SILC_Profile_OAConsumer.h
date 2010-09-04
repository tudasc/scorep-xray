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

#ifndef SILC_PROFILE_OACONSUMER_H
#define SILC_PROFILE_OACONSUMER_H

/**
 * @file        SILC_Profile_OAConsumer.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief Interface called by the OA module to get the profile measurements.
 *
 */
#include "SILC_Types.h"

#define MAX_COUNTER_NAME_LENGTH                         256
#define MAX_COUNTER_UNIT_LENGTH                         10
#define MAX_REGION_NAME_LENGTH                          20
#define MAX_FILE_NAME_LENGTH                            20

typedef struct SILC_OA_PeriscopeContext_struct
{
    uint32_t region_id;
    uint32_t context_id;
    uint32_t parent_context_id;
    uint32_t thread_id;
    uint32_t rank;
    uint64_t call_count;
}SILC_OA_PeriscopeContext;

typedef struct SILC_OA_PeriscopeMeasurement_struct
{
    uint32_t context_id;
    uint32_t counter_id;
    uint64_t sum;
    uint64_t count;
}SILC_OA_PeriscopeMeasurement;

typedef struct SILC_OA_PeriscopeCounterDef_struct
{
    uint32_t counter_id;
    char     name[ MAX_COUNTER_NAME_LENGTH ];
    char     unit[ MAX_COUNTER_UNIT_LENGTH ];
    uint32_t status;
}SILC_OA_PeriscopeCounterDef;

typedef struct SILC_OA_PeriscopeRegionDef_struct
{
    uint32_t region_id;
    char     name[ MAX_REGION_NAME_LENGTH ];
    char     file[ MAX_FILE_NAME_LENGTH ];
    uint32_t rfl;
}SILC_OA_PeriscopeRegionDef;

typedef struct SILC_OA_PeriscopeSummary_struct
{
    SILC_OA_PeriscopeContext*     context_buffer;
    SILC_OA_PeriscopeMeasurement* measurement_buffer;
    SILC_OA_PeriscopeCounterDef*  counter_def_buffer;
    SILC_OA_PeriscopeRegionDef*   region_def_buffer;
    uint64_t                      context_size;
    uint64_t                      measurement_size;
    uint64_t                      counter_def_size;
    uint64_t                      region_def_size;
} SILC_OA_PeriscopeSummary;


SILC_OA_PeriscopeSummary*
SILC_Profile_GetPeriscopeSummary
(
);

#endif // SILC_PROFILE_OACONSUMER_H
