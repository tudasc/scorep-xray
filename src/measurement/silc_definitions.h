#ifndef SILC_INTERNAL_DEFINITIONS_H
#define SILC_INTERNAL_DEFINITIONS_H

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
 * @file       silc_definitions.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <SILC_DefinitionHandles.h>
#include <SILC_PublicTypes.h>
#include "silc_definition_structs.h"
#include <SILC_Memory.h>


/**
 * Allocate, assign id, and store in manager list a new definition of type
 * @type
 */
/* *INDENT-OFF* */
#define SILC_ALLOC_NEW_DEFINITION( Type, type ) \
    do { \
        new_movable = ( SILC_ ## Type ## _Definition_Movable* ) \
            SILC_Memory_AllocForDefinitions( \
                sizeof( SILC_ ## Type ## _Definition ) ); \
        new_definition = \
            SILC_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SILC_ ## Type ## _Definition* ); \
        *silc_definition_manager.type ## _definition_tail_pointer = \
            *new_movable; \
        silc_definition_manager.type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->id = \
            silc_definition_manager.type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */

/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
/* *INDENT-OFF* */
typedef struct SILC_DefinitionManager SILC_DefinitionManager;
struct SILC_DefinitionManager
{
    // note: no ';'
    #define SILC_DEFINE_DEFINITION_LIST( counter_type, Type, type ) \
        SILC_ ## Type ## _Definition_Movable type ## _definition_head; \
        SILC_ ## Type ## _Definition_Movable* type ## _definition_tail_pointer; \
        counter_type type ## _definition_counter;

    SILC_DEFINE_DEFINITION_LIST( uint32_t, String, string )

    /* The counter will not be used to assign ids to locations, only
     * to count the number of locations.
     */
    SILC_DEFINE_DEFINITION_LIST( uint64_t, Location, location )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, SourceFile, source_file )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, Region, region )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, MPICommunicator, mpi_communicator )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, MPIWindow, mpi_window )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, MPICartesianTopology, mpi_cartesian_topology )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, MPICartesianCoords, mpi_cartesian_coords )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, CounterGroup, counter_group )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, Counter, counter )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, IOFileGroup, io_file_group )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, IOFile, io_file )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, MarkerGroup, marker_group )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, Marker, marker )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, Parameter, parameter )
    SILC_DEFINE_DEFINITION_LIST( uint32_t, Callpath, callpath )

    #undef SILC_DEFINE_DEFINITION_LIST
};
/* *INDENT-ON* */

/**
 * Iterator functions for definition. The iterator variable is named
 * @definition.
 *
 * Example:
 * @code
 *  SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, String, string )
 *  {
 *      :
 *      definition->member = ...
 *      :
 *  }
 *  SILC_DEFINITION_FOREACH_WHILE();
 * @endcode
 */
/* *INDENT-OFF* */
#define SILC_DEFINITION_FOREACH_DO( manager_pointer, Type, type ) \
    do { \
        SILC_ ## Type ## _Definition *definition; \
        SILC_ ## Type ## _Definition_Movable moveable; \
        for ( moveable = ( manager_pointer )->type ## _definition_head; \
              !SILC_ALLOCATOR_MOVABLE_IS_NULL( moveable ); \
              moveable = definition->next ) \
        { \
            definition = \
                SILC_MEMORY_DEREF_MOVABLE( &moveable, \
                                           SILC_ ## Type ## _Definition* ); \
            {

#define SILC_DEFINITION_FOREACH_WHILE() \
            } \
        } \
    } while ( 0 )
/* *INDENT-ON* */


void
SILC_Definitions_Initialize();


void
SILC_Definitions_Finalize();


SILC_StringHandle
SILC_DefineString( const char* str );


SILC_LocationHandle
SILC_DefineLocation( uint64_t    locationId,
                     const char* name );


SILC_CallpathHandle
SILC_DefineCallpath( SILC_CallpathHandle parent,
                     SILC_RegionHandle   region );


SILC_CallpathHandle
SILC_DefineCallpathParameterInteger( SILC_CallpathHandle  parent,
                                     SILC_ParameterHandle param,
                                     int64_t              value );


SILC_CallpathHandle
SILC_DefineCallpathParameterString( SILC_CallpathHandle  parent,
                                    SILC_ParameterHandle param,
                                    SILC_StringHandle    value );



#endif /* SILC_INTERNAL_DEFINITIONS_H */
