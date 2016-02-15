/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 *
 * Copyright (c) 2016,
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

bool scorep_memory_recording;

/*
 *  Configuration variables for the MEMORY adapter.
 */
static const SCOREP_ConfigVariable scorep_memory_confvars[] = {
    {
        "recording",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_memory_recording,
        NULL,
        "false",
        "Memory recording",
        "Memory (de)allocations are recorded via the libc/C++ API."
    },
    SCOREP_CONFIG_TERMINATOR
};
