/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Commonly used definitions and functionality in the HIP adapter.
 */

#ifndef SCOREP_HIP_H
#define SCOREP_HIP_H

#include <SCOREP_DefinitionHandles.h>

#include <stdlib.h>
#include <stdint.h>

#define SCOREP_HIP_NO_RANK 0xFFFFFFFF

typedef struct scorep_hip_cpu_location_data
{
    uint32_t local_rank;
} scorep_hip_cpu_location_data;

extern size_t scorep_hip_subsystem_id;

extern uint64_t                         scorep_hip_features;
extern uint64_t                         scorep_hip_activity_buffer_size;
extern uint64_t                         scorep_hip_global_location_count;
extern uint64_t*                        scorep_hip_global_location_ids;
extern SCOREP_InterimCommunicatorHandle scorep_hip_interim_communicator_handle;

void
scorep_hip_collect_comm_locations( void );

void
scorep_hip_unify_pre( void );

void
scorep_hip_unify_post( void );

#endif  /* SCOREP_HIP_H */
