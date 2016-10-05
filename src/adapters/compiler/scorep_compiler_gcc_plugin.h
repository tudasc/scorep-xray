/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013, 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 * @brief Contains definition for
 * region descriptor struct for GCC plugin
 */


#ifndef SCOREP_COMPILER_GCC_PLUGIN_H
#define SCOREP_COMPILER_GCC_PLUGIN_H

#include <stdint.h>

typedef struct
{
    uint32_t*   handle;
    const char* name;
    const char* canonical_name;
    const char* file;
    int         begin_lno;
    int         end_lno;
    unsigned    flags; /* unused */
}
__attribute__( ( aligned( 64 ) ) )
scorep_compiler_region_description;

void
scorep_compiler_register_region( const scorep_compiler_region_description* region_descr );

#endif /* SCOREP_COMPILER_GCC_PLUGIN_H */
