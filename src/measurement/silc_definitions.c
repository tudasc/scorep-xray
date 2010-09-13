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


#include <config.h>


#include "silc_definitions.h"


#include "silc_definition_structs.h"
#include "silc_runtime_management.h"
#include "silc_thread.h"
#include <silc_utility/SILC_Omp.h>
#include <SILC_PublicTypes.h>
#include <SILC_Timing.h>
#include <otf2/otf2.h>
#include <stdbool.h>
#include <stdint.h>
#include <silc_utility/SILC_Debug.h>
#include "silc_mpi.h"
#include "silc_status.h"
#include <jenkins_hash.h>


SILC_DefinitionManager  silc_definition_manager;
SILC_DefinitionManager* silc_remote_definition_manager = 0;
static bool             silc_definitions_initialized   = false;


/* *INDENT-OFF* */
static void silc_write_definitions_to_otf2(void);
static OTF2_DefWriter* silc_create_definition_writer();
static OTF2_FlushType silc_on_definitions_pre_flush();
static void silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_location_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
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

    // note, only lower-case type needed
    #define SILC_INIT_DEFINITION_LIST( type ) \
    do { \
        silc_definition_manager.type ## _definition_tail_pointer = \
            &silc_definition_manager.type ## _definition_head; \
    } while ( 0 )

    SILC_INIT_DEFINITION_LIST( string );
    silc_definition_manager.string_definition_hash_table =
        calloc( SILC_DEFINITION_HASH_TABLE_SIZE,
                sizeof( *silc_definition_manager.string_definition_hash_table ) );
    assert( silc_definition_manager.string_definition_hash_table );

    SILC_INIT_DEFINITION_LIST( location );
    SILC_INIT_DEFINITION_LIST( source_file );
    SILC_INIT_DEFINITION_LIST( region );
    SILC_INIT_DEFINITION_LIST( group );
    SILC_INIT_DEFINITION_LIST( mpi_window );
    SILC_INIT_DEFINITION_LIST( mpi_cartesian_topology );
    SILC_INIT_DEFINITION_LIST( mpi_cartesian_coords );
    SILC_INIT_DEFINITION_LIST( counter_group );
    SILC_INIT_DEFINITION_LIST( counter );
    SILC_INIT_DEFINITION_LIST( io_file_group );
    SILC_INIT_DEFINITION_LIST( io_file );
    SILC_INIT_DEFINITION_LIST( marker_group );
    SILC_INIT_DEFINITION_LIST( marker );
    SILC_INIT_DEFINITION_LIST( parameter );
    SILC_INIT_DEFINITION_LIST( callpath );

    #undef SILC_INIT_DEFINITION_LIST

    // No need the create the definition writer, its only needed in the
    // finalization phase and will be created there.
}


void
SILC_Definitions_Finalize()
{
    if ( !silc_definitions_initialized )
    {
        return;
    }
    silc_definitions_initialized = false;

    free( silc_definition_manager.string_definition_hash_table );
}


void
SILC_Definitions_Write()
{
    if ( !silc_definitions_initialized )
    {
        return;
    }

    if ( SILC_IsTracingEnabled() )
    {
        silc_write_definitions_to_otf2();
    }
}


static OTF2_DefWriter*
silc_create_definition_writer()
{
    SILC_CreateExperimentDir();
    uint64_t otf2_location = SILC_Thread_GetTraceLocationData(
        SILC_Thread_GetLocationData() )->otf_location;

    OTF2_DefWriter* definition_writer =
        OTF2_Archive_GetDefWriter( silc_otf2_archive,
                                   otf2_location,
                                   silc_on_definitions_pre_flush,
                                   SILC_OnTraceAndDefinitionPostFlush );

    assert( definition_writer );
    return definition_writer;
}


OTF2_FlushType
silc_on_definitions_pre_flush()
{
    if ( !SILC_Mpi_IsInitialized )
    {
        // flush before MPI_Init, we are lost.
        assert( false );
    }
    // master/slave already set during initialization
    return OTF2_FLUSH;
}


