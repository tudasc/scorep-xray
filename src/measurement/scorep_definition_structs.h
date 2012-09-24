/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
#include "scorep_definition_macros.h"
#include <SCOREP_Types.h>


/* super object for all definitions */
SCOREP_DEFINE_DEFINITION_TYPE( Any )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Any );
};


SCOREP_DEFINE_DEFINITION_TYPE( String )
{
    SCOREP_DEFINE_DEFINITION_HEADER( String );

    uint32_t string_length;
    // variable array member
    char string_data[];
};


SCOREP_DEFINE_DEFINITION_TYPE( Location )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Location );

    /* don't use the sequence number for the id, this is generated */
    uint64_t              global_location_id;
    SCOREP_LocationHandle parent;
    SCOREP_StringHandle   name_handle;
    SCOREP_LocationType   location_type;
    uint64_t              number_of_events;      // only known after measurement
    uint32_t              location_group_id;
};

SCOREP_DEFINE_DEFINITION_TYPE( LocationGroup )
{
    SCOREP_DEFINE_DEFINITION_HEADER( LocationGroup );

    /* don't use the sequence number for the id, this is generated */
    uint32_t                    global_location_group_id;
    SCOREP_SystemTreeNodeHandle parent;
    SCOREP_StringHandle         name_handle;
    SCOREP_LocationGroupType    location_group_type;
};

SCOREP_DEFINE_DEFINITION_TYPE( SystemTreeNode )
{
    SCOREP_DEFINE_DEFINITION_HEADER( LocationGroup );

    SCOREP_SystemTreeNodeHandle parent_handle;
    SCOREP_StringHandle         name_handle;
    SCOREP_StringHandle         class_handle;
};


SCOREP_DEFINE_DEFINITION_TYPE( SourceFile )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SourceFile );

    SCOREP_StringHandle name_handle;
};


SCOREP_DEFINE_DEFINITION_TYPE( Region )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Region );

    // Add SCOREP_Region stuff from here on.
    SCOREP_StringHandle name_handle;            // This field contains demangled region name
                                                // (if available, otherwise mangled region name)
    SCOREP_StringHandle canonical_name_handle;  // This field always contains mangled region name
    SCOREP_StringHandle description_handle;     // Currently not used
    SCOREP_RegionType   region_type;
    SCOREP_StringHandle file_name_handle;
    SCOREP_LineNo       begin_line;
    SCOREP_LineNo       end_line;
    SCOREP_AdapterType  adapter_type;           // Used by Cube 4
};


SCOREP_DEFINE_DEFINITION_TYPE( Group )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Group );

    /* OTF2 groups have a uint64_t as id, we use the sequences number,
     * until we know, that this may clash with location ids
     */

    // Add SCOREP_Group stuff from here on.
    SCOREP_GroupType    group_type;
    SCOREP_StringHandle name_handle;    // currently not used
    uint64_t            number_of_members;
    // variable array member
    uint64_t members[];
};


SCOREP_DEFINE_DEFINITION_TYPE( LocalMPICommunicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( LocalMPICommunicator );

    bool                              is_self_like;
    uint32_t                          local_rank;
    uint32_t                          global_root_rank;
    uint32_t                          root_id;
    SCOREP_StringHandle               name_handle;
    SCOREP_LocalMPICommunicatorHandle parent_handle;
};

SCOREP_DEFINE_DEFINITION_TYPE( MPICommunicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPICommunicator );

    SCOREP_GroupHandle           group_handle;
    uint32_t                     name_id;
    SCOREP_MPICommunicatorHandle parent_handle;
};

SCOREP_DEFINE_DEFINITION_TYPE( MPIWindow )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPIWindow );

    // Add SCOREP_MPIWindow stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianTopology )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPICartesianTopology );

    // Add SCOREP_MPICartesianTopology stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianCoords )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPICartesianCoords );

    // Add SCOREP_MPICartesianCoords stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( Metric )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Metric );

    // Add SCOREP_Metric stuff from here on.
    SCOREP_StringHandle        name_handle;
    SCOREP_StringHandle        description_handle;
    SCOREP_MetricSourceType    source_type;
    SCOREP_MetricMode          mode;
    SCOREP_MetricValueType     value_type;
    SCOREP_MetricBase          base;
    int64_t                    exponent;
    SCOREP_StringHandle        unit_handle;
    SCOREP_MetricProfilingType profiling_type;
};


SCOREP_DEFINE_DEFINITION_TYPE( SamplingSet )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SamplingSet );

    // order is important
    bool is_scoped;

    SCOREP_MetricOccurrence occurrence;
    uint8_t                 number_of_metrics;
    SCOREP_MetricHandle     metric_handles[];
};


/*
 * this is not a real definition
 * but an overload of the SamplingSet definition
 * because they share the same id namespace in OTF2
 */
SCOREP_DEFINE_DEFINITION_TYPE( ScopedSamplingSet )
{
    // Don't use ScopedSamplingSet here, there is no handle for this definition
    SCOREP_DEFINE_DEFINITION_HEADER( SamplingSet );

    // order is important
    bool is_scoped;

    SCOREP_SamplingSetHandle sampling_set_handle;
    SCOREP_LocationHandle    recorder_handle;
    SCOREP_MetricScope       scope_type;
    /* all types are handles */
    SCOREP_AnyHandle scope_handle;
};


SCOREP_DEFINE_DEFINITION_TYPE( IOFileGroup )
{
    SCOREP_DEFINE_DEFINITION_HEADER( IOFileGroup );

    // Add SCOREP_IOFileGroup stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( IOFile )
{
    SCOREP_DEFINE_DEFINITION_HEADER( IOFile );

    // Add SCOREP_IOFile stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MarkerGroup )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MarkerGroup );

    // Add SCOREP_MarkerGroup stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( Marker )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Marker );

    // Add SCOREP_Marker stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( Parameter )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Parameter );

    // Add SCOREP_Parameter stuff from here on.
    SCOREP_StringHandle  name_handle;
    SCOREP_ParameterType parameter_type;
};


SCOREP_DEFINE_DEFINITION_TYPE( Callpath )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Callpath );

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


SCOREP_DEFINE_DEFINITION_TYPE( Property )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Property );

    SCOREP_Property          property;
    SCOREP_PropertyCondition condition;
    bool                     initialValue;
    bool                     invalidated;
};


/* Not really a definition, but we need to store them somewhere */
typedef struct SCOREP_ClockOffset SCOREP_ClockOffset;
struct SCOREP_ClockOffset
{
    SCOREP_ClockOffset* next;

    uint64_t            time;
    int64_t             offset;
    double              stddev;
};


#endif /* SCOREP_DEFINITION_STRUCTS_H */
