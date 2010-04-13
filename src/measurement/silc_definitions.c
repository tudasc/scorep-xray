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


static bool silc_definitions_initialized = false;


/* *INDENT-OFF* */
static uint64_t silc_on_definitions_post_flush();
static OTF2_DefWriter* silc_create_definition_writer();
static void silc_write_definitions(OTF2_DefWriter* definitionWriter);
static void silc_delete_definition_writer(OTF2_DefWriter* definitionWriter);
static void silc_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_communicator_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_callpath_parameter_integer_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
static void silc_write_callpath_parameter_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter );
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
    #pragma omp critical (define_string)
    {
        static uint64_t         counter        = 0;
        SILC_String_Definition* new_definition = 0;
        SILC_ALLOC_NEW_DEFINITION( SILC_String_Definition )
        SILC_DEFINITIONS_LIST_PUSH_FRONT( silc_string_definitions_head_dummy )
        // init new_definition
        ++ counter;
        return new_definition;
    }
}


SILC_CallpathHandle
SILC_DefineCallpath( SILC_CallpathHandle parent,
                     SILC_RegionHandle   region )
{
    static uint64_t           counter        = 0;
    SILC_Callpath_Definition* new_definition = 0;
    SILC_ALLOC_NEW_DEFINITION( SILC_Callpath_Definition )
    SILC_DEFINITIONS_LIST_PUSH_FRONT( silc_callpath_definitions_head_dummy )
    // init new_definition
    ++ counter;
    return new_definition;
}


SILC_CallpathParameterIntegerHandle
SILC_DefineCallpathParameterInteger( SILC_CallpathHandle  parent,
                                     SILC_ParameterHandle param,
                                     int64_t              value )
{
    static uint64_t                           counter        = 0;
    SILC_CallpathParameterInteger_Definition* new_definition = 0;
    SILC_ALLOC_NEW_DEFINITION( SILC_CallpathParameterInteger_Definition )
    SILC_DEFINITIONS_LIST_PUSH_FRONT( silc_callpath_parameter_integer_definitions_head_dummy )
    // init new_definition
    ++ counter;
    return new_definition;
}


SILC_CallpathParameterStringHandle
SILC_DefineCallpathParameterString( SILC_CallpathHandle  parent,
                                    SILC_ParameterHandle param,
                                    SILC_StringHandle    value )
{
    static uint64_t                          counter        = 0;
    SILC_CallpathParameterString_Definition* new_definition = 0;
    SILC_ALLOC_NEW_DEFINITION( SILC_CallpathParameterString_Definition )
    SILC_DEFINITIONS_LIST_PUSH_FRONT( silc_callpath_parameter_string_definitions_head_dummy )
    // init new_definition
    ++ counter;
    return new_definition;
}


// definition type dependent part starts here

// dummy elements for singly linked lists
SILC_String_Definition                   silc_string_definitions_head_dummy = { 0, 0 };
SILC_SourceFile_Definition               silc_source_file_definitions_head_dummy = { 0, 0 };
SILC_Region_Definition                   silc_region_definitions_head_dummy = { 0, 0 };
SILC_MPICommunicator_Definition          silc_mpi_communicator_definitions_head_dummy = { 0, 0 };
SILC_MPIWindow_Definition                silc_mpi_window_definitions_head_dummy = { 0, 0 };
SILC_MPICartesianTopology_Definition     silc_mpi_cartesian_topology_definitions_head_dummy = { 0, 0 };
SILC_MPICartesianCoords_Definition       silc_mpi_cartesian_coords_definitions_head_dummy = { 0, 0 };
SILC_CounterGroup_Definition             silc_counter_group_definitions_head_dummy = { 0, 0 };
SILC_Counter_Definition                  silc_counter_definitions_head_dummy = { 0, 0 };
SILC_IOFileGroup_Definition              silc_io_file_group_definitions_head_dummy = { 0, 0 };
SILC_IOFile_Definition                   silc_io_file_definitions_head_dummy = { 0, 0 };
SILC_MarkerGroup_Definition              silc_marker_group_definitions_head_dummy = { 0, 0 };
SILC_Marker_Definition                   silc_marker_definitions_head_dummy = { 0, 0 };
SILC_Parameter_Definition                silc_parameter_definitions_head_dummy = { 0, 0 };
SILC_Callpath_Definition                 silc_callpath_definitions_head_dummy = { 0, 0 };
SILC_CallpathParameterInteger_Definition silc_callpath_parameter_integer_definitions_head_dummy = { 0, 0 };
SILC_CallpathParameterString_Definition  silc_callpath_parameter_string_definitions_head_dummy = { 0, 0 };


static void
silc_write_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_String_Definition* definition = &silc_string_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_source_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_SourceFile_Definition* definition = &silc_source_file_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_region_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Region_Definition* definition = &silc_region_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_communicator_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPICommunicator_Definition* definition = &silc_mpi_communicator_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_window_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPIWindow_Definition* definition = &silc_mpi_window_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_cartesian_topology_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPICartesianTopology_Definition* definition = &silc_mpi_cartesian_topology_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_mpi_cartesian_coords_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MPICartesianCoords_Definition* definition = &silc_mpi_cartesian_coords_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_counter_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_CounterGroup_Definition* definition = &silc_counter_group_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_counter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Counter_Definition* definition = &silc_counter_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_io_file_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_IOFileGroup_Definition* definition = &silc_io_file_group_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_io_file_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_IOFile_Definition* definition = &silc_io_file_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_marker_group_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_MarkerGroup_Definition* definition = &silc_marker_group_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_marker_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Marker_Definition* definition = &silc_marker_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_parameter_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Parameter_Definition* definition = &silc_parameter_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_callpath_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_Callpath_Definition* definition = &silc_callpath_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_callpath_parameter_integer_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_CallpathParameterInteger_Definition* definition = &silc_callpath_parameter_integer_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}


static void
silc_write_callpath_parameter_string_definitions_to_otf2( OTF2_DefWriter* definitionWriter )
{
    SILC_CallpathParameterString_Definition* definition = &silc_callpath_parameter_string_definitions_head_dummy;
    while ( definition->next )
    {
        definition = definition->next;
        //OTF2_DefWriter(definitionWriter, ...);
        assert( false ); // implement me
    }
}
