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


/**
 * @file        src/measurement/scorep_types.c
 *
 *
 * @brief Functions to convert types from @ref SCOREP_Types.h into strings.
 */


#include <config.h>

#include "scorep_types.h"

/**
 * Converts a SCOREP_AdapterType into a string.
 */
const char*
scorep_adapter_type_to_string( SCOREP_AdapterType adapterType )
{
    #define SCOREP_ADAPTER( NAME, name_str, OTF2_NAME )  \
    case SCOREP_ADAPTER_ ## NAME:                    \
        return name_str;

    switch ( adapterType )
    {
        SCOREP_ADAPTERS

        default:
            return "unknown";
    }
    #undef SCOREP_ADAPTER
}


/**
 * Converts a SCOREP_RegionType into a string.
 */
const char*
scorep_region_type_to_string( SCOREP_RegionType regionType )
{
    #define SCOREP_REGION_TYPE( NAME, name_str )  \
    case SCOREP_REGION_ ## NAME:                  \
        return name_str;

    switch ( regionType )
    {
        case SCOREP_REGION_FUNCTION:
            return "function";
        case SCOREP_REGION_LOOP:
            return "loop";
        case SCOREP_REGION_USER:
            return "user";
        case SCOREP_REGION_CODE:
            return "code";
        case SCOREP_REGION_PHASE:
            return "phase";
        case SCOREP_REGION_DYNAMIC:
            return "dynamic";
        case SCOREP_REGION_DYNAMIC_PHASE:
            return "dynamic phase";
        case SCOREP_REGION_DYNAMIC_LOOP:
            return "dynamic loop";
        case SCOREP_REGION_DYNAMIC_FUNCTION:
            return "dynamix function";
        case SCOREP_REGION_DYNAMIC_LOOP_PHASE:
            return "dynamix loop phase";

            SCOREP_REGION_TYPES

        default:
            return "unknown";
    }
    #undef SCOREP_REGION_TYPE
}


/**
 * Converts a SCOREP_ParameterType into a string.
 */
const char*
scorep_parameter_type_to_string( SCOREP_ParameterType parameterType )
{
    switch ( parameterType )
    {
        case SCOREP_PARAMETER_INT64:
            return "int64";
        case SCOREP_PARAMETER_UINT64:
            return "uint64";
        case SCOREP_PARAMETER_STRING:
            return "string";
        default:
            return "invalid";
    }
}


/**
 * Converts a SCOREP_ConfigType into a string.
 */
const char*
scorep_config_type_to_string( SCOREP_ConfigType configType )
{
    switch ( configType )
    {
        case SCOREP_CONFIG_TYPE_PATH:
            return "path";
        case SCOREP_CONFIG_TYPE_STRING:
            return "string";
        case SCOREP_CONFIG_TYPE_BOOL:
            return "boolean";
        case SCOREP_CONFIG_TYPE_NUMBER:
            return "number";
        case SCOREP_CONFIG_TYPE_SIZE:
            return "size";
        case SCOREP_CONFIG_TYPE_SET:
            return "set";
        case SCOREP_CONFIG_TYPE_BITSET:
            return "bitset";
        default:
            return "unknown";
    }
}

/**
 * Converts a SCOREP_LocationType into a string.
 */
const char*
scorep_location_type_to_string( SCOREP_LocationType locationType )
{
    switch ( locationType )
    {
        case SCOREP_LOCATION_TYPE_CPU_THREAD:
            return "CPU thread";
        case SCOREP_LOCATION_TYPE_GPU:
            return "GPU";
        case SCOREP_LOCATION_TYPE_METRIC:
            return "metric location";
        default:
            return "unknown";
    }
}
