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
 * Copyright (c) 2009-2014,
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
 * Defines helper functions for managing region data.
 */



#ifndef SCOREP_COMPILER_DATA_H_
#define SCOREP_COMPILER_DATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <UTILS_CStr.h>
#include <SCOREP_Types.h>

/**
   Returns the region handle for the given the region name. If no entry with
   the requested name exists, it returns SCOREP_INVALID_REGION_HANDLE.
   In multithreaded environments, calls to this function do not need
   to be locked if it is guaranteed that initialization is already finished and
   finalization will not start before this function returns.
   @param region_string  the region string as passed to the compiler adapter.
   @returns the hash_node for the given key.
 */
SCOREP_RegionHandle
scorep_compiler_hash_get( const char* region_string );

/**
   Frees the memory allocated for the hash table.
 */
void
scorep_compiler_hash_free( void );

/**
   Initializes the hash table.
 */
void
scorep_compiler_hash_init( void );

/**
   Registers a region to the SCOREP measurement system and creates a new entry
   in the hash table.
   @param region_string  the region string as passed to the compiler adapter.
   @returns the region handle of the newly registered region. If the region
            is filtered, it returns SCOREP_FILTERED_REGION.
 */
SCOREP_RegionHandle
scorep_compiler_register_region( const char* region_string );

#endif /* SCOREP_COMPILER_DATA_H_ */
