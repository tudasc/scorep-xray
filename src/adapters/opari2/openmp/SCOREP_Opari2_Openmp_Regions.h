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
 * Copyright (c) 2009-2011, 2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OPARI2_OPENMP_REGIONS_H
#define SCOREP_OPARI2_OPENMP_REGIONS_H

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Declares functionality for management of OpenMP regions.
 */

#include "SCOREP_Opari2_Openmp_Lock.h"

#include <SCOREP_Mutex.h>
#include <SCOREP_Opari2_Region_Info.h>
#include <pomp2_user_region_info.h>

/**
 * SCOREP_Opari2_RegionType
 * @ingroup OPARI2
 * @{
 *
 */

/**
   Lock to protect on-the-fly assignments.
 */
extern SCOREP_Mutex scorep_opari2_openmp_assign_lock;

/**
   @def SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( handle, ctc_string )
   Checks whether @a handle is initialized and initializes it if it is not.
   @param handle      A pointer to a POMP2_Region_handle.
   @param ctc_string  A string that contains the initialization information.
 */
#define SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( handle, ctc_string ) \
    if ( *handle == NULL )                                                   \
    {                                                                        \
        POMP2_Assign_handle( handle, ctc_string );                           \
    }

/* Must be in the same order as the entries in region_type_map in
   order to ensure correct lookup */
typedef enum
{
    SCOREP_POMP2_Atomic,
    SCOREP_POMP2_Barrier,
    SCOREP_POMP2_Critical,
    SCOREP_POMP2_Do,
    SCOREP_POMP2_Flush,
    SCOREP_POMP2_For,
    SCOREP_POMP2_Master,
    SCOREP_POMP2_Ordered,
    SCOREP_POMP2_Parallel,
    SCOREP_POMP2_ParallelDo,
    SCOREP_POMP2_ParallelFor,
    SCOREP_POMP2_ParallelSections,
    SCOREP_POMP2_ParallelWorkshare,
    SCOREP_POMP2_Sections,
    SCOREP_POMP2_Single,
    SCOREP_POMP2_Task,
    SCOREP_POMP2_Taskuntied,
    SCOREP_POMP2_Taskwait,
    SCOREP_POMP2_Workshare,
    SCOREP_POMP2_NONE
} SCOREP_Opari2_Openmp_Region_Type;

/** Struct which contains all data for an openmp region. */
typedef struct /* SCOREP_Opari2_Openmp_Region */
{
    SCOREP_Opari2_Region             genericInfo;
    SCOREP_Opari2_Openmp_Region_Type regionType;

    char*                            name;        /* critical region name          */
    uint32_t                         numSections; /* sections only: number of sections     */
    /* For combined statements (parallel sections, parallel for) we need up to
       four SCOREP regions. So we use a pair of SCOREP regions for the parallel
       statements and a pair of regions for other statements.
       We must reserve space for 3 region handles, because the parallel sections
       construct uses three region handles:
         1. parallel region (parallel begin/end events)
         2. enclosing sections construct (sections enter/exit)
         3. individual sections begin/end                                              */
    SCOREP_RegionHandle        innerParallel;     /* SCOREP handle for the inner parallel region */
    SCOREP_RegionHandle        barrier;           /* SCOREP handle for the (implicit barrier)    */
    SCOREP_RegionHandle        outerBlock;        /* SCOREP handle for the enclosing region      */
    SCOREP_RegionHandle        innerBlock;        /* SCOREP handle for the enclosed region       */

    SCOREP_Opari2_Openmp_Lock* lock;
} SCOREP_Opari2_Openmp_Region;


void
scorep_opari2_openmp_criticals_finalize( void );

/** @} */

#endif /* SCOREP_POMP_OPENMP_REGIONS_H */