static void
silc_write_definitions_to_otf2( void )
{
    OTF2_DefWriter* definition_writer = silc_create_definition_writer();

    silc_write_string_definitions_to_otf2( definition_writer );
    silc_write_location_definitions_to_otf2( definition_writer );
    silc_write_source_file_definitions_to_otf2( definition_writer );
    silc_write_region_definitions_to_otf2( definition_writer );
    silc_write_group_definitions_to_otf2( definition_writer );
    silc_write_mpi_window_definitions_to_otf2( definition_writer );
    silc_write_mpi_cartesian_topology_definitions_to_otf2( definition_writer );
    silc_write_mpi_cartesian_coords_definitions_to_otf2( definition_writer );
    silc_write_counter_group_definitions_to_otf2( definition_writer );
    silc_write_counter_definitions_to_otf2( definition_writer );
    silc_write_io_file_group_definitions_to_otf2( definition_writer );
    silc_write_io_file_definitions_to_otf2( definition_writer );
    silc_write_marker_group_definitions_to_otf2( definition_writer );
    silc_write_marker_definitions_to_otf2( definition_writer );
    silc_write_parameter_definitions_to_otf2( definition_writer );
    silc_write_callpath_definitions_to_otf2( definition_writer );
}


SILC_StringHandle
SILC_DefineString( const char* str )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new string \"%s\":", str );

    SILC_String_Definition* new_definition = NULL;
    SILC_StringHandle       new_handle     = SILC_INVALID_STRING;

    #pragma omp critical (define_string)
    {
        uint32_t string_length = strlen( str );
        uint32_t string_hash   = hash( str, string_length, 0 );

        SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "  Hash value for string %x", string_hash );

        /* get reference to table bucket for new string */
        SILC_StringHandle* hash_table_bucket =
            &silc_definition_manager.string_definition_hash_table[
                string_hash & SILC_DEFINITION_HASH_TABLE_MASK ];

        SILC_StringHandle hash_list_iterator = *hash_table_bucket;
        while ( hash_list_iterator != SILC_MOVABLE_NULL )
        {
            SILC_String_Definition* string_definition = SILC_HANDLE_DEREF(
                hash_list_iterator, String );

            SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
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
        if ( hash_list_iterator == SILC_MOVABLE_NULL )
        {
            SILC_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( String,
                                                      string,
                                                      char,
                                                      string_length + 1 );

            SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
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
            SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
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
SILC_LocationHandle
SILC_DefineLocation( uint64_t    globalLocationId,
                     const char* name )
{
    SILC_Location_Definition* new_definition = NULL;
    SILC_LocationHandle       new_handle     = SILC_INVALID_LOCATION;

    SILC_ALLOC_NEW_DEFINITION( Location, location );

    new_definition->global_location_id = globalLocationId;
    new_definition->name_handle        = SILC_DefineString( name );

    /* locations wont be unfied, therfore no hash value needed, yet? */

    /** @todo: this needs clarification after the location hierarchy
               has settled */
    new_definition->location_type = SILC_LOCATION_OMP_THREAD;

    return new_handle;
}


SILC_CallpathHandle
SILC_DefineCallpath( SILC_CallpathHandle parentCallpath,
                     SILC_RegionHandle   region )
{
    SILC_Callpath_Definition* new_definition = NULL;
    SILC_CallpathHandle       new_handle     = SILC_INVALID_CALLPATH;

    SILC_ALLOC_NEW_DEFINITION( Callpath, callpath );

    // Init new_definition
    new_definition->parent_callpath_handle = parentCallpath;
    if ( new_definition->parent_callpath_handle != SILC_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( parent_callpath_handle, Callpath );
    }

    new_definition->with_parameter = false;
    HASH_ADD_POD( with_parameter );

    new_definition->callpath_argument.region_handle = region;
    if ( new_definition->callpath_argument.region_handle
         != SILC_INVALID_REGION )
    {
        HASH_ADD_HANDLE( callpath_argument.region_handle, Region );
    }

    return new_handle;
}


SILC_CallpathHandle
SILC_DefineCallpathParameterInteger( SILC_CallpathHandle  parentCallpath,
                                     SILC_ParameterHandle callpathParameter,
                                     int64_t              integerValue )
{
    SILC_Callpath_Definition* new_definition = NULL;
    SILC_CallpathHandle       new_handle     = SILC_INVALID_CALLPATH;

    SILC_ALLOC_NEW_DEFINITION( Callpath, callpath );

    // Init new_definition
    new_definition->parent_callpath_handle = parentCallpath;
    if ( new_definition->parent_callpath_handle != SILC_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( parent_callpath_handle, Callpath );
    }

    new_definition->with_parameter = true;
    HASH_ADD_POD( with_parameter );

    new_definition->callpath_argument.parameter_handle = callpathParameter;
    if ( new_definition->callpath_argument.parameter_handle
         != SILC_INVALID_PARAMETER )
    {
        SILC_ParameterType parameter_type = SILC_HANDLE_DEREF(
            new_definition->callpath_argument.parameter_handle,
            Parameter )->parameter_type;
        assert( parameter_type == SILC_PARAMETER_INT64 );

        HASH_ADD_HANDLE( callpath_argument.parameter_handle, Parameter );

        new_definition->parameter_value.integer_value = integerValue;
        HASH_ADD_POD( parameter_value.integer_value );
    }

    return new_handle;
}


SILC_CallpathHandle
SILC_DefineCallpathParameterString( SILC_CallpathHandle  parentCallpath,
                                    SILC_ParameterHandle callpathParameter,
                                    SILC_StringHandle    stringValue )
{
    SILC_Callpath_Definition* new_definition = NULL;
    SILC_CallpathHandle       new_handle     = SILC_INVALID_CALLPATH;

    SILC_ALLOC_NEW_DEFINITION( Callpath, callpath );

    // Init new_definition
    new_definition->parent_callpath_handle = parentCallpath;
    if ( new_definition->parent_callpath_handle != SILC_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( parent_callpath_handle, Callpath );
    }

    new_definition->with_parameter = true;
    HASH_ADD_POD( with_parameter );

    new_definition->callpath_argument.parameter_handle = callpathParameter;
    if ( new_definition->callpath_argument.parameter_handle
         != SILC_INVALID_PARAMETER )
    {
        SILC_ParameterType type = SILC_HANDLE_DEREF(
            new_definition->callpath_argument.parameter_handle,
            Parameter )->parameter_type;
        assert( type == SILC_PARAMETER_STRING );

        HASH_ADD_HANDLE( callpath_argument.parameter_handle, Parameter );

        new_definition->parameter_value.string_handle = stringValue;
        if ( new_definition->parameter_value.string_handle
             != SILC_INVALID_STRING )
        {
            HASH_ADD_HANDLE( parameter_value.string_handle, String );
        }
    }

    return new_handle;
}


static void
silc_handle_definition_writing_error( SILC_Error_Code status,
                                      const char*     definitionType )
{
    assert( false ); // implement me
}


static OTF2_LocationType
silc_location_type_to_otf_location_type( SILC_LocationType silcType )
{
    /* see SILC_Types.h
       SILC_LOCATION_UNKNOWN = 0,
       SILC_LOCATION_OMP_THREAD,
     */

    static OTF2_LocationType type_map[ SILC_INVALID_LOCATION_TYPE ] = {
        OTF2_LOCATION_TYPE_UNKNOWN,
        OTF2_LOCATION_TYPE_THREAD,

        /* unused */
        /*
           OTF2_LOCATION_TYPE_MACHINE,
           OTF2_LOCATION_TYPE_NODE,
           OTF2_LOCATION_TYPE_PROCESS,
         */
    };

    return type_map[ silcType ];
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

static OTF2_GroupType
silc_group_type_to_otf_group_type( SILC_GroupType silcType )
{
    /* see SILC_Types.h
       SILC_GROUP_UNKNOWN      = 0,
       SILC_GROUP_LOCATIONS    = 1,
       SILC_GROUP_REGIONS      = 2,
       SILC_GROUP_COMMUNICATOR = 3,
       SILC_GROUP_METRIC       = 4,
     */

    static OTF2_GroupType type_map[ SILC_INVALID_GROUP_TYPE ] = {
        OTF2_GROUPTYPE_NON,
        OTF2_GROUPTYPE_LOCATIONS,
        OTF2_GROUPTYPE_REGIONS,
        OTF2_GROUPTYPE_COMMUNICATOR,
        OTF2_GROUPTYPE_METRIC,
    };

    return type_map[ silcType ];
}

static void
silc_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, String, string )
    {
        SILC_Error_Code status = OTF2_DefWriter_DefString(
            definitionWriter,
            definition->sequence_number,
            definition->string_data );
        if ( status != SILC_SUCCESS )
        {
            silc_handle_definition_writing_error( status, "SILC_String_Definition" );
        }
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_location_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Location, location )
    {
        /** @todo add definition count */
        SILC_Error_Code status = OTF2_DefWriter_DefLocation(
            definitionWriter,
            definition->global_location_id,
            SILC_HANDLE_TO_ID( definition->name_handle, String ),
            silc_location_type_to_otf_location_type( definition->location_type ),
            0 );
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
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Region, region )
    {
        uint32_t source_file_id = OTF2_UNDEFINED_UINT32;
        if ( definition->file_handle != SILC_INVALID_STRING )
        {
            source_file_id = SILC_HANDLE_TO_ID( definition->file_handle, String );
        }

        SILC_Error_Code status = OTF2_DefWriter_DefRegion(
            definitionWriter,
            definition->sequence_number,
            SILC_HANDLE_TO_ID( definition->name_handle, String ),
            SILC_HANDLE_TO_ID( definition->description_handle, String ),
            silc_region_type_to_otf_region_type( definition->region_type ),
            source_file_id,
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
silc_write_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, Group, group )
    {
        SILC_Error_Code status = OTF2_DefWriter_DefGroup(
            definitionWriter,
            definition->sequence_number,
            silc_group_type_to_otf_group_type( definition->group_type ),
            definition->number_of_members,
            definition->members );
        if ( status != SILC_SUCCESS )
        {
            silc_handle_definition_writing_error( status, "SILC_Group_Definition" );
        }
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                MPIWindow,
                                mpi_window )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MPIWindow_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                MPICartesianTopology,
                                mpi_cartesian_topology )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MPICartesianTopology_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                MPICartesianCoords,
                                mpi_cartesian_coords )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MPICartesianCoords_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                CounterGroup,
                                counter_group )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_CounterGroup_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                Counter,
                                counter )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Counter_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                IOFileGroup,
                                io_file_group )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_IOFileGroup_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                IOFile,
                                io_file )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_IOFile_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                MarkerGroup,
                                marker_group )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_MarkerGroup_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                Marker,
                                marker )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Marker_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                Parameter,
                                parameter )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Parameter_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


