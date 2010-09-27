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

#ifndef SCOREP_DEFINITION_STRUCTS_H
#define SCOREP_DEFINITION_STRUCTS_H



/**
 * @file       scorep_definition_structs.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <SCOREP_DefinitionHandles.h>
#include "scorep_definition_handles.h"
#include <SCOREP_Types.h>

SCOREP_DEFINE_DEFINITION_TYPE( String )
{
    SCOREP_DEFINITION_HEADER( String );
    SCOREP_StringHandle hash_next;


    uint32_t string_length;
    // variable array member
    char     string_data[ 1 ];
};


SCOREP_DEFINE_DEFINITION_TYPE( Location )
{
    SCOREP_DEFINITION_HEADER( Location );

    /* don't use the sequence number for the id, this is generated */
    uint64_t            global_location_id;
    SCOREP_StringHandle name_handle;
    SCOREP_LocationType location_type;
};


SCOREP_DEFINE_DEFINITION_TYPE( SourceFile )
{
    SCOREP_DEFINITION_HEADER( SourceFile );

    SCOREP_StringHandle name_handle;
};


SCOREP_DEFINE_DEFINITION_TYPE( Region )
{
    SCOREP_DEFINITION_HEADER( Region );

    // Add SCOREP_Region stuff from here on.
    SCOREP_StringHandle name_handle;
    SCOREP_StringHandle description_handle;                  // currently not used
    SCOREP_RegionType   region_type;                         // maps to OTF2_RegionType
    SCOREP_StringHandle file_handle;
    SCOREP_LineNo       begin_line;
    SCOREP_LineNo       end_line;
    SCOREP_AdapterType  adapter_type;                        // Used by Cube 4
};


SCOREP_DEFINE_DEFINITION_TYPE( Group )
{
    SCOREP_DEFINITION_HEADER( Group );

    /* OTF2 groups have a uint64_t as id, we use the sequences number,
     * until we know, that this may clash with location ids
     */

    // Add SCOREP_Group stuff from here on.
    SCOREP_GroupType group_type;
    uint64_t         number_of_members;
    // variable array member
    uint64_t         members[ 1 ];
};


SCOREP_DEFINE_DEFINITION_TYPE( MPIWindow )
{
    SCOREP_DEFINITION_HEADER( MPIWindow );

    // Add SCOREP_MPIWindow stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianTopology )
{
    SCOREP_DEFINITION_HEADER( MPICartesianTopology );

    // Add SCOREP_MPICartesianTopology stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianCoords )
{
    SCOREP_DEFINITION_HEADER( MPICartesianCoords );

    // Add SCOREP_MPICartesianCoords stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( CounterGroup )
{
    SCOREP_DEFINITION_HEADER( CounterGroup );

    // Add SCOREP_CounterGroup stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( Counter )
{
    SCOREP_DEFINITION_HEADER( Counter );

    // Add SCOREP_Counter stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( IOFileGroup )
{
    SCOREP_DEFINITION_HEADER( IOFileGroup );

    // Add SCOREP_IOFileGroup stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( IOFile )
{
    SCOREP_DEFINITION_HEADER( IOFile );

    // Add SCOREP_IOFile stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MarkerGroup )
{
    SCOREP_DEFINITION_HEADER( MarkerGroup );

    // Add SCOREP_MarkerGroup stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( Marker )
{
    SCOREP_DEFINITION_HEADER( Marker );

    // Add SCOREP_Marker stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( Parameter )
{
    SCOREP_DEFINITION_HEADER( Parameter );

    // Add SCOREP_Parameter stuff from here on.
    SCOREP_StringHandle  name_handle;
    SCOREP_StringHandle  description_handle;                 // currently not used
    SCOREP_ParameterType parameter_type;
};

SCOREP_DEFINE_DEFINITION_TYPE( Callpath )
{
    SCOREP_DEFINITION_HEADER( Callpath );

    // Add SCOREP_Callpath stuff from here on.
    SCOREP_CallpathHandle parent_callpath_handle;

    /* This controlls the callpath_argument union */
    bool with_parameter;
    union
    {
        SCOREP_RegionHandle    region_handle;
        SCOREP_ParameterHandle parameter_handle;
    } callpath_argument;

    /* This is controlled by the type of the parameter when
     * with_parameter is true
     */
    union
    {
        SCOREP_StringHandle string_handle;
        int64_t             integer_value;
    } parameter_value;
};


#endif /* SCOREP_DEFINITION_STRUCTS_H */
