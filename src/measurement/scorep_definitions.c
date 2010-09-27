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


/**
 * @file       scorep_definitions.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include "scorep_definitions.h"


#include "scorep_definition_structs.h"
#include "scorep_runtime_management.h"
#include "scorep_thread.h"
#include <scorep_utility/SCOREP_Omp.h>
#include <SCOREP_PublicTypes.h>
#include <SCOREP_Timing.h>
#include <otf2/otf2.h>
#include <stdbool.h>
#include <stdint.h>
#include <scorep_utility/SCOREP_Debug.h>
#include "scorep_mpi.h"
#include "scorep_status.h"
#include <jenkins_hash.h>


SCOREP_DefinitionManager  scorep_definition_manager;
SCOREP_DefinitionManager* scorep_remote_definition_manager = 0;
static bool               scorep_definitions_initialized   = false;


/* *INDENT-OFF* */
static void scorep_write_definitions_to_otf2(void);
static OTF2_DefWriter* scorep_create_definition_writer();
static OTF2_FlushType scorep_on_definitions_pre_flush();
static void scorep_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_write_location_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void scorep_handle_definition_writing_error( SCOREP_Error_Code status, const char* definitionType );
static OTF2_RegionType scorep_region_type_to_otf_region_type( SCOREP_RegionType scorepType );
/* *INDENT-ON* */


void
SCOREP_Definitions_Initialize()
{
    if ( scorep_definitions_initialized )
    {
        return;
    }
    scorep_definitions_initialized = true;

    memset( &scorep_definition_manager, 0, sizeof( scorep_definition_manager ) );

    // note, only lower-case type needed
    #define SCOREP_INIT_DEFINITION_LIST( type ) \
    do { \
        scorep_definition_manager.type ## _definition_tail_pointer = \
            &scorep_definition_manager.type ## _definition_head; \
    } while ( 0 )

    SCOREP_INIT_DEFINITION_LIST( string );
    scorep_definition_manager.string_definition_hash_table =
        calloc( SCOREP_DEFINITION_HASH_TABLE_SIZE,
                sizeof( *scorep_definition_manager.string_definition_hash_table ) );
    assert( scorep_definition_manager.string_definition_hash_table );

    SCOREP_INIT_DEFINITION_LIST( location );
    SCOREP_INIT_DEFINITION_LIST( source_file );
    SCOREP_INIT_DEFINITION_LIST( region );
    SCOREP_INIT_DEFINITION_LIST( group );
    SCOREP_INIT_DEFINITION_LIST( mpi_window );
    SCOREP_INIT_DEFINITION_LIST( mpi_cartesian_topology );
    SCOREP_INIT_DEFINITION_LIST( mpi_cartesian_coords );
    SCOREP_INIT_DEFINITION_LIST( counter_group );
    SCOREP_INIT_DEFINITION_LIST( counter );
    SCOREP_INIT_DEFINITION_LIST( io_file_group );
    SCOREP_INIT_DEFINITION_LIST( io_file );
    SCOREP_INIT_DEFINITION_LIST( marker_group );
    SCOREP_INIT_DEFINITION_LIST( marker );
    SCOREP_INIT_DEFINITION_LIST( parameter );
    SCOREP_INIT_DEFINITION_LIST( callpath );

    #undef SCOREP_INIT_DEFINITION_LIST

    // No need the create the definition writer, its only needed in the
    // finalization phase and will be created there.
}


void
SCOREP_Definitions_Finalize()
{
    if ( !scorep_definitions_initialized )
    {
        return;
    }
    scorep_definitions_initialized = false;

    free( scorep_definition_manager.string_definition_hash_table );
}


void
SCOREP_Definitions_Write()
{
    if ( !scorep_definitions_initialized )
    {
        return;
    }

    if ( SCOREP_IsTracingEnabled() )
    {
        scorep_write_definitions_to_otf2();
    }
}


static OTF2_DefWriter*
scorep_create_definition_writer()
{
    SCOREP_CreateExperimentDir();
    uint64_t otf2_location = SCOREP_Thread_GetTraceLocationData(
        SCOREP_Thread_GetLocationData() )->otf_location;

    OTF2_DefWriter* definition_writer =
        OTF2_Archive_GetDefWriter( scorep_otf2_archive,
                                   otf2_location,
                                   scorep_on_definitions_pre_flush,
                                   SCOREP_OnTraceAndDefinitionPostFlush );

    assert( definition_writer );
    return definition_writer;
}


