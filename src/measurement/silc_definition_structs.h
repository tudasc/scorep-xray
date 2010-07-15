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
#include "silc_definition_handles.h"
#include <SILC_Types.h>

SILC_DEFINE_DEFINITION_TYPE( String )
{
    SILC_DEFINITION_HEADER( String );
    SILC_String_Definition_Movable hash_next;


    uint32_t string_length;
    // variable array member
    char     string_data[ 1 ];
};


SILC_DEFINE_DEFINITION_TYPE( Location )
{
    SILC_DEFINITION_HEADER( Location );

    /* don't use the sequence number for the id, this is generated */
    uint64_t                       global_location_id;
    SILC_String_Definition_Movable name_handle;
    SILC_LocationType              location_type;
};


SILC_DEFINE_DEFINITION_TYPE( SourceFile )
{
    SILC_DEFINITION_HEADER( SourceFile );

    SILC_String_Definition_Movable name_handle;
};


SILC_DEFINE_DEFINITION_TYPE( Region )
{
    SILC_DEFINITION_HEADER( Region );

    // Add SILC_Region stuff from here on.
    SILC_String_Definition_Movable name_handle;
    SILC_String_Definition_Movable description_handle;     // currently not used
    SILC_RegionType                region_type;            // maps to OTF2_RegionType
    SILC_String_Definition_Movable file_handle;
    SILC_LineNo                    begin_line;
    SILC_LineNo                    end_line;
    SILC_AdapterType               adapter_type;           // Used by Cube 4
};


SILC_DEFINE_DEFINITION_TYPE( Group )
{
    SILC_DEFINITION_HEADER( Group );

    /* OTF2 groups have a uint64_t as id, we use the sequences number,
     * until we know, that this may clash with location ids
     */

    // Add SILC_Group stuff from here on.
    SILC_GroupType group_type;
    uint64_t       number_of_members;
    // variable array member
    uint64_t       members[ 1 ];
};


SILC_DEFINE_DEFINITION_TYPE( MPIWindow )
{
    SILC_DEFINITION_HEADER( MPIWindow );

    // Add SILC_MPIWindow stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( MPICartesianTopology )
{
    SILC_DEFINITION_HEADER( MPICartesianTopology );

    // Add SILC_MPICartesianTopology stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( MPICartesianCoords )
{
    SILC_DEFINITION_HEADER( MPICartesianCoords );

    // Add SILC_MPICartesianCoords stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( CounterGroup )
{
    SILC_DEFINITION_HEADER( CounterGroup );

    // Add SILC_CounterGroup stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( Counter )
{
    SILC_DEFINITION_HEADER( Counter );

    // Add SILC_Counter stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( IOFileGroup )
{
    SILC_DEFINITION_HEADER( IOFileGroup );

    // Add SILC_IOFileGroup stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( IOFile )
{
    SILC_DEFINITION_HEADER( IOFile );

    // Add SILC_IOFile stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( MarkerGroup )
{
    SILC_DEFINITION_HEADER( MarkerGroup );

    // Add SILC_MarkerGroup stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( Marker )
{
    SILC_DEFINITION_HEADER( Marker );

    // Add SILC_Marker stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( Parameter )
{
    SILC_DEFINITION_HEADER( Parameter );

    // Add SILC_Parameter stuff from here on.
    SILC_ParameterType parameter_type;
};

SILC_DEFINE_DEFINITION_TYPE( Callpath )
{
    SILC_DEFINITION_HEADER( Callpath );

    // Add SILC_Callpath stuff from here on.
    SILC_Callpath_Definition_Movable parent_callpath_handle;

    /* This controlls the callpath_argument union */
    bool with_parameter;
    union
    {
        SILC_Region_Definition_Movable    region_handle;
        SILC_Parameter_Definition_Movable parameter_handle;
    } callpath_argument;

    /* This is controlled by the type of the parameter when
     * with_parameter is true
     */
    union
    {
        SILC_String_Definition_Movable string_handle;
        int64_t                        integer_value;
    } parameter_value;
};


#endif /* SILC_DEFINITION_STRUCTS_H */
