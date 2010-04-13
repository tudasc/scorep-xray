#ifndef SILC_DEFINITION_STRUCTS_H
#define SILC_DEFINITION_STRUCTS_H

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
 * @file       silc_definition_structs.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <SILC_DefinitionHandles.h>


struct SILC_String_Definition
{
    SILC_String_Definition* next;
    uint64_t                id;
    // add stuff here
};


struct SILC_SourceFile_Definition
{
    SILC_SourceFile_Definition* next;
    uint64_t                    id;
    // add stuff here
};


struct SILC_Region_Definition
{
    SILC_Region_Definition* next;
    uint64_t                id;
    // add stuff here
};


struct SILC_MPICommunicator_Definition
{
    SILC_MPICommunicator_Definition* next;
    uint64_t                         id;
    // add stuff here
};


struct SILC_MPIWindow_Definition
{
    SILC_MPIWindow_Definition* next;
    uint64_t                   id;
    // add stuff here
};


struct SILC_MPICartesianTopology_Definition
{
    SILC_MPICartesianTopology_Definition* next;
    uint64_t                              id;
    // add stuff here
};


struct SILC_MPICartesianCoords_Definition
{
    SILC_MPICartesianCoords_Definition* next;
    uint64_t                            id;
    // add stuff here
};


struct SILC_CounterGroup_Definition
{
    SILC_CounterGroup_Definition* next;
    uint64_t                      id;
    // add stuff here
};


struct SILC_Counter_Definition
{
    SILC_Counter_Definition* next;
    uint64_t                 id;
    // add stuff here
};


struct SILC_IOFileGroup_Definition
{
    SILC_IOFileGroup_Definition* next;
    uint64_t                     id;
    // add stuff here
};


struct SILC_IOFile_Definition
{
    SILC_IOFile_Definition* next;
    uint64_t                id;
    // add stuff here
};


struct SILC_MarkerGroup_Definition
{
    SILC_MarkerGroup_Definition* next;
    uint64_t                     id;
    // add stuff here
};


struct SILC_Marker_Definition
{
    SILC_Marker_Definition* next;
    uint64_t                id;
    // add stuff here
};


struct SILC_Parameter_Definition
{
    SILC_Parameter_Definition* next;
    uint64_t                   id;
    // add stuff here
};


struct SILC_Callpath_Definition
{
    SILC_Callpath_Definition* next;
    uint64_t                  id;
    // add stuff here
};


struct SILC_CallpathParameterInteger_Definition
{
    SILC_CallpathParameterInteger_Definition* next;
    uint64_t                                  id;
    // add stuff here
};


struct SILC_CallpathParameterString_Definition
{
    SILC_CallpathParameterString_Definition* next;
    uint64_t                                 id;
    // add stuff here
};


extern SILC_String_Definition                   silc_string_definitions_head_dummy;
extern SILC_SourceFile_Definition               silc_source_file_definitions_head_dummy;
extern SILC_Region_Definition                   silc_region_definitions_head_dummy;
extern SILC_MPICommunicator_Definition          silc_mpi_communicator_definitions_head_dummy;
extern SILC_MPIWindow_Definition                silc_mpi_window_definitions_head_dummy;
extern SILC_MPICartesianTopology_Definition     silc_mpi_cartesian_topology_definitions_head_dummy;
extern SILC_MPICartesianCoords_Definition       silc_mpi_cartesian_coords_definitions_head_dummy;
extern SILC_CounterGroup_Definition             silc_counter_group_definitions_head_dummy;
extern SILC_Counter_Definition                  silc_counter_definitions_head_dummy;
extern SILC_IOFileGroup_Definition              silc_io_file_group_definitions_head_dummy;
extern SILC_IOFile_Definition                   silc_io_file_definitions_head_dummy;
extern SILC_MarkerGroup_Definition              silc_marker_group_definitions_head_dummy;
extern SILC_Marker_Definition                   silc_marker_definitions_head_dummy;
extern SILC_Parameter_Definition                silc_parameter_definitions_head_dummy;
extern SILC_Callpath_Definition                 silc_callpath_definitions_head_dummy;
extern SILC_CallpathParameterInteger_Definition silc_callpath_parameter_integer_definitions_head_dummy;
extern SILC_CallpathParameterString_Definition  silc_callpath_parameter_string_definitions_head_dummy;

#endif /* SILC_DEFINITION_STRUCTS_H */
