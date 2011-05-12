/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @status      alpha
 * @file        src/measurement/tracing/scorep_tracing_definitions.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Handles tracing specific definitions tasks.
 *
 *
 */


#include <config.h>


#include <stdbool.h>
#include <stdint.h>


#include <otf2/otf2.h>


#include <scorep_utility/SCOREP_Debug.h>


#include <SCOREP_PublicTypes.h>
#include <scorep_runtime_management.h>
#include <scorep_environment.h>
#include <scorep_mpi.h>

#include <scorep_definitions.h>
#include <scorep_definition_structs.h>
#include <scorep_definition_macros.h>


extern SCOREP_DefinitionManager  scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;


static void
scorep_handle_definition_writing_error( SCOREP_Error_Code status,
                                        const char*       definitionType )
{
    assert( false ); // implement me
}


static OTF2_LocationType
scorep_location_type_to_otf_location_type( SCOREP_LocationType scorepType,
                                           bool                isGlobal )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_LOCATION_TYPE_ ## SCOREP: \
        return isGlobal \
               ? OTF2_GLOB_LOCATION_TYPE_ ## OTF2 \
               : OTF2_LOCATION_TYPE_ ## OTF2

        case_return( CPU_THREAD, CPU_THREAD );
        case_return( GPU, GPU );
        case_return( METRIC, METRIC );

#undef case_return
        default:
            assert( !"Invalid location type" );
    }
}

static OTF2_LocationGroupType
scorep_location_group_type_to_otf_location_group_type( SCOREP_LocationType scorepType,
                                                       bool                isGlobal )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_LOCATION_GROUP_TYPE_ ## SCOREP: \
        return isGlobal \
               ? OTF2_GLOB_LOCATION_GROUP_TYPE_ ## OTF2 \
               : OTF2_LOCATION_GROUP_TYPE_ ## OTF2

        case_return( PROCESS, PROCESS );

#undef case_return
        default:
            assert( !"Invalid location group type" );
    }
}

static OTF2_RegionType
scorep_region_type_to_otf_region_type( SCOREP_RegionType scorepType,
                                       bool              isGlobal )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_REGION_ ## SCOREP: \
        return isGlobal \
               ? OTF2_GLOB_REGION_ ## OTF2 \
               : OTF2_REGION_ ## OTF2

        case_return( UNKNOWN,              TYPE_UNKNOWN );
        case_return( FUNCTION,             TYPE_FUNCTION );
        case_return( LOOP,                 TYPE_LOOP );
        case_return( USER,                 TYPE_USER_REGION );
        case_return( PHASE,                PHASE );
        case_return( DYNAMIC,              DYNAMIC );
        case_return( DYNAMIC_PHASE,        DYNAMIC_PHASE );
        case_return( DYNAMIC_LOOP,         DYNAMIC_LOOP );
        case_return( DYNAMIC_FUNCTION,     DYNAMIC_FUNCTION );
        case_return( DYNAMIC_LOOP_PHASE,   DYNAMIC_LOOP_PHASE );
        case_return( MPI_COLL_BARRIER,     TYPE_MPI_COLL_BARRIER );
        case_return( MPI_COLL_ONE2ALL,     TYPE_MPI_COLL_ONE2ALL );
        case_return( MPI_COLL_ALL2ONE,     TYPE_MPI_COLL_ALL2ONE );
        case_return( MPI_COLL_ALL2ALL,     TYPE_MPI_COLL_ALL2ALL );
        case_return( MPI_COLL_OTHER,       TYPE_MPI_COLL_OTHER );
        case_return( OMP_PARALLEL,         TYPE_OMP_PARALLEL );
        case_return( OMP_LOOP,             TYPE_OMP_LOOP );
        case_return( OMP_SECTIONS,         TYPE_OMP_SECTIONS );
        case_return( OMP_SECTION,          TYPE_OMP_SECTION );
        case_return( OMP_WORKSHARE,        TYPE_OMP_WORKSHARE );
        case_return( OMP_SINGLE,           TYPE_OMP_SINGLE );
        case_return( OMP_MASTER,           TYPE_OMP_MASTER );
        case_return( OMP_CRITICAL,         TYPE_OMP_CRITICAL );
        case_return( OMP_ATOMIC,           TYPE_OMP_ATOMIC );
        case_return( OMP_BARRIER,          TYPE_OMP_BARRIER );
        case_return( OMP_IMPLICIT_BARRIER, TYPE_OMP_IBARRIER );
        case_return( OMP_FLUSH,            TYPE_OMP_FLUSH );
        case_return( OMP_CRITICAL_SBLOCK,  TYPE_OMP_CRITICAL_SBLOCK );
        case_return( OMP_SINGLE_SBLOCK,    TYPE_OMP_SINGLE_SBLOCK );
        case_return( OMP_WRAPPER,          TYPE_OMP_WRAPPER );