OTF2_FlushType
scorep_on_definitions_pre_flush()
{
    if ( !SCOREP_Mpi_IsInitialized )
    {
        // flush before MPI_Init, we are lost.
        assert( false );
    }
    // master/slave already set during initialization
    return OTF2_FLUSH;
}


static void
scorep_write_definitions_to_otf2( void )
{
    OTF2_DefWriter* definition_writer = scorep_create_definition_writer();

    scorep_write_string_definitions_to_otf2( definition_writer );
    scorep_write_location_definitions_to_otf2( definition_writer );
    scorep_write_source_file_definitions_to_otf2( definition_writer );
    scorep_write_region_definitions_to_otf2( definition_writer );
    scorep_write_group_definitions_to_otf2( definition_writer );
    scorep_write_mpi_window_definitions_to_otf2( definition_writer );
    scorep_write_mpi_cartesian_topology_definitions_to_otf2( definition_writer );
    scorep_write_mpi_cartesian_coords_definitions_to_otf2( definition_writer );
    scorep_write_counter_group_definitions_to_otf2( definition_writer );
    scorep_write_counter_definitions_to_otf2( definition_writer );
    scorep_write_io_file_group_definitions_to_otf2( definition_writer );
    scorep_write_io_file_definitions_to_otf2( definition_writer );
    scorep_write_marker_group_definitions_to_otf2( definition_writer );
    scorep_write_marker_definitions_to_otf2( definition_writer );
    scorep_write_parameter_definitions_to_otf2( definition_writer );
    //scorep_write_callpath_definitions_to_otf2( definition_writer );
}


SCOREP_StringHandle
SCOREP_DefineString( const char* str )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new string \"%s\":", str );

    SCOREP_String_Definition* new_definition = NULL;
    SCOREP_StringHandle       new_handle     = SCOREP_INVALID_STRING;

    #pragma omp critical (define_string)
    {
        uint32_t string_length = strlen( str );
        uint32_t string_hash   = hash( str, string_length, 0 );

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                             "  Hash value for string %x", string_hash );

        /* get reference to table bucket for new string */
        SCOREP_StringHandle* hash_table_bucket =
            &scorep_definition_manager.string_definition_hash_table[
                string_hash & SCOREP_DEFINITION_HASH_TABLE_MASK ];

        SCOREP_StringHandle hash_list_iterator = *hash_table_bucket;
        while ( hash_list_iterator != SCOREP_MOVABLE_NULL )
        {
            SCOREP_String_Definition* string_definition = SCOREP_HANDLE_DEREF(
                hash_list_iterator, String );

            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                                 "  Comparing against: \"%s\" (#%x)",
                                 string_definition->string_data,
                                 string_definition->hash_value );

            if ( string_definition->hash_value == string_hash
                 && string_definition->string_length == string_length
                 && 0 == strcmp( string_definition->string_data, str ) )
            {
                break;
            }

            hash_list_iterator = string_definition->hash_next;
        }

        /* need to define new string  */
        if ( hash_list_iterator == SCOREP_MOVABLE_NULL )
        {
            SCOREP_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( String,
                                                        string,
                                                        char,
                                                        string_length + 1 );

            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                                 "  Allocate new string" );

            /*
             * string_length is a derived member, no need to add this to the
             * hash value
             */
            new_definition->string_length = string_length;

            /*
             * we know the length of the string already, therefore we can use
             * the faster memcpy
             */
            memcpy( new_definition->string_data, str, string_length + 1 );
            new_definition->hash_value = string_hash;

            /* link into hash chain */
            new_definition->hash_next = *hash_table_bucket;
            *hash_table_bucket        = new_handle;
        }
        else
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                                 "  Re-using string" );

            /* return found existing string */
            new_handle = hash_list_iterator;
        }
    }

    return new_handle;
}


/**
 * Registers a new local location into the definitions.
 *
 * @in internal
 */
