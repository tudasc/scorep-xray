/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */

bool scorep_shmem_memory_recording = false;


static const SCOREP_ConfigVariable scorep_shmem_confvars[] = {
    {
        "memory_recording",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_shmem_memory_recording,
        NULL,
        "false",
        "Enable tracking of memory allocations done by calls to the SHMEM "
        "allocation API",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
