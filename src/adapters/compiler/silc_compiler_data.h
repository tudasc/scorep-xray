/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       silc_compiler_data.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status     ALPHA
 *
 * Contains helper functions which are common for all compiler adapters.
 */



#ifndef SILC_COMPILER_DATA_H_
#define SILC_COMPILER_DATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SILC_Utils.h>
#include "SILC_Types.h"
#include "SILC_Error.h"

/**
   @def SILC_COMPILER_HASH_MAX The number of slots in the region hash table.
 */
#define SILC_COMPILER_REGION_SLOTS 1021

/**
   @def SILC_COMPILER_FILE_SLOTS The number of slots in the file hash table.
 */
#define SILC_COMPILER_FILE_SLOTS 15

/**
 * @brief Hash table to map function addresses to region identifier
 * identifier is called region handle
 *
 * @param key           hash key (address of function)
 * @param region_name   associated function name
 * @param file_name     file name
 * @param line_no_begin line number of begin of function

 * @param line_no_end   line number of end of function
 * @param region_handle region identifier
 * @param next          pointer to next element with the same hash value.
 */
typedef struct silc_compiler_hash_node
{
    long                            key;
    char*                           region_name;
    char*                           file_name;
    SILC_LineNo                     line_no_begin;
    SILC_LineNo                     line_no_end;
    SILC_RegionHandle               region_handle;
    struct silc_compiler_hash_node* next;
} silc_compiler_hash_node;

/**
   Returns a the hash_node for a given key. If no node with the requested key exists,
   it returns NULL.
   @param key The key value.
 */
extern silc_compiler_hash_node*
silc_compiler_hash_get( long key );

/**
   Creates a new entry for the region hashtable with the given values.
   @param key           The key under which the new entry is stored.
   @param region_name   The name of the region.
   @param file_name     The name of the source file of the registered region.
   @param line_no_begin The source code line number where the region starts.
   @returns a pointer to the newly created hash node.
 */
extern silc_compiler_hash_node*
silc_compiler_hash_put( long        key,
                        const char* region_name,
                        const char* file_name,
                        int         line_no_begin );

/**
   Frees the memory allocated for the hash table.
 */
extern void
silc_compiler_hash_free();

/**
   Initializes the hash table.
 */
extern void
silc_compiler_hash_init();

/**
   Registers a region to the SILC measurement system from data of a hash node.
   @param node A pointer to a hash node which contains the region data for the
               region to be registered ot the SILC measurement system.
 */
extern void
silc_compiler_register_region( silc_compiler_hash_node* node );

/**
   Initialize the file table.
 */
void
silc_compiler_init_file_table();

/**
   Finalize the file table
 */
void
silc_compiler_final_file_table();

/**
   Returns the file handle for a given file name. It searches in the hash table if the
   requested name is already there and returns the stored value. If the file name is not
   found in the hash table, it creates a bew entry and registers the file to the SILC
   measurement system.
   @param file The file name fr which the handle is returned.
   @returns the handle for the @a file.
 */
SILC_SourceFileHandle
silc_compiler_get_file( const char* file );



#endif /* SILC_COMPILER_DATA_H_ */