SCOREP_LocationHandle
SCOREP_DefineLocation( uint64_t    globalLocationId,
                       const char* name )
{
    SCOREP_Location_Definition* new_definition = NULL;
    SCOREP_LocationHandle       new_handle     = SCOREP_INVALID_LOCATION;

    SCOREP_ALLOC_NEW_DEFINITION( Location, location );

    new_definition->global_location_id = globalLocationId;
    new_definition->name_handle        = SCOREP_DefineString( name );

    /* locations wont be unfied, therfore no hash value needed, yet? */

    /** @todo: this needs clarification after the location hierarchy
               has settled */
    new_definition->location_type = SCOREP_LOCATION_OMP_THREAD;

    return new_handle;
}


SCOREP_CallpathHandle
SCOREP_DefineCallpath( SCOREP_CallpathHandle parentCallpath,
                       SCOREP_RegionHandle   region )
{
    SCOREP_Callpath_Definition* new_definition = NULL;
    SCOREP_CallpathHandle       new_handle     = SCOREP_INVALID_CALLPATH;

    SCOREP_ALLOC_NEW_DEFINITION( Callpath, callpath );

    // Init new_definition
    new_definition->parent_callpath_handle = parentCallpath;
    if ( new_definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( parent_callpath_handle, Callpath );
    }

    new_definition->with_parameter = false;
    HASH_ADD_POD( with_parameter );

    new_definition->callpath_argument.region_handle = region;
    if ( new_definition->callpath_argument.region_handle
         != SCOREP_INVALID_REGION )
    {
        HASH_ADD_HANDLE( callpath_argument.region_handle, Region );
    }

    return new_handle;
}


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterInteger( SCOREP_CallpathHandle  parentCallpath,
                                       SCOREP_ParameterHandle callpathParameter,
                                       int64_t                integerValue )
{
    SCOREP_Callpath_Definition* new_definition = NULL;
    SCOREP_CallpathHandle       new_handle     = SCOREP_INVALID_CALLPATH;

    SCOREP_ALLOC_NEW_DEFINITION( Callpath, callpath );

    // Init new_definition
    new_definition->parent_callpath_handle = parentCallpath;
    if ( new_definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( parent_callpath_handle, Callpath );
    }

    new_definition->with_parameter = true;
    HASH_ADD_POD( with_parameter );

    new_definition->callpath_argument.parameter_handle = callpathParameter;
    if ( new_definition->callpath_argument.parameter_handle
         != SCOREP_INVALID_PARAMETER )
    {
        SCOREP_ParameterType parameter_type = SCOREP_HANDLE_DEREF(
            new_definition->callpath_argument.parameter_handle,
            Parameter )->parameter_type;
        assert( parameter_type == SCOREP_PARAMETER_INT64 );

        HASH_ADD_HANDLE( callpath_argument.parameter_handle, Parameter );

        new_definition->parameter_value.integer_value = integerValue;
        HASH_ADD_POD( parameter_value.integer_value );
    }

    return new_handle;
}


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterString( SCOREP_CallpathHandle  parentCallpath,
                                      SCOREP_ParameterHandle callpathParameter,
                                      SCOREP_StringHandle    stringValue )
{
    SCOREP_Callpath_Definition* new_definition = NULL;
    SCOREP_CallpathHandle       new_handle     = SCOREP_INVALID_CALLPATH;

    SCOREP_ALLOC_NEW_DEFINITION( Callpath, callpath );

    // Init new_definition
    new_definition->parent_callpath_handle = parentCallpath;
    if ( new_definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( parent_callpath_handle, Callpath );
    }

    new_definition->with_parameter = true;
    HASH_ADD_POD( with_parameter );

    new_definition->callpath_argument.parameter_handle = callpathParameter;
    if ( new_definition->callpath_argument.parameter_handle
         != SCOREP_INVALID_PARAMETER )
    {
        SCOREP_ParameterType type = SCOREP_HANDLE_DEREF(
            new_definition->callpath_argument.parameter_handle,
            Parameter )->parameter_type;
        assert( type == SCOREP_PARAMETER_STRING );

        HASH_ADD_HANDLE( callpath_argument.parameter_handle, Parameter );

        new_definition->parameter_value.string_handle = stringValue;
        if ( new_definition->parameter_value.string_handle
             != SCOREP_INVALID_STRING )
        {
            HASH_ADD_HANDLE( parameter_value.string_handle, String );
        }
    }

    return new_handle;
}


