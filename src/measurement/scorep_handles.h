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


#ifndef SCOREP_INTERNAL_HANDLES_H
#define SCOREP_INTERNAL_HANDLES_H

#include <stdlib.h>
#include <stdio.h>


/**
 * @file        src/measurement/scorep_handles.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      alpha
 *
 * @brief Functions to convert definition handles into strings.
 */


#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>


/**
 * Converts a uint32_t into a string.
 *
 * @param stringBuffer      A string buffer that holds at least @a
 *                          stringBufferSize bytes.
 * @param stringBufferSize  The size of @a stringBuffer.
 * @param format            A printf like format string, used to represent
 *                          @a lineNo.
 * @param value             The value which should be expressed as a string.
 * @param invalidValue      The invalid value.

 * @return                  A string representation of @a handle.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */
extern const char*
scorep_uint32_to_string( char*       stringBuffer,
                         size_t      stringBufferSize,
                         const char* format,
                         uint32_t    value,
                         uint32_t    invalidValue );


/**
 * Converts a SCOREP_SourceFileHandle into a string.
 *
 * If @a handle equals to @ref SCOREP_INVALID_SOURCE_FILE "invalid" will be
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
scorep_source_file_to_string( char*                   stringBuffer,
                              size_t                  stringBufferSize,
                              const char*             format,
                              SCOREP_SourceFileHandle handle );


/**
 * Converts a SCOREP_LineNo into a string.
 *
 * If @a lineNo equals to @ref SCOREP_INVALID_LINE_NO "invalid" will be
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
scorep_line_number_to_string( char*         stringBuffer,
                              size_t        stringBufferSize,
                              const char*   format,
                              SCOREP_LineNo lineNo );


/**
 * Converts a SCOREP_RegionHandle into a string.
 *
 * If @a lineNo equals to @ref SCOREP_INVALID_REGION "invalid" will be
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
scorep_region_to_string( char*               stringBuffer,
                         size_t              stringBufferSize,
                         const char*         format,
                         SCOREP_RegionHandle regionHandle );


/**
 * Converts a SCOREP_LocalMPICommunicatorHandle into a string.
 *
 * If @a lineNo equals to @ref SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR "invalid"
 * will be returned, else a formated representation of the value.

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
scorep_comm_to_string( char*                             stringBuffer,
                       size_t                            stringBufferSize,
                       const char*                       format,
                       SCOREP_LocalMPICommunicatorHandle commHandle );


/**
 * Converts a SCOREP_MPIWindowHandle into a string.
 *
 * If @a lineNo equals to @ref SCOREP_INVALID_MPI_WINDOW "invalid" will be
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
scorep_window_to_string( char*                  stringBuffer,
                         size_t                 stringBufferSize,
                         const char*            format,
                         SCOREP_MPIWindowHandle windowHandle );


#endif /* SCOREP_INTERNAL_TYPES_H */
