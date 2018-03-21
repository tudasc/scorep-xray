/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup    OpenCL_Wrapper
 *
 * @brief Basic setup for the library wrapping.
 */

#ifndef SCOREP_OPENCL_LIBWRAP_H
#define SCOREP_OPENCL_LIBWRAP_H

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.h"
#endif

#include <SCOREP_Definitions.h>

#ifdef SCOREP_LIBWRAP_SHARED

#define SCOREP_LIBWRAP_FUNC_REAL_NAME( func ) \
    scorep_opencl_funcptr__ ## func

#endif

#include <scorep/SCOREP_Libwrap_Macros.h>

/*
 * In shared mode:
 * - Declaration of Score-P region handles for wrapped OpenCL functions
 */
#define SCOREP_OPENCL_PROCESS_FUNC( return_type, func, func_args ) \
    SCOREP_LIBWRAP_DECLARE_REAL_FUNC( ( return_type ), func, func_args );

#include "scorep_opencl_function_list.inc"

#endif /* SCOREP_OPENCL_LIBWRAP_H */
