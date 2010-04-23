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


SILC_DefinitionManager silc_definition_manager;
static bool            silc_definitions_initialized = false;


/* *INDENT-OFF* */
static OTF2_DefWriter* silc_create_definition_writer();
static uint64_t silc_on_definitions_post_flush();
static void silc_delete_definition_writer(OTF2_DefWriter* definitionWriter);
static void silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
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
static void silc_handle_definition_writing_error( SILC_Error_Code status, const char* definitionType );
static OTF2_RegionType silc_region_type_to_otf_region_type( SILC_RegionType silcType );
/* *INDENT-ON* */


void
SILC_Definitions_Initialize()
{
    if ( silc_definitions_initialized )
    {
        return;
    }
    silc_definitions_initialized = true;

    memset( &silc_definition_manager, 0, sizeof( silc_definition_manager ) );

    #define INIT_DEF( type ) \
    do { \
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( \
            silc_definition_manager.SILC_ ## type ## _Definition_head ); \
        silc_definition_manager.SILC_ ## type ## _Definition_tail_pointer = \
            &silc_definition_manager.SILC_ ## type ## _Definition_head; \
    } while ( 0 )

    INIT_DEF( String );
    INIT_DEF( SourceFile );
    INIT_DEF( Region );
    INIT_DEF( MPICommunicator );
    INIT_DEF( MPIWindow );
    INIT_DEF( MPICartesianTopology );
    INIT_DEF( MPICartesianCoords );
    INIT_DEF( CounterGroup );
    INIT_DEF( Counter );
    INIT_DEF( IOFileGroup );
    INIT_DEF( IOFile );
    INIT_DEF( MarkerGroup );
    INIT_DEF( Marker );
    INIT_DEF( Parameter );
    INIT_DEF( Callpath );

    #undef INIT_DEF

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
    SILC_String_Definition*         new_definition = NULL;
    SILC_String_Definition_Movable* new_movable    = NULL;

    #pragma omp critical (define_string)
    {
        SILC_ALLOC_NEW_DEFINITION( String );

        SILC_Memory_AllocForDefinitionsRaw( strlen( str ) + 1,
                                            ( SILC_Allocator_MovableMemory* )&new_definition->str );
        strcpy( SILC_MEMORY_DEREF_MOVABLE( &new_definition->str, char* ), str );
    }

    return new_movable;
}


SILC_CallpathHandle
SILC_DefineCallpath( SILC_CallpathHandle parent,
                     SILC_RegionHandle   region )
{
    SILC_Callpath_Definition*         new_definition = NULL;
    SILC_Callpath_Definition_Movable* new_movable    = NULL;

    SILC_ALLOC_NEW_DEFINITION( Callpath );

    // Init new_definition

    return new_movable;
}


SILC_CallpathHandle
SILC_DefineCallpathParameterInteger( SILC_CallpathHandle  parent,
                                     SILC_ParameterHandle param,
                                     int64_t              value )
{
    SILC_Callpath_Definition*         new_definition = NULL;
    SILC_Callpath_Definition_Movable* new_movable    = NULL;

    SILC_ALLOC_NEW_DEFINITION( Callpath );

    // Init new_definition

    return new_movable;
}


SILC_CallpathHandle
SILC_DefineCallpathParameterString( SILC_CallpathHandle  parent,
                                    SILC_ParameterHandle param,
                                    SILC_StringHandle    value )
{
    SILC_Callpath_Definition*         new_definition = NULL;
    SILC_Callpath_Definition_Movable* new_movable    = NULL;

    SILC_ALLOC_NEW_DEFINITION( Callpath );

    // Init new_definition

    return new_movable;
}


static void
silc_handle_definition_writing_error( SILC_Error_Code status,
                                      const char*     definitionType )
{
    assert( false ); // implement me
}


static OTF2_RegionType
silc_region_type_to_otf_region_type( SILC_RegionType silcType )
{
    /* see SILC_Types.h
       SILC_REGION_UNKNOWN = 0,
       SILC_REGION_FUNCTION,
       SILC_REGION_LOOP,
       SILC_REGION_USER,
       SILC_REGION_PHASE,
       SILC_REGION_DYNAMIC,

       SILC_REGION_DYNAMIC_PHASE,
       SILC_REGION_DYNAMIC_LOOP,
       SILC_REGION_DYNAMIC_FUNCTION,
       SILC_REGION_DYNAMIC_LOOP_PHASE,

       SILC_REGION_MPI_COLL_BARRIER,
       SILC_REGION_MPI_COLL_ONE2ALL,
       SILC_REGION_MPI_COLL_ALL2ONE,
       SILC_REGION_MPI_COLL_ALL2ALL,
       SILC_REGION_MPI_COLL_OTHER,

       SILC_REGION_OMP_PARALLEL,
       SILC_REGION_OMP_LOOP,
       SILC_REGION_OMP_SECTIONS,
       SILC_REGION_OMP_SECTION,
       SILC_REGION_OMP_WORKSHARE,
       SILC_REGION_OMP_SINGLE,
       SILC_REGION_OMP_MASTER,
       SILC_REGION_OMP_CRITICAL,
       SILC_REGION_OMP_ATOMIC,
       SILC_REGION_OMP_BARRIER,
       SILC_REGION_OMP_IMPLICIT_BARRIER,
       SILC_REGION_OMP_FLUSH,
       SILC_REGION_OMP_CRITICAL_SBLOCK, /// @todo what is SBLOCK?
       SILC_REGION_OMP_SINGLE_SBLOCK,
       SILC_REGION_OMP_WRAPPER,

       SILC_INVALID_REGION_TYPE
     */

    /* see Records/OTF2_LocalDefinitions.c */

    static OTF2_RegionType type_map[ SILC_INVALID_REGION_TYPE ] = {
        OTF2_REGION_TYPE_UNKNOWN,
        OTF2_REGION_TYPE_FUNCTION,
        OTF2_REGION_TYPE_LOOP,
        OTF2_REGION_TYPE_USER_REGION,
        OTF2_REGION_PHASE,
        OTF2_REGION_DYNAMIC,
        OTF2_REGION_DYNAMIC_PHASE,
        OTF2_REGION_DYNAMIC_LOOP,
        OTF2_REGION_DYNAMIC_FUNCTION,
        OTF2_REGION_DYNAMIC_LOOP_PHASE,
        OTF2_REGION_TYPE_MPI_COLL_BARRIER,
        OTF2_REGION_TYPE_MPI_COLL_ONE2ALL,
        OTF2_REGION_TYPE_MPI_COLL_ALL2ONE,
        OTF2_REGION_TYPE_MPI_COLL_ALL2ALL,
        OTF2_REGION_TYPE_MPI_COLL_OTHER,
        OTF2_REGION_TYPE_OMP_PARALLEL,
        OTF2_REGION_TYPE_OMP_LOOP,
        OTF2_REGION_TYPE_OMP_SECTIONS,
        OTF2_REGION_TYPE_OMP_SECTION,
        OTF2_REGION_TYPE_OMP_WORKSHARE,
        OTF2_REGION_TYPE_OMP_SINGLE,
        OTF2_REGION_TYPE_OMP_MASTER,
        OTF2_REGION_TYPE_OMP_CRITICAL,
        OTF2_REGION_TYPE_OMP_ATOMIC,
        OTF2_REGION_TYPE_OMP_BARRIER,
        OTF2_REGION_TYPE_OMP_IBARRIER,
        OTF2_REGION_TYPE_OMP_FLUSH,
        OTF2_REGION_TYPE_OMP_CRITICAL_SBLOCK,
        OTF2_REGION_TYPE_OMP_SINGLE_SBLOCK,
        OTF2_REGION_TYPE_OMP_WRAPPER,
    };

    return type_map[ silcType ];
}

static void
silc_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, String )
    {
        SILC_Error_Code status = OTF2_DefWriter_DefString(
            definitionWriter,
            definition->id,
            SILC_MEMORY_DEREF_MOVABLE( &( definition->str ), char* ) );
        if ( status != SILC_SUCCESS )
        {
            silc_handle_definition_writing_error( status, "SILC_String_Definition" );
        }
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    // nothing to be done here
    // SourceFile_Definitions are only available as String_Definition in
    // OTF2, and the string itself for this definition was already passed to
    // OTF2
}


