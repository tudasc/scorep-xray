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
 * @brief Registration of OpenCL function pointers
 */

#include <config.h>

#include "scorep_opencl_function_pointers.h"
#include <SCOREP_Libwrap.h>

#include <stdio.h>


static const char*              wrapped_lib_name          = "libOpenCL.so";
static SCOREP_LibwrapHandle*    opencl_libwrap_handle     = NULL;
static SCOREP_LibwrapAttributes opencl_libwrap_attributes =
{
    SCOREP_LIBWRAP_MODE_SHARED, /* libwrap mode */
    1,                          /* number of wrapped libraries */
    &wrapped_lib_name           /* name of wrapped library */
};

#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args ) \
    return_type( *scorep_opencl_funcptr_ ## func ) func_args;

#include "scorep_opencl_function_list.inc"


/**
 * Register OpenCL functions and initialize data structures
 */
void
scorep_opencl_register_function_pointers( void )
{
    SCOREP_Libwrap_Create( &opencl_libwrap_handle,
                           &opencl_libwrap_attributes );

#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args )                  \
    SCOREP_Libwrap_SharedPtrInit( opencl_libwrap_handle, #func,                     \
                                  ( void** )( &scorep_opencl_funcptr_ ##func ) );

#include "scorep_opencl_function_list.inc"
}
