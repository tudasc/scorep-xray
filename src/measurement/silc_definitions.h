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


#define SILC_ALLOC_NEW_DEFINITION( DefinitionType )                             \
    new_movable = ( DefinitionType ## _Movable* )                               \
                  SILC_Memory_AllocForDefinitions( sizeof( DefinitionType ) );  \
    new_definition = SILC_MEMORY_DEREF_MOVABLE( new_movable, DefinitionType* );


#define SILC_DEFINITIONS_LIST_PUSH_FRONT( ListHeadDummy )                                    \
    new_definition->next                       = silc_definition_manager.ListHeadDummy.next; \
    silc_definition_manager.ListHeadDummy.next = *new_movable;


typedef struct SILC_DefinitionManager SILC_DefinitionManager;
struct SILC_DefinitionManager
{
    SILC_String_Definition               string_definitions_head_dummy;
    SILC_SourceFile_Definition           source_file_definitions_head_dummy;
    SILC_Region_Definition               region_definitions_head_dummy;
    SILC_MPICommunicator_Definition      mpi_communicator_definitions_head_dummy;
    SILC_MPIWindow_Definition            mpi_window_definitions_head_dummy;
    SILC_MPICartesianTopology_Definition mpi_cartesian_topology_definitions_head_dummy;
    SILC_MPICartesianCoords_Definition   mpi_cartesian_coords_definitions_head_dummy;
    SILC_CounterGroup_Definition         counter_group_definitions_head_dummy;
    SILC_Counter_Definition              counter_definitions_head_dummy;
    SILC_IOFileGroup_Definition          io_file_group_definitions_head_dummy;
    SILC_IOFile_Definition               io_file_definitions_head_dummy;
    SILC_MarkerGroup_Definition          marker_group_definitions_head_dummy;
    SILC_Marker_Definition               marker_definitions_head_dummy;
    SILC_Parameter_Definition            parameter_definitions_head_dummy;
    SILC_Callpath_Definition             callpath_definitions_head_dummy;
};


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
