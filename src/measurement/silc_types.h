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


#ifndef SILC_INTERNAL_TYPES_H
#define SILC_INTERNAL_TYPES_H


#include <stdlib.h>
#include <stdio.h>


/**
 * @file        silc_types.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      ALPHA
 *
 * @brief Functions to convert types from @ref SILC_Types.h into strings.
 */


#include <SILC_Types.h>


/**
 * Converts a SILC_SourceFileHandle into a string.
 *
 * If @a handle equals to @ref SILC_INVALID_SOURCE_FILE "invalid" will be
 * returned, else a formated representation of the value.

 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.
 * @param format            A printf like format string, used to represent
 *                          @a lineNo.
 * @param handle            The source file handle which should be expressed as
 *                          a string.

 * @return                  A string representation of @a handle.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */

extern const char*
silc_source_file_to_string
(
    char*                 stringBuffer,
    size_t                stringBufferSize,
    const char*           format,
    SILC_SourceFileHandle handle
);


/**
 * Converts a SILC_LineNo into a string.
 *
 * If @a lineNo equals to @ref SILC_INVALID_LINE_NO "invalid" will be
 * returned, else a formated representation of the value.

 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.
 * @param format            A printf like format string, used to represent
 *                          @a lineNo.
 * @param lineNo            The line number which should be expressed as a
 *                          string.

 * @return                  A string representation of @a lineNo.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */

extern const char*
silc_line_number_to_string
(
    char*       stringBuffer,
    size_t      stringBufferSize,
    const char* format,
    SILC_LineNo lineNo
);


/**
 * Converts a SILC_RegionHandle into a string.
 *
 * If @a lineNo equals to @ref SILC_INVALID_REGION "invalid" will be
 * returned, else a formated representation of the value.

 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.
 * @param format            A printf like format string, used to represent
 *                          @a regionHandle.
 * @param lineNo            The line number which should be expressed as a
 *                          string.

 * @return                  A string representation of @a regionHandle.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */

extern const char*
silc_region_to_string
(
    char*             stringBuffer,
    size_t            stringBufferSize,
    const char*       format,
    SILC_RegionHandle regionHandle
);


/**
 * Converts a SILC_MPICommunicatorHandle into a string.
 *
 * If @a lineNo equals to @ref SILC_INVALID_MPI_COMMUNICATOR "invalid" will be
 * returned, else a formated representation of the value.

 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.
 * @param format            A printf like format string, used to represent
 *                          @a commHandle.
 * @param lineNo            The line number which should be expressed as a
 *                          string.

 * @return                  A string representation of @a commHandle.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */

extern const char*
silc_comm_to_string
(
    char*                      stringBuffer,
    size_t                     stringBufferSize,
    const char*                format,
    SILC_MPICommunicatorHandle commHandle
);


/**
 * Converts a SILC_MPIWindowHandle into a string.
 *
 * If @a lineNo equals to @ref SILC_INVALID_MPI_WINDOW "invalid" will be
 * returned, else a formated representation of the value.

 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.
 * @param format            A printf like format string, used to represent
 *                          @a windowHandle.
 * @param lineNo            The line number which should be expressed as a
 *                          string.

 * @return                  A string representation of @a windowHandle.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */

extern const char*
silc_window_to_string
(
    char*                stringBuffer,
    size_t               stringBufferSize,
    const char*          format,
    SILC_MPIWindowHandle windowHandle
);


/**
 * Converts a SILC_MPICartTopolHandle into a string.
 *
 * If @a cartHandle equals to @ref SILC_INVALID_CART_TOPOLOGY "invalid" will
 * be returned, else a decimal representation of the value.

 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.
 * @param format            A printf like format string, used to represent
 *                          @a cartHandle.
 * @param lineNo            The line number which should be expressed as a
 *                          string.

 * @return                  A string representation of @a cartHandle.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */

extern const char*
silc_mpi_cart_topol_to_string
(
    char*                   stringBuffer,
    size_t                  stringBufferSize,
    const char*             format,
    SILC_MPICartTopolHandle cartHandle
);


/**
 * Converts a SILC_AdapterType into a string.
 *
 * @param adapterType   The adapter type which should be expressed as a string.
 *
 * @return              A string representation of @a adapterType.
 */

extern const char*
silc_adapter_type_to_string
(
    SILC_AdapterType adapterType
);


/**
 * Converts a SILC_RegionType into a string.
 *
 * @param regionType    The region type which should be expressed as a string.
 *
 * @return              A string representation of @a regionType.
 */

extern const char*
silc_region_type_to_string
(
    SILC_RegionType regionType
);


/**
 * Converts a SILC_CounterType into a string.
 *
 * @param counterType   The counter type which should be expressed as a string.
 *
 * @return              A string representation of @a counterType.
 */

extern const char*
silc_counter_type_to_string
(
    SILC_CounterType counterType
);


/**
 * Converts a SILC_ParameterType into a string.
 *
 * @param parameterType The parameter type which should be expressed as a
 *                      string.
 *
 * @return              A string representation of @a parameterType.
 */

extern const char*
silc_parameter_type_to_string
(
    SILC_ParameterType parameterType
);


/**
 * Converts a SILC_ConfigType into a string.
 *
 * @param configType    The config type which should be expressed as a string.
 *
 * @return              A string representation of @a configType.
 */

extern const char*
silc_config_type_to_string
(
    SILC_ConfigType configType
);


#endif /* SILC_INTERNAL_TYPES_H */
