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
 *  @file
 *
 *  @brief Implementation of the feature configuration for the OpenCL adapter.
 */

#include <config.h>

#include "scorep_opencl_config.h"

#include <UTILS_Error.h>


bool scorep_opencl_record_api     = false;
bool scorep_opencl_record_kernels = false;
bool scorep_opencl_record_memcpy  = false;

void
scorep_opencl_set_features( void )
{
    /* check for OpenCL API */
    if ( scorep_opencl_features & SCOREP_OPENCL_FEATURE_API )
    {
        scorep_opencl_record_api = true;
    }

    /* check for OpenCL kernels */
    if ( scorep_opencl_features & SCOREP_OPENCL_FEATURE_KERNEL )
    {
        scorep_opencl_record_kernels = true;
    }

    /* check for OpenCL memcpy */
    if ( scorep_opencl_features & SCOREP_OPENCL_FEATURE_MEMCPY )
    {
        scorep_opencl_record_memcpy = true;
    }
}

void
scorep_opencl_disable( void )
{
    scorep_opencl_record_api     = false;
    scorep_opencl_record_kernels = false;
    scorep_opencl_record_memcpy  = false;
}
