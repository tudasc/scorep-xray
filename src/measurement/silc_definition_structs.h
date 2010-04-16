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

typedef struct char_Movable char_Movable;
SILC_MOVABLE_TYPE( char );


SILC_MOVABLE_TYPE( SILC_String_Definition );
struct SILC_String_Definition
{
    SILC_String_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_SourceFile stuff from here on.
    char_Movable str;
};


SILC_MOVABLE_TYPE( SILC_SourceFile_Definition );
struct SILC_SourceFile_Definition
{
    SILC_SourceFile_Definition_Movable next;
    uint32_t                           id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_SourceFile stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_Region_Definition );
struct SILC_Region_Definition
{
    SILC_Region_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Region stuff from here on.
    SILC_String_Definition_Movable     name_handle;
    SILC_SourceFile_Definition_Movable file_handle;                   // is actually a string handle
    uint32_t                           region_description_identifier; // currently not used
    SILC_RegionType                    region_type;                   // maps to OTF2_RegionType
    SILC_LineNo                        begin_line;
    SILC_LineNo                        end_line;
    SILC_AdapterType                   adapter_type; // currently not used
};


SILC_MOVABLE_TYPE( SILC_MPICommunicator_Definition );
struct SILC_MPICommunicator_Definition
{
    SILC_MPICommunicator_Definition_Movable next;
    uint32_t                                id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MPICommunicator stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_MPIWindow_Definition );
struct SILC_MPIWindow_Definition
{
    SILC_MPIWindow_Definition_Movable next;
    uint32_t                          id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MPIWindow stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_MPICartesianTopology_Definition );
struct SILC_MPICartesianTopology_Definition
{
    SILC_MPICartesianTopology_Definition_Movable next;
    uint32_t                                     id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MPICartesianTopology stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_MPICartesianCoords_Definition );
struct SILC_MPICartesianCoords_Definition
{
    SILC_MPICartesianCoords_Definition_Movable next;
    uint32_t                                   id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MPICartesianCoords stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_CounterGroup_Definition );
struct SILC_CounterGroup_Definition
{
    SILC_CounterGroup_Definition_Movable next;
    uint32_t                             id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_CounterGroup stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_Counter_Definition );
struct SILC_Counter_Definition
{
    SILC_Counter_Definition_Movable next;
    uint32_t                        id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Counter stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_IOFileGroup_Definition );
struct SILC_IOFileGroup_Definition
{
    SILC_IOFileGroup_Definition_Movable next;
    uint32_t                            id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_IOFileGroup stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_IOFile_Definition );
struct SILC_IOFile_Definition
{
    SILC_IOFile_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_IOFile stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_MarkerGroup_Definition );
struct SILC_MarkerGroup_Definition
{
    SILC_MarkerGroup_Definition_Movable next;
    uint32_t                            id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_MarkerGroup stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_Marker_Definition );
struct SILC_Marker_Definition
{
    SILC_Marker_Definition_Movable next;
    uint32_t                       id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Marker stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_Parameter_Definition );
struct SILC_Parameter_Definition
{
    SILC_Parameter_Definition_Movable next;
    uint32_t                          id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Parameter stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_Callpath_Definition );
struct SILC_Callpath_Definition
{
    SILC_Callpath_Definition_Movable next;
    uint32_t                         id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_Callpath stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_CallpathParameterInteger_Definition );
struct SILC_CallpathParameterInteger_Definition
{
    SILC_CallpathParameterInteger_Definition_Movable next;
    uint32_t                                         id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_CallpathParameterInteger stuff from here on.
};


SILC_MOVABLE_TYPE( SILC_CallpathParameterString_Definition );
struct SILC_CallpathParameterString_Definition
{
    SILC_CallpathParameterString_Definition_Movable next;
    uint32_t                                        id;
    // Keep above order to be able to cast between definition types.
    // Add SILC_CallpathParameterString stuff from here on.
};


#endif /* SILC_DEFINITION_STRUCTS_H */
