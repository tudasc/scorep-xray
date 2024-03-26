/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2024,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 * @brief
 * Contains region descriptor struct for GCC and LLVM plugin
 */

#include <config.h>

#include <stdlib.h>

#include "scorep_compiler_plugin.h"

const scorep_compiler_region_description
__attribute__( ( section( ".scorep.region.descrs" ) ) )
scorep_region_descriptions_end =
{
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    0,
    0
};
