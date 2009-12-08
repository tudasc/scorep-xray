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
 * @file        silc_types.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      ALPHA
 *
 * @brief Functions to convert types from @ref SILC_Types.h into strings.
 */


#include "silc_types.h"


/**
 * Generic function to convert a type with an invalid value into a string.
 *
 * @note invalid will not be copied into the @a stringBuffer.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */
static inline const char*
generic_uint32_to_string
(
    char*       stringBuffer,
    size_t      stringBufferSize,
    const char* format,
    uint32_t    value,
    uint32_t    invalidValue
)
{
    if ( value == invalidValue )
    {
        return "invalid";
    }

    snprintf( stringBuffer, stringBufferSize, format, value );
    return stringBuffer;
}


/**
 * Converts a SILC_SourceFileHandle into a string.
 */
const char*
silc_source_file_to_string
(
    char*                 stringBuffer,
    size_t                stringBufferSize,
    const char*           format,
    SILC_SourceFileHandle handle
)
{
    return generic_uint32_to_string( stringBuffer,
                                     stringBufferSize,
                                     format,
                                     handle,
                                     SILC_INVALID_SOURCE_FILE );
}


/**
 * Converts a SILC_LineNo into a string.
 */
const char*
silc_line_number_to_string
(
    char*       stringBuffer,
    size_t      stringBufferSize,
    const char* format,
    SILC_LineNo lineNo
)
{
    return generic_uint32_to_string( stringBuffer,
                                     stringBufferSize,
                                     format,
                                     lineNo,
                                     SILC_INVALID_LINE_NO );
}


/**
 * Converts a SILC_RegionHandle into a string.
 */
extern const char*
silc_region_to_string
(
    char*             stringBuffer,
    size_t            stringBufferSize,
    const char*       format,
    SILC_RegionHandle regionHandle
)
{
    return generic_uint32_to_string( stringBuffer,
                                     stringBufferSize,
                                     format,
                                     regionHandle,
                                     SILC_INVALID_REGION );
}


/**
 * Converts a SILC_MPICommunicatorHandle into a string.
 */
extern const char*
silc_comm_to_string
(
    char*                      stringBuffer,
    size_t                     stringBufferSize,
    const char*                format,
    SILC_MPICommunicatorHandle commHandle
)
{
    return generic_uint32_to_string( stringBuffer,
                                     stringBufferSize,
                                     format,
                                     commHandle,
                                     SILC_INVALID_MPI_COMMUNICATOR );
}


/**
 * Converts a SILC_MPIWindowHandle into a string.
 */
extern const char*
silc_window_to_string
(
    char*                stringBuffer,
    size_t               stringBufferSize,
    const char*          format,
    SILC_MPIWindowHandle windowHandle
)
{
    return generic_uint32_to_string( stringBuffer,
                                     stringBufferSize,
                                     format,
                                     windowHandle,
                                     SILC_INVALID_MPI_WINDOW );
}


/**
 * Converts a SILC_MPICartTopolHandle into a string.
 */
const char*
silc_mpi_cart_topol_to_string
(
    char*                   stringBuffer,
    size_t                  stringBufferSize,
    const char*             format,
    SILC_MPICartTopolHandle cartHandle
)
{
    return generic_uint32_to_string( stringBuffer,
                                     stringBufferSize,
                                     format,
                                     cartHandle,
                                     SILC_INVALID_CART_TOPOLOGY );
}


/**
 * Converts a SILC_AdapterType into a string.
 */
const char*
silc_adapter_type_to_string
(
    SILC_AdapterType adapterType
)
{
    switch ( adapterType )
    {
        case SILC_ADAPTER_USER:
            return "user";
        case SILC_ADAPTER_COMPILER:
            return "compiler";
        case SILC_ADAPTER_MPI:
            return "mpi";
        case SILC_ADAPTER_POMP:
            return "pomp";
        case SILC_ADAPTER_PTHREAD:
            return "pthread";
        default:
            return "unknown";
    }
}


