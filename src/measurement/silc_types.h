#ifndef SILC_INTERNAL_TYPES_H
#define SILC_INTERNAL_TYPES_H


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
 * If @a handle is equal to @ref SILC_INVALID_SOURCE_FILE "invalid" will be
   returned, else a hexadecimal representation of the value.

 * @a stringBuffer and @a stringBufferSize are needed to make this function
   re-entrant safe.

 * @param handle            The source file handle which should be expressed as
 *                          a string.
 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.

 * @return                  A string representation of @a handle.
 */
const char*
silc_source_file_handle_to_string
(
    SILC_SourceFileHandle handle,
    char*                 stringBuffer
    size_t                stringBufferSize
);


/**
 * Converts a SILC_LineNo into a string.
 *
 * If @a lineNo is equal to @ref SILC_INVALID_LINE_NO "invalid" will be
   returned, else a decimal representation of the value.

 * @a stringBuffer and @a stringBufferSize are needed to make this function
   re-entrant safe.

 * @param lineNo            The line number which should be expressed as a
 *                          string.
 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.

 * @return                  A string representation of @a handle.
 */
const char*
silc_line_number_to_string
(
    SILC_LineNo lineNo,
    char*       stringBuffer
    size_t      stringBufferSize
);


/**
 * Converts a SILC_AdapterType into a string.
 *
 * @param adapterType   The adapter type which should be expressed as a string.
 *
 * @return              A string representation of @a adapterType.
 */
const char*
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
const char*
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
const char*
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
const char*
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
const char*
silc_config_type_to_string
(
    SILC_ConfigType configType
);


#endif /* SILC_INTERNAL_TYPES_H */