#undef case_return
        default:
            assert( !"Invalid location type" );
    }
}

static OTF2_GroupType
scorep_group_type_to_otf_group_type( SCOREP_GroupType scorepType,
                                     bool             isGlobal )
{
    switch ( scorepType )
    {
#define case_return( SCOREP, OTF2 ) \
    case SCOREP_GROUP_ ## SCOREP: \
        return isGlobal \
               ? OTF2_GLOB_GROUPTYPE_ ## OTF2 \
               : OTF2_GROUPTYPE_ ## OTF2

        case_return( UNKNOWN,       NON );
        case_return( LOCATIONS,     LOCATIONS );
        case_return( REGIONS,       REGIONS );
        case_return( METRIC,        METRIC );
        case_return( COMM_SELF,     COMMUNICATOR_SELF );
        case_return( MPI_GROUP,     MPI_GROUP );
        case_return( MPI_LOCATIONS, MPI_LOCATIONS );

#undef case_return
        default:
            assert( !"Invalid group type" );
    }
}

static void
scorep_write_string_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defString )( void*,
                                       uint32_t,
                                       char* ) =
        ( void* )OTF2_DefWriter_DefString;
    if ( isGlobal )
    {
        defString = ( void* )OTF2_GlobDefWriter_GlobDefString;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, String, string )
    {
        SCOREP_Error_Code status = defString(
            writerHandle,
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
scorep_write_location_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defLocation )( void*,
                                         uint64_t,
                                         uint32_t,
                                         OTF2_LocationType,
                                         uint64_t,
                                         uint64_t,
                                         uint64_t,
                                         uint64_t ) =
        ( void* )OTF2_DefWriter_DefLocation;
    if ( isGlobal )
    {
        defLocation = ( void* )OTF2_GlobDefWriter_GlobDefLocation;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, Location, location )
    {
        SCOREP_Error_Code status = defLocation(
            writerHandle,
            definition->global_location_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_location_type_to_otf_location_type( definition->location_type, isGlobal ),
            definition->number_of_events,
            definition->number_of_definitions,
            definition->timer_resolution,
            definition->location_group_id );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Location_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

static void
scorep_write_location_group_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defLocationGroup )( void*,
                                              uint64_t,
                                              uint32_t,
                                              OTF2_LocationGroupType,
                                              uint32_t ) =
        ( void* )OTF2_DefWriter_DefLocationGroup;
    if ( isGlobal )
    {
        defLocationGroup = ( void* )OTF2_GlobDefWriter_GlobDefLocationGroup;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, LocationGroup, location_group )
    {
        SCOREP_Error_Code status = defLocationGroup(
            writerHandle,
            definition->global_location_group_id,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            scorep_location_group_type_to_otf_location_group_type( definition->location_group_type, isGlobal ),
            SCOREP_HANDLE_TO_ID( definition->parent, SystemTreeNode, definitionManager->page_manager ) );
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_LocationGroup_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

static void
scorep_write_system_tree_node_definitions(
    void*                     writerHandle,
    SCOREP_DefinitionManager* definitionManager,
    bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defSystemTreeNode )( void*,
                                               uint32_t,
                                               uint32_t,
                                               uint32_t,
                                               uint32_t ) =
        ( void* )OTF2_DefWriter_DefSystemTreeNode;
    if ( isGlobal )
    {
        defSystemTreeNode = ( void* )OTF2_GlobDefWriter_GlobDefSystemTreeNode;
    }


    SCOREP_DEFINITION_FOREACH_DO( definitionManager, SystemTreeNode, system_tree_node )
    {
        /* Determine parent id savely */
        uint64_t parent = 0;
        if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            parent = SCOREP_HANDLE_TO_ID( definition->parent_handle, SystemTreeNode, definitionManager->page_manager );
        }

        /* Write defintion */
        SCOREP_Error_Code status = defSystemTreeNode(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->class_handle, String, definitionManager->page_manager ),
            parent );
        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_SystemTreeNode_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_source_file_definitions( void*                     writerHandle,
                                      SCOREP_DefinitionManager* definitionManager,
                                      bool                      isGlobal )
{
    // nothing to be done here
    // SourceFile_Definitions are only available as String_Definition in
    // OTF2, and the string itself for this definition was already passed to
    // OTF2
}


static void
scorep_write_region_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defRegion )( void*,
                                       uint32_t,
                                       uint32_t,
                                       uint32_t,
                                       OTF2_RegionType,
                                       uint32_t,
                                       uint32_t,
                                       uint32_t ) =
        ( void* )OTF2_DefWriter_DefRegion;
    if ( isGlobal )
    {
        defRegion = ( void* )OTF2_GlobDefWriter_GlobDefRegion;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, Region, region )
    {
        uint32_t source_file_id = OTF2_UNDEFINED_UINT32;
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            source_file_id = SCOREP_HANDLE_TO_ID(
                definition->file_name_handle,
                String,
                definitionManager->page_manager );
        }

        SCOREP_Error_Code status = defRegion(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
            SCOREP_HANDLE_TO_ID( definition->description_handle, String, definitionManager->page_manager ),
            scorep_region_type_to_otf_region_type( definition->region_type, isGlobal ),
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
scorep_write_communicator_definitions( void*                     writerHandle,
                                       SCOREP_DefinitionManager* definitionManager )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defComm )( void*,
                                     uint64_t,
                                     uint64_t,
                                     uint64_t ) =
        ( void* )OTF2_GlobDefWriter_GlobDefMpiComm;

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, MPICommunicator, mpi_communicator )
    {
        SCOREP_Error_Code status = defComm(
            writerHandle,
            definition->sequence_number,
            SCOREP_HANDLE_TO_ID( definition->group, Group, definitionManager->page_manager ),
            definition->number_of_ranks );

        if ( status != SCOREP_SUCCESS )
        {
            scorep_handle_definition_writing_error( status, "SCOREP_Communicator_Definition" );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_group_definitions( void*                     writerHandle,
                                SCOREP_DefinitionManager* definitionManager,
                                bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defGroup )( void*,
                                      uint64_t,
                                      OTF2_GroupType,
                                      uint32_t,
                                      uint64_t,
                                      uint64_t* ) =
        ( void* )OTF2_DefWriter_DefGroup;
    if ( isGlobal )
    {
        defGroup = ( void* )OTF2_GlobDefWriter_GlobDefGroup;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager, Group, group )
    {
        SCOREP_Error_Code status = defGroup(
            writerHandle,
            definition->sequence_number,
            scorep_group_type_to_otf_group_type( definition->group_type, isGlobal ),
            SCOREP_HANDLE_TO_ID( definition->name_handle, String, definitionManager->page_manager ),
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
scorep_write_mpi_window_definitions( void*                     writerHandle,
                                     SCOREP_DefinitionManager* definitionManager,
                                     bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defMpiWin )( void*,
                                       uint32_t,
                                       uint64_t ) =
        ( void* )OTF2_DefWriter_DefMpiWin;
    if ( isGlobal )
    {
        defMpiWin = ( void* )OTF2_GlobDefWriter_GlobDefMpiWin;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  MPIWindow,
                                  mpi_window )
    {
        //SCOREP_Error_Code status = defMpiWin(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MPIWindow_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_mpi_cartesian_topology_definitions( void*                     writerHandle,
                                                 SCOREP_DefinitionManager* definitionManager,
                                                 bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  MPICartesianTopology,
                                  mpi_cartesian_topology )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MPICartesianTopology_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_mpi_cartesian_coords_definitions( void*                     writerHandle,
                                               SCOREP_DefinitionManager* definitionManager,
                                               bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  MPICartesianCoords,
                                  mpi_cartesian_coords )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MPICartesianCoords_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_counter_group_definitions( void*                     writerHandle,
                                        SCOREP_DefinitionManager* definitionManager,
                                        bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  CounterGroup,
                                  counter_group )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_CounterGroup_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_counter_definitions( void*                     writerHandle,
                                  SCOREP_DefinitionManager* definitionManager,
                                  bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  Counter,
                                  counter )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_Counter_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_io_file_group_definitions( void*                     writerHandle,
                                        SCOREP_DefinitionManager* definitionManager,
                                        bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  IOFileGroup,
                                  io_file_group )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_IOFileGroup_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_io_file_definitions( void*                     writerHandle,
                                  SCOREP_DefinitionManager* definitionManager,
                                  bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  IOFile,
                                  io_file )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_IOFile_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_marker_group_definitions( void*                     writerHandle,
                                       SCOREP_DefinitionManager* definitionManager,
                                       bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  MarkerGroup,
                                  marker_group )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_MarkerGroup_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_marker_definitions( void*                     writerHandle,
                                 SCOREP_DefinitionManager* definitionManager,
                                 bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  Marker,
                                  marker )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_Marker_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_parameter_definitions( void*                     writerHandle,
                                    SCOREP_DefinitionManager* definitionManager,
                                    bool                      isGlobal )
{
    assert( writerHandle );
    //SCOREP_Error_Code ( * def... )( void*,
    //                                ...
    //                                ... ) =
    //    ( void* )OTF2_DefWriter_Def...;
    //if ( isGlobal )
    //{
    //    def... = ( void* )OTF2_GlobDefWriter_GlobDef...;
    //}

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  Parameter,
                                  parameter )
    {
        //SCOREP_Error_Code status = def...(writerHandle, ...);
        //if ( status != SCOREP_SUCCESS )
        //{
        //    scorep_handle_definition_writing_error( status, "SCOREP_Parameter_Definition" );
        //}
        SCOREP_DEBUG_NOT_YET_IMPLEMENTED();
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_callpath_definitions( void*                     writerHandle,
                                   SCOREP_DefinitionManager* definitionManager,
                                   bool                      isGlobal )
{
    assert( writerHandle );
    SCOREP_Error_Code ( * defCallpath )( void*,
                                         uint32_t,
                                         uint32_t,
                                         uint32_t,
                                         uint8_t ) =
        ( void* )OTF2_DefWriter_DefCallpath;
    if ( isGlobal )
    {
        defCallpath = ( void* )OTF2_GlobDefWriter_GlobDefCallpath;
    }

    SCOREP_DEFINITION_FOREACH_DO( definitionManager,
                                  Callpath,
                                  callpath )
    {
        if ( !definition->with_parameter )
        {
            uint32_t parent_callpath_sequence_number = 0;
            if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
            {
                parent_callpath_sequence_number = SCOREP_HANDLE_TO_ID( definition->parent_callpath_handle,
                                                                       Callpath,
                                                                       definitionManager->page_manager );
            }
            SCOREP_Error_Code status = defCallpath(
                writerHandle,
                definition->sequence_number,
                parent_callpath_sequence_number,
                SCOREP_HANDLE_TO_ID( definition->callpath_argument.region_handle,
                                     Region,
                                     definitionManager->page_manager ),
                0 /// @todo (uint8_t call_path_order) what are we supposed to pass here?
                );

            if ( status != SCOREP_SUCCESS )
            {
                scorep_handle_definition_writing_error( status, "SCOREP_Callpath_Definition" );
            }
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}


static void
scorep_write_mappings( OTF2_DefWriter* localDefinitionWriter )
{
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( region, REGION, localDefinitionWriter );
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( group, GROUP, localDefinitionWriter );
    SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( mpi_communicator, MPI_COMMUNICATOR, localDefinitionWriter );
}


static void
scorep_write_local_definitions( OTF2_DefWriter* localDefinitionWriter )
{
    scorep_write_string_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_system_tree_node_definitions(       localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_group_definitions(         localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_location_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_source_file_definitions(            localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_region_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_group_definitions(                  localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_mpi_window_definitions(             localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_mpi_cartesian_topology_definitions( localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_mpi_cartesian_coords_definitions(   localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_counter_group_definitions(          localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_counter_definitions(                localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_io_file_group_definitions(          localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_io_file_definitions(                localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_marker_group_definitions(           localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_marker_definitions(                 localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_parameter_definitions(              localDefinitionWriter, &scorep_local_definition_manager, false );
    scorep_write_callpath_definitions(               localDefinitionWriter, &scorep_local_definition_manager, false );
}


static void
scorep_write_global_definitions( OTF2_GlobDefWriter* global_definition_writer )
{
    assert( SCOREP_Mpi_GetRank() == 0 );
    assert( scorep_unified_definition_manager );

    scorep_write_string_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_system_tree_node_definitions(       global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_location_group_definitions(         global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_location_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_source_file_definitions(            global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_region_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_group_definitions(                  global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_mpi_window_definitions(             global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_mpi_cartesian_topology_definitions( global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_mpi_cartesian_coords_definitions(   global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_counter_group_definitions(          global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_counter_definitions(                global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_io_file_group_definitions(          global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_io_file_definitions(                global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_marker_group_definitions(           global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_marker_definitions(                 global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_parameter_definitions(              global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_callpath_definitions(               global_definition_writer, scorep_unified_definition_manager, true );
    scorep_write_communicator_definitions(           global_definition_writer, scorep_unified_definition_manager );
}


static void
scorep_write_number_of_definitions_per_location( OTF2_GlobDefWriter* global_definition_writer )
{
    int* n_locations_per_rank = SCOREP_Mpi_GatherNumberOfLocationsPerRank();
    int  n_global_locations   = 0;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        for ( int rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
        {
            n_global_locations += n_locations_per_rank[ rank ];
        }
    }

    int* n_definitions_per_location = 0;
    n_definitions_per_location = SCOREP_Mpi_GatherNumberOfDefinitionsPerLocation( n_locations_per_rank, n_global_locations );

    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        OTF2_Archive_SetNumberOfLocations( scorep_otf2_archive, n_global_locations );

        if ( 0 /* unify failed => fallback */ )
        {
            SCOREP_Error_Code status = OTF2_GlobDefWriter_GlobDefString( global_definition_writer, 0, "" );
            assert( status == SCOREP_SUCCESS );
            uint32_t          location_name_id = 0;

            int               location_index = 0; // index into n_definitions_per_location[]
            for ( int rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
            {
                for ( int local_location_id = 0; local_location_id < n_locations_per_rank[ rank ]; ++local_location_id )
                {
                    uint64_t          global_location_id = ( ( ( uint64_t )local_location_id ) << 32 ) | ( uint64_t )rank;
                    SCOREP_Error_Code status             = OTF2_GlobDefWriter_GlobDefLocation(
                        global_definition_writer,
                        global_location_id,
                        location_name_id,
                        OTF2_GLOB_LOCATION_TYPE_CPU_THREAD, // use THREAD instead of PROCESS according to Dominic
                        0 /* dummy number of events */,
                        n_definitions_per_location[ location_index ],
                        1 /* dummy timer resolution */,
                        rank /*assume that the rank is global id of process group */  );
                    assert( status == SCOREP_SUCCESS );
                    ++location_index;
                }
            }
        }
    }

    free( n_definitions_per_location );
    free( n_locations_per_rank );
}


static OTF2_FlushType
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


static OTF2_DefWriter*
scorep_create_local_definition_writer( SCOREP_Location_Definition* definition )
{
    OTF2_DefWriter* definition_writer =
        OTF2_Archive_GetDefWriter( scorep_otf2_archive,
                                   definition->global_location_id,
                                   scorep_on_definitions_pre_flush,
                                   SCOREP_OnTraceAndDefinitionPostFlush );

    assert( definition_writer );
    return definition_writer;
}


static OTF2_GlobDefWriter*
scorep_create_global_definition_writer()
{
    OTF2_GlobDefWriter* global_definition_writer =
        OTF2_Archive_GetGlobDefWriter( scorep_otf2_archive,
                                       SCOREP_OnTracePreFlush,
                                       SCOREP_OnTraceAndDefinitionPostFlush );
    assert( global_definition_writer );
    return global_definition_writer;
}


void
SCOREP_Tracing_WriteDefinitions()
{
    assert( scorep_otf2_archive );

    /* Write for all local locations the same local definition file */
    SCOREP_CreateExperimentDir();
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Location, location )
    {
        OTF2_DefWriter* local_definition_writer =
            scorep_create_local_definition_writer( definition );
        if ( 1 /* unify debug/fallback */ )
        {
            scorep_write_local_definitions( local_definition_writer );
        }
        scorep_write_mappings( local_definition_writer );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();


    OTF2_GlobDefWriter* global_definition_writer = NULL;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        global_definition_writer = scorep_create_global_definition_writer();
        scorep_write_global_definitions( global_definition_writer );
        /// @todo Daniel, what to do here for profiling?
    }
    // uses MPI communication. references string handle, so write after strings
    // this may become obsolete, see comment in scorep_write_location_definitions()
    // only rank 0 will reference global_definition_writer
    scorep_write_number_of_definitions_per_location( global_definition_writer );
}
