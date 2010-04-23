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
#define SILC_ALLOC_NEW_DEFINITION( type ) \
    do { \
        new_movable = ( SILC_ ## type ## _Definition_Movable* ) \
            SILC_Memory_AllocForDefinitions( \
                sizeof( SILC_ ## type ## _Definition ) ); \
        new_definition = \
            SILC_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SILC_ ## type ## _Definition* ); \
        *silc_definition_manager. SILC_ ## type ## _Definition_tail_pointer = \
            *new_movable; \
        silc_definition_manager.SILC_ ## type ## _Definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->id = \
            silc_definition_manager.SILC_ ## type ## _Definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */

/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
typedef struct SILC_DefinitionManager SILC_DefinitionManager;
struct SILC_DefinitionManager
{
    SILC_String_Definition_Movable                SILC_String_Definition_head;
    SILC_String_Definition_Movable*               SILC_String_Definition_tail_pointer;
    uint32_t                                      SILC_String_Definition_counter;

    SILC_SourceFile_Definition_Movable            SILC_SourceFile_Definition_head;
    SILC_SourceFile_Definition_Movable*           SILC_SourceFile_Definition_tail_pointer;
    uint32_t                                      SILC_SourceFile_Definition_counter;

    SILC_Region_Definition_Movable                SILC_Region_Definition_head;
    SILC_Region_Definition_Movable*               SILC_Region_Definition_tail_pointer;
    uint32_t                                      SILC_Region_Definition_counter;

    SILC_MPICommunicator_Definition_Movable       SILC_MPICommunicator_Definition_head;
    SILC_MPICommunicator_Definition_Movable*      SILC_MPICommunicator_Definition_tail_pointer;
    uint32_t                                      SILC_MPICommunicator_Definition_counter;

    SILC_MPIWindow_Definition_Movable             SILC_MPIWindow_Definition_head;
    SILC_MPIWindow_Definition_Movable*            SILC_MPIWindow_Definition_tail_pointer;
    uint32_t                                      SILC_MPIWindow_Definition_counter;

    SILC_MPICartesianTopology_Definition_Movable  SILC_MPICartesianTopology_Definition_head;
    SILC_MPICartesianTopology_Definition_Movable* SILC_MPICartesianTopology_Definition_tail_pointer;
    uint32_t                                      SILC_MPICartesianTopology_Definition_counter;

    SILC_MPICartesianCoords_Definition_Movable    SILC_MPICartesianCoords_Definition_head;
    SILC_MPICartesianCoords_Definition_Movable*   SILC_MPICartesianCoords_Definition_tail_pointer;
    uint32_t                                      SILC_MPICartesianCoords_Definition_counter;

    SILC_CounterGroup_Definition_Movable          SILC_CounterGroup_Definition_head;
    SILC_CounterGroup_Definition_Movable*         SILC_CounterGroup_Definition_tail_pointer;
    uint32_t                                      SILC_CounterGroup_Definition_counter;

    SILC_Counter_Definition_Movable               SILC_Counter_Definition_head;
    SILC_Counter_Definition_Movable*              SILC_Counter_Definition_tail_pointer;
    uint32_t                                      SILC_Counter_Definition_counter;

    SILC_IOFileGroup_Definition_Movable           SILC_IOFileGroup_Definition_head;
    SILC_IOFileGroup_Definition_Movable*          SILC_IOFileGroup_Definition_tail_pointer;
    uint32_t                                      SILC_IOFileGroup_Definition_counter;

    SILC_IOFile_Definition_Movable                SILC_IOFile_Definition_head;
    SILC_IOFile_Definition_Movable*               SILC_IOFile_Definition_tail_pointer;
    uint32_t                                      SILC_IOFile_Definition_counter;

    SILC_MarkerGroup_Definition_Movable           SILC_MarkerGroup_Definition_head;
    SILC_MarkerGroup_Definition_Movable*          SILC_MarkerGroup_Definition_tail_pointer;
    uint32_t                                      SILC_MarkerGroup_Definition_counter;

    SILC_Marker_Definition_Movable                SILC_Marker_Definition_head;
    SILC_Marker_Definition_Movable*               SILC_Marker_Definition_tail_pointer;
    uint32_t                                      SILC_Marker_Definition_counter;

    SILC_Parameter_Definition_Movable             SILC_Parameter_Definition_head;
    SILC_Parameter_Definition_Movable*            SILC_Parameter_Definition_tail_pointer;
    uint32_t                                      SILC_Parameter_Definition_counter;

    SILC_Callpath_Definition_Movable              SILC_Callpath_Definition_head;
    SILC_Callpath_Definition_Movable*             SILC_Callpath_Definition_tail_pointer;
    uint32_t                                      SILC_Callpath_Definition_counter;
};

/**
 * Iterator functions for definition.
 *
 * Example:
 * @code
 *  SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, String )
 *  {
 *      :
 *      :
 *  }
 *  SILC_DEFINITION_FOREACH_WHILE();
 * @endcode
 */
/* *INDENT-OFF* */
#define SILC_DEFINITION_FOREACH_DO( manager_pointer, type ) \
    do { \
        SILC_ ## type ## _Definition * definition; \
        SILC_ ## type ## _Definition_Movable moveable; \
        for ( moveable = ( manager_pointer )->SILC_ ## type ## _Definition_head; \
              !SILC_ALLOCATOR_MOVABLE_IS_NULL( moveable ); \
              moveable = definition->next ) \
        { \
            definition = \
                SILC_MEMORY_DEREF_MOVABLE( &moveable, \
                                           SILC_ ## type ## _Definition* ); \
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
