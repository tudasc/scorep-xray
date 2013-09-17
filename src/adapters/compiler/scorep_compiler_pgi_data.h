/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
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
 * Contains helper data structures which are used by the PGI compiler adapter.
 */


#ifndef SCOREP_COMPILER_PGI_DATA_
#define SCOREP_COMPILER_PGI_DATA_

#include <stdint.h>

#include <SCOREP_Definitions.h>


/**
    Defines the maximum size of a callstack.
 */
#define CALLSTACK_MAX 128

/**
   Contains the callstack data for each location.
 */
typedef struct
{
    /**
        Pointer to the callstack starting position.
     */
    SCOREP_RegionHandle callstack_base[ CALLSTACK_MAX ];

    /**
        Counts the current level of nesting.
     */
    uint32_t callstack_count;

    /**
        Current callstack position.
     */
    uint32_t callstack_top;
} pgi_location_data;


#endif /* SCOREP_COMPILER_PGI_DATA_ */