static void
silc_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Region )
    {
        uint32_t descriptionId = 0;
        if ( !SILC_ALLOCATOR_MOVABLE_IS_NULL( definition->description_handle ) )
        {
            descriptionId = ( SILC_MEMORY_DEREF_MOVABLE(
                                  &( definition->description_handle ), SILC_String_Definition* )
                              )->id;
        }

        SILC_Error_Code status = OTF2_DefWriter_DefRegion(
            definitionWriter,
            definition->id,
            ( SILC_MEMORY_DEREF_MOVABLE( &( definition->name_handle ),
                                         SILC_String_Definition* ) )->id,
            descriptionId,
            silc_region_type_to_otf_region_type( definition->region_type ),
            ( SILC_MEMORY_DEREF_MOVABLE( &( definition->file_handle ),
                                         SILC_String_Definition* ) )->id,
            definition->begin_line,
            definition->end_line );
        if ( status != SILC_SUCCESS )
        {
            silc_handle_definition_writing_error( status, "SILC_Region_Definition" );
        }
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_mpi_communicator_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, MPICommunicator )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MPICommunicator_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, MPIWindow )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MPIWindow_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, MPICartesianTopology )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MPICartesianTopology_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, MPICartesianCoords )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MPICartesianCoords_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, CounterGroup )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_CounterGroup_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Counter )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Counter_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, IOFileGroup )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_IOFileGroup_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, IOFile )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_IOFile_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, MarkerGroup )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MarkerGroup_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Marker )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Marker_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Parameter )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Parameter_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Callpath )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Callpath_Definition" );
        //}
        assert( false ); // implement me
    }
    SILC_DEFINITION_FOREACH_WHILE();
}
