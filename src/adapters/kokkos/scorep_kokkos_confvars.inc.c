/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

uint64_t scorep_kokkos_features;

/*
 * Mapping of options for Kokkos measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry kokkos_enable_groups[] =
{
    {
        "regions",
        SCOREP_KOKKOS_FEATURE_REGIONS,
        "Kokkos parallel regions"
    },
    {
        "user",
        SCOREP_KOKKOS_FEATURE_USER,
        "Kokkos user regions"
    },
    {
        "malloc",
        SCOREP_KOKKOS_FEATURE_MALLOC,
        "Kokkos memory allocation"
    },
    {
        "memcpy",
        SCOREP_KOKKOS_FEATURE_MEMCPY,
        "Kokkos deep copy"
    },
    {
        "default/yes/1",
        SCOREP_KOKKOS_FEATURES_DEFAULT,
        "Kokkos parallel regions, user regions, and allocations"
    },
    { NULL, 0, NULL }
};


/*
 *  Configuration variables for the Kokkos adapter.
 */
static SCOREP_ConfigVariable scorep_kokkos_confvars[] =
{
    {
        "enable",
        SCOREP_CONFIG_TYPE_BITSET, /* type */
        &scorep_kokkos_features,   /* pointer to target variable */
        ( void* )kokkos_enable_groups,
        "no",                      /* default value */
        "Kokkos measurement features",
        "Sets the Kokkos measurement mode to capture:"
    },
    SCOREP_CONFIG_TERMINATOR
};
