/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016,
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

#include "scorep_shmem_internal.h"

#define SCOREP_SHMEM_PROCESS_FUNC( type, return_type, func, func_args ) \
    SCOREP_RegionHandle scorep_shmem_region__ ##func;

#include "scorep_shmem_function_list.inc"


/**
 * Register SHMEM functions and initialize data structures
 */
void
scorep_shmem_register_regions( void )
{
    SCOREP_SourceFileHandle source_file_handle =
        SCOREP_Definitions_NewSourceFile( SCOREP_SHMEM_NAME );

#define SCOREP_SHMEM_PROCESS_FUNC( type, return_type, func, func_args )                    \
    scorep_shmem_region__ ##func = SCOREP_Definitions_NewRegion( #func,                    \
                                                                 NULL,                     \
                                                                 source_file_handle,       \
                                                                 SCOREP_INVALID_LINE_NO,   \
                                                                 SCOREP_INVALID_LINE_NO,   \
                                                                 SCOREP_PARADIGM_SHMEM,    \
                                                                 SCOREP_REGION_ ## type );


#include "scorep_shmem_function_list.inc"
}