static void
silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_DEFINITION_FOREACH_DO( &silc_definition_manager,
                                Callpath,
                                callpath )
    {
        //SILC_Error_Code status = OTF2_DefWriter_Def...(definitionWriter, ...);
        //if ( status != SILC_SUCCESS )
        //{
        //    silc_handle_definition_writing_error( status, "SILC_Callpath_Definition" );
        //}
        SILC_DEBUG_PRINTF( 0, "Not yet implemented." );
    }
    SILC_DEFINITION_FOREACH_WHILE();
}


int
SILC_GetNumberOfDefinitions()
{
    assert( !omp_in_parallel() );

    int n_definitions = 0;
    n_definitions += silc_definition_manager.string_definition_counter;
    n_definitions += silc_definition_manager.location_definition_counter;
    n_definitions += silc_definition_manager.source_file_definition_counter;
    n_definitions += silc_definition_manager.region_definition_counter;
    n_definitions += silc_definition_manager.group_definition_counter;
    n_definitions += silc_definition_manager.mpi_window_definition_counter;
    n_definitions += silc_definition_manager.mpi_cartesian_topology_definition_counter;
    n_definitions += silc_definition_manager.mpi_cartesian_coords_definition_counter;
    n_definitions += silc_definition_manager.counter_group_definition_counter;
    n_definitions += silc_definition_manager.counter_definition_counter;
    n_definitions += silc_definition_manager.io_file_group_definition_counter;
    n_definitions += silc_definition_manager.io_file_definition_counter;
    n_definitions += silc_definition_manager.marker_group_definition_counter;
    n_definitions += silc_definition_manager.marker_definition_counter;
    n_definitions += silc_definition_manager.parameter_definition_counter;
    n_definitions += silc_definition_manager.callpath_definition_counter;

    return n_definitions;
}

int
SILC_GetNumberOfRegionDefinitions()
{
    assert( !omp_in_parallel() );
    return silc_definition_manager.region_definition_counter;
}

int
SILC_GetNumberOfCounterDefinitions()
{
    assert( !omp_in_parallel() );
    return silc_definition_manager.counter_definition_counter;
}

int
SILC_GetRegionHandleToID( SILC_RegionHandle handle )
{
    assert( !omp_in_parallel() );
    return SILC_HANDLE_TO_ID( handle, Region );
}

int
SILC_CallPathHandleToRegionID( SILC_CallpathHandle handle )
{
    SILC_Callpath_Definition* callpath = SILC_HANDLE_DEREF( handle, Callpath );

    return SILC_GetRegionHandleToID( callpath->callpath_argument.region_handle );
}
