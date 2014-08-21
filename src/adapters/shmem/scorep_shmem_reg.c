/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup    SHMEM_Wrapper
 *
 * @brief Registration of SHMEM regions
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <SCOREP_Filter.h>

#include "scorep_shmem_internal.h"

#define SCOREP_SHMEM_PROCESS_FUNC( return_type, func, func_args ) \
    SCOREP_RegionHandle scorep_shmem_region__ ##func;

#include "scorep_shmem_function_list.inc"

SCOREP_RegionHandle scorep_shmem_region__SHMEM;


/**
 * Register SHMEM functions and initialize data structures
 */
void
scorep_shmem_register_regions( void )
{
#define SCOREP_SHMEM_PROCESS_FUNC( return_type, func, func_args )                                               \
    scorep_shmem_region__ ##func = SCOREP_Definitions_NewRegion( #func,                                         \
                                                                 NULL,                                          \
                                                                 SCOREP_Definitions_NewSourceFile( "SHMEM" ),   \
                                                                 SCOREP_INVALID_LINE_NO,                        \
                                                                 SCOREP_INVALID_LINE_NO,                        \
                                                                 SCOREP_PARADIGM_SHMEM,                         \
                                                                 SCOREP_REGION_FUNCTION );

#include "scorep_shmem_function_list.inc"

    /*
     * Artificial root for SHMEM-only experiments without user-code instrumentation
     */

    scorep_shmem_region__SHMEM =
        SCOREP_Definitions_NewRegion( "PARALLEL",
                                      NULL,
                                      SCOREP_INVALID_SOURCE_FILE,
                                      SCOREP_INVALID_LINE_NO,
                                      SCOREP_INVALID_LINE_NO,
                                      SCOREP_PARADIGM_SHMEM,
                                      SCOREP_REGION_ARTIFICIAL );
}
