/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup    OpenCL_Wrapper
 *
 * @brief Registration of OpenCL regions
 */

#include <config.h>

#include <stdio.h>

#include <scorep_opencl_regions.h>


#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args ) \
    SCOREP_RegionHandle scorep_opencl_region__ ##func;

#include "scorep_opencl_function_list.inc"


/**
 * Register OpenCL functions and initialize data structures
 */
void
scorep_opencl_register_regions( void )
{
#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args )                                               \
    scorep_opencl_region__ ##func = SCOREP_Definitions_NewRegion( #func,                                         \
                                                                  NULL,                                          \
                                                                  SCOREP_Definitions_NewSourceFile( "OpenCL" ),  \
                                                                  SCOREP_INVALID_LINE_NO,                        \
                                                                  SCOREP_INVALID_LINE_NO,                        \
                                                                  SCOREP_PARADIGM_OPENCL,                        \
                                                                  SCOREP_REGION_WRAPPER );

#include "scorep_opencl_function_list.inc"
}