static void
scorep_handle_definition_writing_error( SCOREP_Error_Code status,
                                        const char*       definitionType )
{
    assert( false ); // implement me
}


static OTF2_LocationType
scorep_location_type_to_otf_location_type( SCOREP_LocationType scorepType )
{
    /* see SCOREP_Types.h
       SCOREP_LOCATION_UNKNOWN = 0,
       SCOREP_LOCATION_OMP_THREAD,
     */

    static OTF2_LocationType type_map[ SCOREP_INVALID_LOCATION_TYPE ] = {
        OTF2_LOCATION_TYPE_UNKNOWN,
        OTF2_LOCATION_TYPE_THREAD,

        /* unused */
        /*
           OTF2_LOCATION_TYPE_MACHINE,
           OTF2_LOCATION_TYPE_NODE,
           OTF2_LOCATION_TYPE_PROCESS,
         */
    };

    return type_map[ scorepType ];
}

static OTF2_RegionType
scorep_region_type_to_otf_region_type( SCOREP_RegionType scorepType )
{
    /* see SCOREP_Types.h
       SCOREP_REGION_UNKNOWN = 0,
       SCOREP_REGION_FUNCTION,
       SCOREP_REGION_LOOP,
       SCOREP_REGION_USER,
       SCOREP_REGION_PHASE,
       SCOREP_REGION_DYNAMIC,

       SCOREP_REGION_DYNAMIC_PHASE,
       SCOREP_REGION_DYNAMIC_LOOP,
       SCOREP_REGION_DYNAMIC_FUNCTION,
       SCOREP_REGION_DYNAMIC_LOOP_PHASE,

       SCOREP_REGION_MPI_COLL_BARRIER,
       SCOREP_REGION_MPI_COLL_ONE2ALL,
       SCOREP_REGION_MPI_COLL_ALL2ONE,
       SCOREP_REGION_MPI_COLL_ALL2ALL,
       SCOREP_REGION_MPI_COLL_OTHER,

       SCOREP_REGION_OMP_PARALLEL,
       SCOREP_REGION_OMP_LOOP,
       SCOREP_REGION_OMP_SECTIONS,
       SCOREP_REGION_OMP_SECTION,
       SCOREP_REGION_OMP_WORKSHARE,
       SCOREP_REGION_OMP_SINGLE,
       SCOREP_REGION_OMP_MASTER,
       SCOREP_REGION_OMP_CRITICAL,
       SCOREP_REGION_OMP_ATOMIC,
       SCOREP_REGION_OMP_BARRIER,
       SCOREP_REGION_OMP_IMPLICIT_BARRIER,
       SCOREP_REGION_OMP_FLUSH,
       SCOREP_REGION_OMP_CRITICAL_SBLOCK, /// @todo what is SBLOCK?
       SCOREP_REGION_OMP_SINGLE_SBLOCK,
       SCOREP_REGION_OMP_WRAPPER,

       SCOREP_INVALID_REGION_TYPE
     */

    /* see Records/OTF2_LocalDefinitions.c */

    static OTF2_RegionType type_map[ SCOREP_INVALID_REGION_TYPE ] = {
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

    return type_map[ scorepType ];
}

static OTF2_GroupType
scorep_group_type_to_otf_group_type( SCOREP_GroupType scorepType )
{
    /* see SCOREP_Types.h
       SCOREP_GROUP_UNKNOWN      = 0,
       SCOREP_GROUP_LOCATIONS    = 1,
       SCOREP_GROUP_REGIONS      = 2,
       SCOREP_GROUP_COMMUNICATOR = 3,
       SCOREP_GROUP_METRIC       = 4,
     */

    static OTF2_GroupType type_map[ SCOREP_INVALID_GROUP_TYPE ] = {
        OTF2_GROUPTYPE_NON,
        OTF2_GROUPTYPE_LOCATIONS,
        OTF2_GROUPTYPE_REGIONS,
        OTF2_GROUPTYPE_COMMUNICATOR,
        OTF2_GROUPTYPE_METRIC,
    };

    return type_map[ scorepType ];
}

static void
scorep_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, String, string )
    {
        SCOREP_Error_Code status = OTF2_DefWriter_DefString(
            definitionWriter,
            definition->sequence_number,
            definition->string_data );
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_String_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_location_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, Location, location )
    {
        /** @todo add definition count */
        SCOREP_Error_Code status = OTF2_DefWriter_DefLocation(
            definitionWriter,
            definition->global_location_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String ),
            scorep_location_type_to_otf_location_type( definition->location_type ),
            0 );
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_String_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    // nothing to be done here
    // SourceFile_Definitions are only available as String_Definition in
    // OTF2, and the string itself for this definition was already passed to
    // OTF2
}


