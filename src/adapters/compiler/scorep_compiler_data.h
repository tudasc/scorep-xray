/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 * Contains helper functions which are common for all compiler adapters.
 */



#ifndef SCOREP_COMPILER_DATA_H
#define SCOREP_COMPILER_DATA_H

#include <stdint.h>

#include <SCOREP_Types.h>

/**
   @def SCOREP_COMPILER_HASH_MAX The number of slots in the region hash table.
 */
#define SCOREP_COMPILER_REGION_SLOTS 1021

/**
   @def SCOREP_COMPILER_FILE_SLOTS The number of slots in the file hash table.
 */
#define SCOREP_COMPILER_FILE_SLOTS 15

/**
 * @brief Hash table to map function addresses to region identifier
 * identifier is called region handle
 *
 * @param key                   hash key (address of function)
 * @param region_name_mangled   associated function name (use it for mangled name)
 * @param region_name_demangled associated function name (use it for demangled name)
 * @param file_name             file name
 * @param line_no_begin         line number of begin of function
 * @param line_no_end           line number of end of function
 * @param region_handle         region identifier
 * @param next                  pointer to next element with the same hash value.
 */
typedef struct scorep_compiler_hash_node
{
    uint64_t                          key;
    char*                             region_name_mangled;
    char*                             region_name_demangled;
    char*                             file_name;
    SCOREP_LineNo                     line_no_begin;
    SCOREP_LineNo                     line_no_end;
    SCOREP_RegionHandle               region_handle;
    struct scorep_compiler_hash_node* next;
} scorep_compiler_hash_node;

/**
   Returns the hash_node for the given key. If no node with the requested key exists,
   it returns NULL. In multithreaded environments, calls to this function do not need
   to be locked if it is guaranteed that initialization is already finished and
   finalization will not start before this function returns.
   @param key The key value.
   @returns the hash_node for the given key.
 */
extern scorep_compiler_hash_node*
scorep_compiler_hash_get( uint64_t key );

/**
   Creates a new entry for the region hashtable with the given values.
   @param key                   The key under which the new entry is stored.
   @param region_name_mangled   The mangled name of the region.
   @param region_name_demangled The demangled name of the region.
   @param file_name             The name of the source file of the registered region.
   @param line_no_begin         The source code line number where the region starts.
   @returns a pointer to the newly created hash node.
 */
extern scorep_compiler_hash_node*
scorep_compiler_hash_put( uint64_t      key,
                          const char*   region_name_mangled,
                          const char*   region_name_demangled,
                          const char*   file_name,
                          SCOREP_LineNo line_no_begin );

/**
   Frees the memory allocated for the hash table.
 */
extern void
scorep_compiler_hash_free( void );

/**
   Initializes the hash table.
 */
extern void
scorep_compiler_hash_init( void );

/**
   Registers a region to the SCOREP measurement system from data of a hash node.
   @param node A pointer to a hash node which contains the region data for the
               region to be registered to the SCOREP measurement system.
 */
extern void
scorep_compiler_register_region( scorep_compiler_hash_node* node );


/**
 * In debug mode SCOREP_DEBUG_COMPILER, calculate and print hash table
 * statistics like number of elements and collisions and fill rate.
 */
extern void
scorep_compiler_get_hash_statistics( void );

#endif /* SCOREP_COMPILER_DATA_H */