/**
 * Converts a SILC_RegionType into a string.
 */
const char*
silc_region_type_to_string
(
    SILC_RegionType regionType
)
{
    switch ( regionType )
    {
        case SILC_REGION_FUNCTION:
            return "function";
        case SILC_REGION_LOOP:
            return "loop";
        case SILC_REGION_USER:
            return "user";
        case SILC_REGION_PHASE:
            return "phase";
        case SILC_REGION_DYNAMIC:
            return "dynamic";
        case SILC_REGION_DYNAMIC_PHASE:
            return "dynamic phase";
        case SILC_REGION_DYNAMIC_LOOP:
            return "dynamic loop";
        case SILC_REGION_DYNAMIC_FUNCTION:
            return "dynamix function";
        case SILC_REGION_DYNAMIC_LOOP_PHASE:
            return "dynamix loop phase";
        case SILC_REGION_MPI_COLL_BARRIER:
            return "mpi barrier";
        case SILC_REGION_MPI_COLL_ONE2ALL:
            return "mpi one2all";
        case SILC_REGION_MPI_COLL_ALL2ONE:
            return "mpi all2one";
        case SILC_REGION_MPI_COLL_ALL2ALL:
            return "mpi all2all";
        case SILC_REGION_MPI_COLL_OTHER:
            return "mpi other";
        case SILC_REGION_OMP_PARALLEL:
            return "omp parallel";
        case SILC_REGION_OMP_LOOP:
            return "omp loop";
        case SILC_REGION_OMP_SECTIONS:
            return "omp secions";
        case SILC_REGION_OMP_SECTION:
            return "omp section";
        case SILC_REGION_OMP_WORKSHARE:
            return "omp workshare";
        case SILC_REGION_OMP_SINGLE:
            return "omp single";
        case SILC_REGION_OMP_MASTER:
            return "omp master";
        case SILC_REGION_OMP_CRITICAL:
            return "omp critical";
        case SILC_REGION_OMP_ATOMIC:
            return "omp atomic";
        case SILC_REGION_OMP_BARRIER:
            return "omp barrier";
        case SILC_REGION_OMP_IMPLICIT_BARRIER:
            return "omp implicit barrier";
        case SILC_REGION_OMP_FLUSH:
            return "omp flush";
        case SILC_REGION_OMP_CRITICAL_SBLOCK:
            return "omp critical sblock";
        case SILC_REGION_OMP_SINGLE_SBLOCK:
            return "omp single sblock";
        default:
            return "unknown";
    }
}


/**
 * Converts a SILC_CounterType into a string.
 */
const char*
silc_counter_type_to_string
(
    SILC_CounterType counterType
)
{
    switch ( counterType )
    {
        case SILC_COUNTER_INT64:
            return "int64";
        case SILC_COUNTER_DOUBLE:
            return "double";
        default:
            return "invalid";
    }
}


/**
 * Converts a SILC_ParameterType into a string.
 */
const char*
silc_parameter_type_to_string
(
    SILC_ParameterType parameterType
)
{
    switch ( parameterType )
    {
        case SILC_PARAMETER_INT64:
            return "int64";
        case SILC_PARAMETER_DOUBLE:
            return "double";
        case SILC_PARAMETER_STRING:
            return "string";
        default:
            return "invalid";
    }
}


/**
 * Converts a SILC_ConfigType into a string.
 */
const char*
silc_config_type_to_string
(
    SILC_ConfigType configType
)
{
    switch ( configType )
    {
        case SILC_CONFIG_TYPE_PATH:
            return "path";
        case SILC_CONFIG_TYPE_STRING:
            return "string";
        case SILC_CONFIG_TYPE_BOOL:
            return "boolean";
        case SILC_CONFIG_TYPE_NUMBER:
            return "number";
        case SILC_CONFIG_TYPE_SIZE:
            return "size";
        case SILC_CONFIG_TYPE_SET:
            return "set";
        case SILC_CONFIG_TYPE_BITSET:
            return "bitset";
        default:
            return "unknown";
    }
}
