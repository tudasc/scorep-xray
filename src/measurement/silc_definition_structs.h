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
    SILC_String_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    uint32_t                       string_length;
    // variable array member
    char                           string_data[ 1 ];
};


SILC_DEFINE_DEFINITION_TYPE( Location )
{
    SILC_Location_Definition_Movable next;
    /** locations do have an uin64_t as id. */
    uint64_t                         id;
    SILC_String_Definition_Movable   name_handle;
    SILC_LocationType                location_type;
};


SILC_DEFINE_DEFINITION_TYPE( SourceFile )
{
    SILC_SourceFile_Definition_Movable next;
    uint32_t                           id;
    // Keep above order to be able to cast between definition types.
    SILC_String_Definition_Movable     name_handle;
};


SILC_DEFINE_DEFINITION_TYPE( Region )
{
    SILC_Region_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Region stuff from here on.
    SILC_String_Definition_Movable     name_handle;
    SILC_String_Definition_Movable     description_handle; // currently not used
    SILC_RegionType                    region_type;        // maps to OTF2_RegionType
    SILC_SourceFile_Definition_Movable file_handle;
    SILC_LineNo                        begin_line;
    SILC_LineNo                        end_line;
    SILC_AdapterType                   adapter_type;       // currently not used
};


SILC_DEFINE_DEFINITION_TYPE( Group )
{
    SILC_Group_Definition_Movable next;
    uint64_t                      id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Group stuff from here on.
    SILC_GroupType group_type;
    uint64_t       number_of_members;
    // variable array member
    uint64_t       members[ 1 ];
};


SILC_DEFINE_DEFINITION_TYPE( MPIWindow )
{
    SILC_MPIWindow_Definition_Movable next;
    uint32_t                          id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MPIWindow stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( MPICartesianTopology )
{
    SILC_MPICartesianTopology_Definition_Movable next;
    uint32_t                                     id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MPICartesianTopology stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( MPICartesianCoords )
{
    SILC_MPICartesianCoords_Definition_Movable next;
    uint32_t                                   id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MPICartesianCoords stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( CounterGroup )
{
    SILC_CounterGroup_Definition_Movable next;
    uint32_t                             id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_CounterGroup stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( Counter )
{
    SILC_Counter_Definition_Movable next;
    uint32_t                        id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Counter stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( IOFileGroup )
{
    SILC_IOFileGroup_Definition_Movable next;
    uint32_t                            id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_IOFileGroup stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( IOFile )
{
    SILC_IOFile_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_IOFile stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( MarkerGroup )
{
    SILC_MarkerGroup_Definition_Movable next;
    uint32_t                            id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MarkerGroup stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( Marker )
{
    SILC_Marker_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Marker stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( Parameter )
{
    SILC_Parameter_Definition_Movable next;
    uint32_t                          id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Parameter stuff from here on.
};


SILC_DEFINE_DEFINITION_TYPE( Callpath )
{
    SILC_Callpath_Definition_Movable next;
    uint32_t                         id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Callpath stuff from here on.
};


#endif /* SILC_DEFINITION_STRUCTS_H */
