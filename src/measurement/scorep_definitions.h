/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#define SCOREP_INTERNAL_DEFINITIONS_H


/**
 * @file       scorep_definitions.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_PublicTypes.h>
#include "scorep_definition_structs.h"
#include <SCOREP_Memory.h>

#include <jenkins_hash.h>


/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type
 */
/* *INDENT-OFF* */
#define SCOREP_ALLOC_NEW_DEFINITION( Type, type ) \
    do { \
        new_handle = SCOREP_Memory_AllocForDefinitions( \
            sizeof( SCOREP_ ## Type ## _Definition ) ); \
        new_definition = \
            SCOREP_MEMORY_DEREF_MOVABLE( new_handle, \
                                       SCOREP_ ## Type ## _Definition* ); \
        new_definition->next = SCOREP_MOVABLE_NULL; \
        new_definition->hash_value = 0; \
        *scorep_definition_manager.type ## _definition_tail_pointer = \
            new_handle; \
        scorep_definition_manager.type ## _definition_tail_pointer = \
            &new_definition->next; \
        new_definition->sequence_number = \
            scorep_definition_manager.type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */

/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type with a variable array member of type @array_type
 * and a total number of members of @number_of_members
 */
/* *INDENT-OFF* */
#define SCOREP_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( Type, \
                                                  type, \
                                                  array_type, \
                                                  number_of_members ) \
    do { \
        new_handle = SCOREP_Memory_AllocForDefinitions( \
            sizeof( SCOREP_ ## Type ## _Definition ) + \
            ( ( number_of_members ) - 1 ) * sizeof( array_type ) ); \
        new_definition = \
            SCOREP_MEMORY_DEREF_MOVABLE( new_handle, \
                                       SCOREP_ ## Type ## _Definition* ); \
        new_definition->next = SCOREP_MOVABLE_NULL; \
        new_definition->hash_value = 0; \
        *scorep_definition_manager.type ## _definition_tail_pointer = \
            new_handle; \
        scorep_definition_manager.type ## _definition_tail_pointer = \
            &new_definition->next; \
        new_definition->sequence_number = \
            scorep_definition_manager.type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */

/* this size is temporary */
#define SCOREP_DEFINITION_HASH_TABLE_SIZE hashsize( 8 )
#define SCOREP_DEFINITION_HASH_TABLE_MASK hashmask( 8 )

/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
/* *INDENT-OFF* */
typedef struct SCOREP_DefinitionManager SCOREP_DefinitionManager;
struct SCOREP_DefinitionManager
{
    /* note: no ';' */
    #define SCOREP_DEFINE_DEFINITION_LIST( Type, type ) \
        SCOREP_ ## Type ## Handle  type ## _definition_head; \
        SCOREP_ ## Type ## Handle* type ## _definition_tail_pointer; \
        uint32_t                              type ## _definition_counter;

    SCOREP_DEFINE_DEFINITION_LIST( String, string )
    SCOREP_StringHandle* string_definition_hash_table;

    SCOREP_DEFINE_DEFINITION_LIST( Location, location )
    SCOREP_DEFINE_DEFINITION_LIST( SourceFile, source_file )
    SCOREP_DEFINE_DEFINITION_LIST( Region, region )
    SCOREP_DEFINE_DEFINITION_LIST( Group, group )
    SCOREP_DEFINE_DEFINITION_LIST( MPIWindow, mpi_window )
    SCOREP_DEFINE_DEFINITION_LIST( MPICartesianTopology, mpi_cartesian_topology )
    SCOREP_DEFINE_DEFINITION_LIST( MPICartesianCoords, mpi_cartesian_coords )
    SCOREP_DEFINE_DEFINITION_LIST( CounterGroup, counter_group )
    SCOREP_DEFINE_DEFINITION_LIST( Counter, counter )
    SCOREP_DEFINE_DEFINITION_LIST( IOFileGroup, io_file_group )
    SCOREP_DEFINE_DEFINITION_LIST( IOFile, io_file )
    SCOREP_DEFINE_DEFINITION_LIST( MarkerGroup, marker_group )
    SCOREP_DEFINE_DEFINITION_LIST( Marker, marker )
    SCOREP_DEFINE_DEFINITION_LIST( Parameter, parameter )
    SCOREP_DEFINE_DEFINITION_LIST( Callpath, callpath )

    #undef SCOREP_DEFINE_DEFINITION_LIST
};
/* *INDENT-ON* */

/**
 * Iterator functions for definition. The iterator variable is named
 * @definition.
 *
 * Example:
 * @code
 *  SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, String, string )
 *  {
 *      :
 *      definition->member = ...
 *      :
 *  }
 *  SCOREP_DEFINITION_FOREACH_WHILE();
 * @endcode
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITION_FOREACH_DO( manager_pointer, Type, type ) \
    do { \
        SCOREP_ ## Type ## _Definition* definition; \
        SCOREP_ ## Type ## Handle handle; \
        for ( handle = ( manager_pointer )->type ## _definition_head; \
              handle != SCOREP_MOVABLE_NULL; \
              handle = definition->next ) \
        { \
            definition = SCOREP_MEMORY_DEREF_MOVABLE( \
                handle, SCOREP_ ## Type ## _Definition* ); \
            {

#define SCOREP_DEFINITION_FOREACH_WHILE() \
            } \
        } \
    } while ( 0 )
/* *INDENT-ON* */


/* Some convenient macros to add members or sub-hashes to the hash value */
#define HASH_ADD_POD( pod_member ) \
    new_definition->hash_value = hash( &new_definition->pod_member, \
                                       sizeof( new_definition->pod_member ), \
                                       new_definition->hash_value )

#define HASH_ADD_HANDLE( handle_member, Type ) \
    new_definition->hash_value = hashword( \
        &SCOREP_HANDLE_GET_HASH( new_definition->handle_member, Type ), \
        1, new_definition->hash_value )

#define HASH_ADD_ARRAY( array_member, number_member ) \
    new_definition->hash_value = hash( \
        new_definition->array_member, \
        sizeof( new_definition->array_member[ 0 ] ) \
        * new_definition->number_member, \
        new_definition->hash_value )


void
SCOREP_Definitions_Initialize();


void
SCOREP_Definitions_Finalize();


void
SCOREP_Definitions_Write();


SCOREP_StringHandle
SCOREP_DefineString( const char* str );


SCOREP_LocationHandle
SCOREP_DefineLocation( uint64_t    globalLocationId,
                       const char* name );


SCOREP_CallpathHandle
SCOREP_DefineCallpath( SCOREP_CallpathHandle parent,
                       SCOREP_RegionHandle   region );


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterInteger( SCOREP_CallpathHandle  parent,
                                       SCOREP_ParameterHandle param,
                                       int64_t                value );


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterString( SCOREP_CallpathHandle  parent,
                                      SCOREP_ParameterHandle param,
                                      SCOREP_StringHandle    value );


int
SCOREP_GetNumberOfDefinitions();

int
SCOREP_GetNumberOfRegionDefinitions();

int
SCOREP_GetNumberOfCounterDefinitions();

int
SCOREP_CallPathHandleToRegionID( SCOREP_CallpathHandle handle );

int
SCOREP_GetHandleToID( SCOREP_RegionHandle handle );

#endif /* SCOREP_INTERNAL_DEFINITIONS_H */
