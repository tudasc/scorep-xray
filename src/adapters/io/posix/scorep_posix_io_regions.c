/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup    POSIX_IO_Wrapper
 *
 * @brief Registration of POSIX I/O regions
 */

#ifdef __PGI
#define restrict
#endif

#include <config.h>

#include "scorep_posix_io_regions.h"

#include <stdio.h>


#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args ) \
    SCOREP_RegionHandle scorep_posix_io_region_ ##func;

#include "scorep_posix_io_function_list.inc"


/**
 * Register POSIX I/O functions and initialize data structures
 */
void
scorep_posix_io_register_regions( void )
{
#define SCOREP_POSIX_IO_PROCESS_FUNC( PARADIGM, TYPE, return_type, func, func_args )                                \
    scorep_posix_io_region_ ##func = SCOREP_Definitions_NewRegion( #func,                                           \
                                                                   NULL,                                            \
                                                                   SCOREP_Definitions_NewSourceFile( #PARADIGM ),   \
                                                                   SCOREP_INVALID_LINE_NO,                          \
                                                                   SCOREP_INVALID_LINE_NO,                          \
                                                                   SCOREP_PARADIGM_IO,                              \
                                                                   SCOREP_REGION_##TYPE );

#include "scorep_posix_io_function_list.inc"
}
