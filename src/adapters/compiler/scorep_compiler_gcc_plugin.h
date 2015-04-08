/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 * @brief Contains defintion for
 * region descriptor struct for GCC plugin
 */


#ifndef SCOREP_COMPILER_GCC_PLUGIN_H_
#define SCOREP_COMPILER_GCC_PLUGIN_H_

#include <stdint.h>

/* avaiable flags */
#define SCOREP_COMPILER_REGION_FLAG_NO_CONDITION ( 1 << 0 )

typedef struct
{
    uint32_t*   handle;
    const char* name;
    const char* canonical_name;
    const char* file;
    int         begin_lno;
    int         end_lno;
    unsigned    flags;
}
__attribute__( ( aligned( 64 ) ) )
scorep_compiler_region_description;

void
scorep_compiler_register_region( const scorep_compiler_region_description* region_descr );

enum
{
    SCOREP_COMPILER_PHASE_PRE_INIT      = -1,
    SCOREP_COMPILER_PHASE_MEASUREMENT   = 0,
    SCOREP_COMPILER_PHASE_POST_FINALIZE = +1
};
extern int scorep_compiler_measurement_phase;

#endif /* SCOREP_COMPILER_GCC_PLUGIN_H_ */
