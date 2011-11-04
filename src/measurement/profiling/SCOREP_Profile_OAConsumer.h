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

#ifndef SCOREP_PROFILE_OACONSUMER_H
#define SCOREP_PROFILE_OACONSUMER_H

/**
 * @file        SCOREP_Profile_OAConsumer.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief Interface called by the OA module to get the profile measurements.
 *
 */
#include "SCOREP_Types.h"

#define MAX_COUNTER_NAME_LENGTH                         256
#define MAX_COUNTER_UNIT_LENGTH                         10
#define MAX_REGION_NAME_LENGTH                          150
#define MAX_FILE_NAME_LENGTH                            150

typedef struct SCOREP_OA_Key_struct
{
    uint32_t parent_region_id;
    uint32_t region_id;
    uint32_t metric_id;
}SCOREP_OA_Key;

typedef struct SCOREP_OA_CallPathContext_struct
{
    uint32_t region_id;
    uint32_t context_id;
    uint32_t parent_context_id;
    uint32_t thread;
    uint64_t rank;
    uint64_t call_count;
}SCOREP_OA_CallPathContext;

typedef struct SCOREP_OA_CallPathMeasurement_struct
{
    uint32_t context_id;
    uint32_t counter_id;
    uint64_t sum;
    uint64_t count;
}SCOREP_OA_CallPathMeasurement;

typedef struct SCOREP_OA_CallPathCounterDef_struct
{
    uint32_t counter_id;
    char     name[ MAX_COUNTER_NAME_LENGTH ];
    char     unit[ MAX_COUNTER_UNIT_LENGTH ];
    uint32_t status;
}SCOREP_OA_CallPathCounterDef;

typedef struct SCOREP_OA_CallPathRegionDef_struct
{
    uint32_t region_id;
    char     name[ MAX_REGION_NAME_LENGTH ];
    char     file[ MAX_FILE_NAME_LENGTH ];
    uint32_t rfl;
    uint32_t rel;
    uint32_t adapter_type;
}SCOREP_OA_CallPathRegionDef;


typedef struct SCOREP_OA_StaticProfileMeasurement_struct
{
    uint32_t measurement_id;
    uint64_t rank;
    uint32_t thread;
    uint32_t region_id;
    uint64_t samples;
    uint32_t metric_id;
    uint64_t int_val;
}SCOREP_OA_StaticProfileMeasurement;

typedef struct SCOREP_OA_ProfileSummary_struct
{
    SCOREP_OA_CallPathContext*          context_buffer;
    SCOREP_OA_CallPathMeasurement*      measurement_buffer;
    SCOREP_OA_CallPathCounterDef*       counter_def_buffer;
    SCOREP_OA_CallPathRegionDef*        region_def_buffer;
    SCOREP_OA_StaticProfileMeasurement* static_measurement_buffer;
    uint32_t                            context_size;
    uint32_t                            measurement_size;
    uint32_t                            counter_def_size;
    uint32_t                            region_def_size;
    uint32_t                            static_measurement_size;
} SCOREP_OA_CallPathSummary;

#define SCOREP_OA_COUNTER_UNDEFINED                     0
#define SCOREP_OA_COUNTER_TIME                  1
#define SCOREP_OA_COUNTER_LATE                  2

typedef enum SCOREP_OAConsumer_DataTypes
{
    STATIC_PROFILE = 0,
    MERGED_REGION_DEFINITIONS,
    REGION_DEFINITIONS,
    COUNTER_DEFINITIONS,
    CALLPATH_PROFILE_CONTEXTS,
    CALLPATH_PROFILE_MEASUREMENTS
} SCOREP_OAConsumer_DataTypes;


/*SCOREP_OA_CallPathSummary*
   SCOREP_Profile_GetSummary
   (
   );*/

void
SCOREP_OAConsumer_Initialize
(
    SCOREP_RegionHandle phase_handle
);

void
SCOREP_OAConsumer_DismissData
(
);

void*
SCOREP_OAConsumer_GetData
(
    SCOREP_OAConsumer_DataTypes data_type
);

uint32_t
SCOREP_OAConsumer_GetDataSize
(
    SCOREP_OAConsumer_DataTypes data_type
);

#endif // SCOREP_PROFILE_OACONSUMER_H
