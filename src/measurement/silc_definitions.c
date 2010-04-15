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
 * @file       silc_definitions.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include "silc_definitions.h"

#include "silc_definition_structs.h"
#include "silc_runtime_management.h"
#include "silc_thread.h"
#include <SILC_Omp.h>
#include <SILC_PublicTypes.h>
#include <SILC_Timing.h>
#include <OTF2_DefWriter_inc.h>
#include <OTF2_DefWriter.h>
#include <stdbool.h>
#include <stdint.h>


SILC_DefinitionManager silc_definition_manager =
{
    { { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
    ,{ { SILC_MEMORY_MOVABLE_INVALID_PAGE_ID, SILC_MEMORY_MOVABLE_INVALID_OFFSET } }
};

static bool silc_definitions_initialized = false;


/* *INDENT-OFF* */
static OTF2_DefWriter* silc_create_definition_writer();
static uint64_t silc_on_definitions_post_flush();
static void silc_delete_definition_writer(OTF2_DefWriter* definitionWriter);
static void silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_callpath_parameter_integer_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_callpath_parameter_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_definitions(OTF2_DefWriter* definitionWriter);
static void silc_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_communicator_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
/* *INDENT-ON* */


void
SILC_Definitions_Initialize()
{
    if ( silc_definitions_initialized )
    {
        return;
    }
    silc_definitions_initialized = true;

    // Can't create definition writer here because otf location is
    // not known until SILC_InitMeasurementMPI
}


void
SILC_Definitions_Finalize()
{
    if ( !silc_definitions_initialized )
    {
        return;
    }
    silc_definitions_initialized = false;

    OTF2_DefWriter* definition_writer = silc_create_definition_writer();
    silc_write_definitions( definition_writer );
    silc_delete_definition_writer( definition_writer );
}


static OTF2_DefWriter*
silc_create_definition_writer()
{
    SILC_CreateExperimentDir();
    uint64_t              otf2_location = SILC_Thread_GetTraceLocationData(
        SILC_Thread_GetLocationData() )->otf_location;
    OTF2_PreFlushCallback pre_flush = 0;

    OTF2_DefWriter*       definition_writer =
        OTF2_DefWriter_New( SILC_GetExperimentDirName(),
                            "definitions",
                            otf2_location,
                            OTF2_SUBSTRATE_POSIX,
                            pre_flush,
                            silc_on_definitions_post_flush );
    assert( definition_writer );
    return definition_writer;
}


static uint64_t
silc_on_definitions_post_flush()
{
    return SILC_GetClockTicks();
}


static void
silc_write_definitions( OTF2_DefWriter* definitionWriter )
{
    silc_write_string_definitions_to_otf2( definitionWriter );
    silc_write_source_file_definitions_to_otf2( definitionWriter );
    silc_write_region_definitions_to_otf2( definitionWriter );
    silc_write_mpi_communicator_definitions_to_otf2( definitionWriter );
    silc_write_mpi_window_definitions_to_otf2( definitionWriter );
    silc_write_mpi_cartesian_topology_definitions_to_otf2( definitionWriter );
    silc_write_mpi_cartesian_coords_definitions_to_otf2( definitionWriter );
    silc_write_counter_group_definitions_to_otf2( definitionWriter );
    silc_write_counter_definitions_to_otf2( definitionWriter );
    silc_write_io_file_group_definitions_to_otf2( definitionWriter );
    silc_write_io_file_definitions_to_otf2( definitionWriter );
    silc_write_marker_group_definitions_to_otf2( definitionWriter );
    silc_write_marker_definitions_to_otf2( definitionWriter );
    silc_write_parameter_definitions_to_otf2( definitionWriter );
    silc_write_callpath_definitions_to_otf2( definitionWriter );
    silc_write_callpath_parameter_integer_definitions_to_otf2( definitionWriter );
    silc_write_callpath_parameter_string_definitions_to_otf2( definitionWriter );
}


static void
silc_delete_definition_writer( OTF2_DefWriter* definitionWriter )
{
    SILC_Error_Code status = OTF2_DefWriter_Delete( definitionWriter );
    if ( status != SILC_SUCCESS )
    {
        assert( 0 );
    }
}


SILC_StringHandle
SILC_DefineString( const char* str )
{
    static uint32_t                 counter        = 0;
    SILC_String_Definition*         new_definition = 0;
    SILC_String_Definition_Movable* new_movable    = 0;

    #pragma omp critical (define_string)
    {
        SILC_ALLOC_NEW_DEFINITION( SILC_String_Definition );
        SILC_DEFINITIONS_LIST_PUSH_FRONT( string_definitions_head_dummy );
        // init new_definition
        new_definition->id = counter++;
        SILC_Memory_AllocForDefinitionsRaw( strlen( str ) + 1,
                                            ( SILC_Allocator_MoveableMemory* )&( new_definition->str ) );
        strcpy( SILC_MEMORY_DEREF_MOVABLE( &( new_definition->str ), char* ), str );
    }

    return new_movable;
}


SILC_CallpathHandle
SILC_DefineCallpath( SILC_CallpathHandle parent,
                     SILC_RegionHandle   region )
{
    static uint32_t                   counter        = 0;
    SILC_Callpath_Definition*         new_definition = 0;
    SILC_Callpath_Definition_Movable* new_movable    = 0;
    SILC_ALLOC_NEW_DEFINITION( SILC_Callpath_Definition );
    SILC_DEFINITIONS_LIST_PUSH_FRONT( callpath_definitions_head_dummy );
    // Init new_definition
    new_definition->id = counter++;
    return new_movable;
}


SILC_CallpathParameterIntegerHandle
SILC_DefineCallpathParameterInteger( SILC_CallpathHandle  parent,
                                     SILC_ParameterHandle param,
                                     int64_t              value )
{
    static uint32_t                                   counter        = 0;
    SILC_CallpathParameterInteger_Definition*         new_definition = 0;
    SILC_CallpathParameterInteger_Definition_Movable* new_movable    = 0;
    SILC_ALLOC_NEW_DEFINITION( SILC_CallpathParameterInteger_Definition );
    SILC_DEFINITIONS_LIST_PUSH_FRONT( callpath_parameter_integer_definitions_head_dummy );
    // Init new_definition
    new_definition->id = counter++;
    return new_movable;
}


SILC_CallpathParameterStringHandle
SILC_DefineCallpathParameterString( SILC_CallpathHandle  parent,
                                    SILC_ParameterHandle param,
                                    SILC_StringHandle    value )
{
    static uint32_t                                  counter        = 0;
    SILC_CallpathParameterString_Definition*         new_definition = 0;
    SILC_CallpathParameterString_Definition_Movable* new_movable    = 0;
    SILC_ALLOC_NEW_DEFINITION( SILC_CallpathParameterString_Definition );
    SILC_DEFINITIONS_LIST_PUSH_FRONT( callpath_parameter_string_definitions_head_dummy );
    // Init new_definition
    new_definition->id = counter++;
    return new_movable;
}


static void
silc_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_String_Definition* definition =
        &( silc_definition_manager.string_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_String_Definition* );
        OTF2_DefWriter_DefString( definitionWriter,
                                  definition->id,
                                  SILC_MEMORY_DEREF_MOVABLE( &( definition->str ), char* ) );
    }
}


