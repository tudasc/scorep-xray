/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_MEMORY_MGMT_H
#define SCOREP_MEMORY_MGMT_H

/**
 * @file
 */


#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Config.h>
#include <SCOREP_AllocMetric.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCOREP_MEMORY_REGIONS                            \
    SCOREP_MEMORY_REGION( MALLOC,                   ALLOCATE,   "malloc" ) \
    SCOREP_MEMORY_REGION( REALLOC,                  REALLOCATE, "realloc" ) \
    SCOREP_MEMORY_REGION( CALLOC,                   ALLOCATE,   "calloc" ) \
    SCOREP_MEMORY_REGION( FREE,                     DEALLOCATE, "free" ) \
    SCOREP_MEMORY_REGION( MEMALIGN,                 ALLOCATE,   "memalign" ) \
    SCOREP_MEMORY_REGION( POSIX_MEMALIGN,           ALLOCATE,   "posix_memalign" ) \
    SCOREP_MEMORY_REGION( VALLOC,                   ALLOCATE,   "valloc" ) \
    SCOREP_MEMORY_REGION( ALIGNED_ALLOC,            ALLOCATE,   "aligned_alloc" ) \
    SCOREP_MEMORY_REGION( NEW,                      ALLOCATE,   "operator new(size_t)" ) \
    SCOREP_MEMORY_REGION( DELETE,                   DEALLOCATE, "operator delete(void*)" ) \
    SCOREP_MEMORY_REGION( DELETE_SIZE,              DEALLOCATE, "operator delete(void*,size_t)" ) \
    SCOREP_MEMORY_REGION( NEW_ARRAY,                ALLOCATE,   "operator new[](size_t)" ) \
    SCOREP_MEMORY_REGION( DELETE_ARRAY,             DEALLOCATE, "operator delete[](void*)" ) \
    SCOREP_MEMORY_REGION( DELETE_ARRAY_SIZE,        DEALLOCATE, "operator delete[](void*,size_t)" ) \
    SCOREP_MEMORY_REGION( HBW_MALLOC,               ALLOCATE,   "hbw_malloc" ) \
    SCOREP_MEMORY_REGION( HBW_REALLOC,              REALLOCATE, "hbw_realloc" ) \
    SCOREP_MEMORY_REGION( HBW_CALLOC,               ALLOCATE,   "hbw_calloc" ) \
    SCOREP_MEMORY_REGION( HBW_FREE,                 DEALLOCATE, "hbw_free" ) \
    SCOREP_MEMORY_REGION( HBW_POSIX_MEMALIGN,       ALLOCATE,   "hbw_posix_memalign" ) \
    SCOREP_MEMORY_REGION( HBW_POSIX_MEMALIGN_PSIZE, ALLOCATE,   "hbw_posix_memalign_psize" )


typedef enum scorep_memory_region_types
{
#define SCOREP_MEMORY_REGION( NAME, TYPE, name ) SCOREP_MEMORY_ ## NAME,
    SCOREP_MEMORY_REGIONS
#undef SCOREP_MEMORY_REGION

    SCOREP_MEMORY_REGION_SENTINEL
} scorep_memory_region_types;


extern SCOREP_RegionHandle scorep_memory_regions[ SCOREP_MEMORY_REGION_SENTINEL ];

/**
 * Recording requested by the user.
 */
extern bool scorep_memory_recording;

extern SCOREP_AllocMetric* scorep_memory_metric;

#endif /* SCOREP_MEMORY_MGMT_H */
