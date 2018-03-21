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
 * A function pointer of each wrapped OpenCL function is registered at
 * initialization time of the OpenCL adapter.
 */

#ifndef SCOREP_OPENCL_FUNC_PTR_H
#define SCOREP_OPENCL_FUNC_PTR_H

/**
 * Register OpenCL functions and initialize data structures
 */
void
scorep_opencl_register_function_pointers( void );

#endif /* SCOREP_OPENCL_FUNC_PTR_H */