static void
silc_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_SourceFile_Definition* definition =
        &( silc_definition_manager.source_file_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_SourceFile_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Region_Definition* definition =
        &( silc_definition_manager.region_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_Region_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_communicator_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPICommunicator_Definition* definition =
        &( silc_definition_manager.mpi_communicator_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_MPICommunicator_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPIWindow_Definition* definition =
        &( silc_definition_manager.mpi_window_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_MPIWindow_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPICartesianTopology_Definition* definition =
        &( silc_definition_manager.mpi_cartesian_topology_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_MPICartesianTopology_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPICartesianCoords_Definition* definition =
        &( silc_definition_manager.mpi_cartesian_coords_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_MPICartesianCoords_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_CounterGroup_Definition* definition =
        &( silc_definition_manager.counter_group_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_CounterGroup_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Counter_Definition* definition =
        &( silc_definition_manager.counter_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_Counter_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_IOFileGroup_Definition* definition =
        &( silc_definition_manager.io_file_group_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_IOFileGroup_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_IOFile_Definition* definition =
        &( silc_definition_manager.io_file_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_IOFile_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MarkerGroup_Definition* definition =
        &( silc_definition_manager.marker_group_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_MarkerGroup_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Marker_Definition* definition =
        &( silc_definition_manager.marker_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_Marker_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Parameter_Definition* definition =
        &( silc_definition_manager.parameter_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_Parameter_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Callpath_Definition* definition =
        &( silc_definition_manager.callpath_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_Callpath_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_callpath_parameter_integer_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_CallpathParameterInteger_Definition* definition =
        &( silc_definition_manager.callpath_parameter_integer_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_CallpathParameterInteger_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_callpath_parameter_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_CallpathParameterString_Definition* definition =
        &( silc_definition_manager.callpath_parameter_string_definitions_head_dummy );
    while ( !SILC_MEMORY_MOVABLE_IS_NULL( definition->next ) )
    {
        definition = SILC_MEMORY_DEREF_MOVABLE( &( definition->next ),
                                                SILC_CallpathParameterString_Definition* );
        //OTF2_DefWriter_Def...(definitionWriter, ...);
        assert( false ); // implement me
    }
}
