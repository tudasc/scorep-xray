/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */

#include <config.h>

#include "scorep_openacc_confvars.h"

#include <SCOREP_Config.h>

uint64_t scorep_openacc_features;


/*
 * Mapping of options for OpenACC measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry openacc_enable_groups[] =
{
    {
        "regions",
        SCOREP_OPENACC_FEATURE_REGIONS,
        "OpenACC regions"
    },
    {
        "wait",
        SCOREP_OPENACC_FEATURE_WAIT,
        "OpenACC wait operations"
    },
    {
        "enqueue",
        SCOREP_OPENACC_FEATURE_ENQUEUE,
        "OpenACC enqueue operations (kernel, upload, download)"
    },
    {
        "device_alloc",
        SCOREP_OPENACC_FEATURE_DEVICE_ALLOC,
        "OpenACC device memory allocations"
    },
    {
        "kernel_properties",
        SCOREP_OPENACC_FEATURE_KERNEL_PROPERTIES,
        "Record kernel properties such as the kernel name as well as the gang, "
        "worker and vector size for kernel launch operations"
    },
    {
        "variable_names",
        SCOREP_OPENACC_FEATURE_VARNAMES,
        "Record variable names for OpenACC data allocation and enqueue upload/download"
    },
    {
        "default/yes/1",
        SCOREP_OPENACC_FEATURES_DEFAULT,
        "OpenACC regions, enqueue and wait operations.\n"
        "Includes:\n"
        "\240-\240regions\n"
        "\240-\240wait\n"
        "\240-\240enqueue"
    },
    { NULL, 0, NULL }
};


/*
 *  Configuration variables for the OpenACC adapter.
 */
static const SCOREP_ConfigVariable scorep_openacc_configs[] =
{
    {
        "enable",
        SCOREP_CONFIG_TYPE_BITSET, /* type */
        &scorep_openacc_features,  /* pointer to target variable */
        ( void* )openacc_enable_groups,
        "no",                      /* default value */
        "OpenACC measurement features",
        "Sets the OpenACC measurement mode to capture:"
    },
    SCOREP_CONFIG_TERMINATOR
};
