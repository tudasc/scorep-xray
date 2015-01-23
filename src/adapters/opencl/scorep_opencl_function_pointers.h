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
 * A function pointer of each wrapped OpenCL function is registered at
 * initialization time of the OpenCL adapter.
 */

#ifndef SCOREP_OPENCLWRAP_FUNC_PTR_H
#define SCOREP_OPENCLWRAP_FUNC_PTR_H

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

#include <SCOREP_Definitions.h>


#ifdef SCOREP_LIBWRAP_STATIC

/*
 * In static mode: nothing to do
 */
#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args )

#elif SCOREP_LIBWRAP_SHARED

/*
 * In shared mode:
 * - Declaration of Score-P region handles for wrapped OpenCL functions
 */
#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args )   \
    extern return_type( *scorep_opencl_funcptr_ ## func ) func_args;

#else

#error Use of function pointers in unsupported OpenCL wrapping mode

#endif


#include "scorep_opencl_function_list.inc"

/**
 * Register OpenCL functions and initialize data structures
 */
void
scorep_opencl_register_function_pointers( void );

#endif /* SCOREP_OPENCLWRAP_FUNC_PTR_H */
