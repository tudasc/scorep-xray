/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2017,
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

#include "scorep_opencl_libwrap.h"

#include <scorep/SCOREP_Libwrap.h>

#include <stdio.h>


/* *INDENT-OFF* */
#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args ) \
    SCOREP_LIBWRAP_DEFINE_REAL_FUNC( ( return_type ), func, func_args );
/* *INDENT-ON* */

#include "scorep_opencl_function_list.inc"


/**
 * Register OpenCL functions and initialize data structures
 */
static void
opencl_init_function_pointers( SCOREP_LibwrapHandle* handle )
{
#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args ) \
    SCOREP_Libwrap_SharedPtrInit( handle, #func, \
                                  ( void** )( &SCOREP_LIBWRAP_FUNC_REAL_NAME( func ) ) );

#include "scorep_opencl_function_list.inc"
}


static const char*                    wrapped_lib_name          = "libOpenCL.so.1";
static SCOREP_LibwrapHandle*          opencl_libwrap_handle     = NULL;
static const SCOREP_LibwrapAttributes opencl_libwrap_attributes =
{
    SCOREP_LIBWRAP_VERSION,
    "opencl",                      /* name of the library wrapper */
    "OpenCL",                      /* display name of the library wrapper */
    SCOREP_LIBWRAP_MODE_SHARED,    /* library wrapper mode */
    opencl_init_function_pointers, /* init function */
    1,                             /* number of wrapped libraries */
    &wrapped_lib_name              /* name of wrapped library */
};


/**
 * Register OpenCL functions and initialize data structures
 */
void
scorep_opencl_register_function_pointers( void )
{
    SCOREP_Libwrap_Create( &opencl_libwrap_handle,
                           &opencl_libwrap_attributes );
}
