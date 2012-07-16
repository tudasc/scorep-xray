/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       scorep_compiler_data.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status     ALPHA
 *
 * Contains helper functions which are common for all compiler adapters.
 */



#ifndef SCOREP_COMPILER_DATA_H_
#define SCOREP_COMPILER_DATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SCOREP_Debug.h>
#include <SCOREP_IO.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_CStr.h>
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
scorep_compiler_hash_free();

/**
   Initializes the hash table.
 */
extern void
scorep_compiler_hash_init();

/**
   Registers a region to the SCOREP measurement system from data of a hash node.
   @param node A pointer to a hash node which contains the region data for the
               region to be registered ot the SCOREP measurement system.
 */
extern void
scorep_compiler_register_region( scorep_compiler_hash_node* node );

/**
   Initialize the file table.
 */
void
scorep_compiler_init_file_table();

/**
   Finalize the file table
 */
void
scorep_compiler_finalize_file_table();

/**
   Returns the file handle for a given file name. It searches in the hash table if the
   requested name is already there and returns the stored value. If the file name is not
   found in the hash table, it creates a bew entry and registers the file to the SCOREP
   measurement system.
   @param file The file name fr which the handle is returned.
   @returns the handle for the @a file.
 */
SCOREP_SourceFileHandle
scorep_compiler_get_file( const char* file );



#endif /* SCOREP_COMPILER_DATA_H_ */