static void
scorep_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, Region, region )
    {
        uint32_t source_file_id = OTF2_UNDEFINED_UINT32;
        if ( definition->file_handle != SCOREP_INVALID_STRING )
        {
            source_file_id = SCOREP_HANDLE_TO_ID( definition->file_handle, String );
        }

        SCOREP_Error_Code status = OTF2_DefWriter_DefRegion(
            definitionWriter,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String ),
            SCOREP_HANDLE_TO_ID( definition->description_handle, String ),
            scorep_region_type_to_otf_region_type( definition->region_type ),
            source_file_id,
            definition->begin_line,
            definition->end_line );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Region_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, Group, group )
    {
        SCOREP_Error_Code status = OTF2_DefWriter_DefGroup(
            definitionWriter,
            definition->sequence_number,
            scorep_group_type_to_otf_group_type( definition->group_type ),
            definition->number_of_members,
            definition->members );
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Group_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  MPIWindow,
                                  mpi_window )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MPIWindow_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  MPICartesianTopology,
                                  mpi_cartesian_topology )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MPICartesianTopology_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  MPICartesianCoords,
                                  mpi_cartesian_coords )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MPICartesianCoords_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  CounterGroup,
                                  counter_group )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_CounterGroup_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  Counter,
                                  counter )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_Counter_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  IOFileGroup,
                                  io_file_group )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_IOFileGroup_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  IOFile,
                                  io_file )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_IOFile_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  MarkerGroup,
                                  marker_group )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MarkerGroup_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  Marker,
                                  marker )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_Marker_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  Parameter,
                                  parameter )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_Parameter_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager,
                                  Callpath,
                                  callpath )
    {
        //SCOREP_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_Callpath_Definition" );
        //}
        SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


int
SCOREP_GetNumberOfDefinitions()
{
    assert( !omp_in_parallel() );

    int n_definitions = 0;
    n_definitions += scorep_definition_manager.string_definition_counter;
    n_definitions += scorep_definition_manager.location_definition_counter;
    n_definitions += scorep_definition_manager.source_file_definition_counter;
    n_definitions += scorep_definition_manager.region_definition_counter;
    n_definitions += scorep_definition_manager.group_definition_counter;
    n_definitions += scorep_definition_manager.mpi_window_definition_counter;
    n_definitions += scorep_definition_manager.mpi_cartesian_topology_definition_counter;
    n_definitions += scorep_definition_manager.mpi_cartesian_coords_definition_counter;
    n_definitions += scorep_definition_manager.counter_group_definition_counter;
    n_definitions += scorep_definition_manager.counter_definition_counter;
    n_definitions += scorep_definition_manager.io_file_group_definition_counter;
    n_definitions += scorep_definition_manager.io_file_definition_counter;
    n_definitions += scorep_definition_manager.marker_group_definition_counter;
    n_definitions += scorep_definition_manager.marker_definition_counter;
    n_definitions += scorep_definition_manager.parameter_definition_counter;
    n_definitions += scorep_definition_manager.callpath_definition_counter;

    return n_definitions;
}

int
SCOREP_GetNumberOfRegionDefinitions()
{
    assert( !omp_in_parallel() );
    return scorep_definition_manager.region_definition_counter;
}

int
SCOREP_GetNumberOfCounterDefinitions()
{
    assert( !omp_in_parallel() );
    return scorep_definition_manager.counter_definition_counter;
}

int
SCOREP_GetRegionHandleToID( SCOREP_RegionHandle handle )
{
    assert( !omp_in_parallel() );
    return SCOREP_HANDLE_TO_ID( handle, Region );
}

int
SCOREP_CallPathHandleToRegionID( SCOREP_CallpathHandle handle )
{
    SCOREP_Callpath_Definition* callpath = SCOREP_HANDLE_DEREF( handle, Callpath );

    return SCOREP_GetRegionHandleToID( callpath->callpath_argument.region_handle );
}
