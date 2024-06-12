/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013, 2015-2016,
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
 * @brief Contains definition for
 * region descriptor struct for GCC and LLVM plugin
 */


#ifndef SCOREP_COMPILER_PLUGIN_H
#define SCOREP_COMPILER_PLUGIN_H

#include <stdint.h>
#include <scorep/SCOREP_PublicTypes.h>

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
scorep_compiler_plugin_register_region( const scorep_compiler_region_description* region_descr );

//TODO!: Make these only available if xray is used?
// XRay plugin manually instruments each function at runtime instead of using start and end pointers
void scorep_plugin_register_region( const scorep_compiler_region_description* regionDescr );
void scorep_plugin_enter_region( SCOREP_RegionHandle regionHandle );
void scorep_plugin_exit_region( SCOREP_RegionHandle regionHandle );

#endif /* SCOREP_COMPILER_PLUGIN_H */
