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
 * @file        src/measurement/scorep_handles.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status      alpha
 *
 * @brief Functions to convert definition handles into strings.
 */


#include <config.h>

#include "scorep_types.h"
#include "scorep_definition_structs.h"
#include <SCOREP_Memory.h>

/**
 * Generic function to convert a type with an invalid value into a string.
 *
 * @note invalid will not be copied into the @a stringBuffer.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */
const char*
scorep_uint32_to_string( char*       stringBuffer,
                         size_t      stringBufferSize,
                         const char* format,
                         uint32_t    value,
                         uint32_t    invalidValue )
{
    if ( value == invalidValue )
    {
        return "invalid";
    }

    snprintf( stringBuffer, stringBufferSize, format, value );
    return stringBuffer;
}


/**
 * Generic function to convert a type with an invalid value into a string.
 *
 * @note invalid will not be copied into the @a stringBuffer.
 *
 * @note For gcc, this function should be annotated with the
 *       __attribute__ ((format_arg (3))) attribute.
 */
const char*
scorep_any_handle_to_string( char*            stringBuffer,
                             size_t           stringBufferSize,
                             const char*      format,
                             SCOREP_AnyHandle handle,
                             SCOREP_AnyHandle invalidValue )
{
    if ( handle == invalidValue )
    {
        return "invalid";
    }

    snprintf( stringBuffer, stringBufferSize,
              format,
              SCOREP_LOCAL_HANDLE_TO_ID( handle, Any ) );
    return stringBuffer;
}


/**
 * Converts a SCOREP_SourceFileHandle into a string.
 */
const char*
scorep_source_file_to_string( char*                   stringBuffer,
                              size_t                  stringBufferSize,
                              const char*             format,
                              SCOREP_SourceFileHandle handle )
{
    return scorep_any_handle_to_string( stringBuffer,
                                        stringBufferSize,
                                        format,
                                        handle,
                                        SCOREP_INVALID_SOURCE_FILE );
}


/**
 * Converts a SCOREP_LineNo into a string.
 */
const char*
scorep_line_number_to_string( char*         stringBuffer,
                              size_t        stringBufferSize,
                              const char*   format,
                              SCOREP_LineNo lineNo )
{
    if ( lineNo == SCOREP_INVALID_LINE_NO )
    {
        return "invalid";
    }

    snprintf( stringBuffer, stringBufferSize, format, lineNo );
    return stringBuffer;
}


/**
 * Converts a SCOREP_RegionHandle into a string.
 */
extern const char*
scorep_region_to_string( char*               stringBuffer,
                         size_t              stringBufferSize,
                         const char*         format,
                         SCOREP_RegionHandle regionHandle )
{
    return scorep_any_handle_to_string( stringBuffer,
                                        stringBufferSize,
                                        format,
                                        regionHandle,
                                        SCOREP_INVALID_REGION );
}


/**
 * Converts a SCOREP_LocalMPICommunicatorHandle into a string.
 */
extern const char*
scorep_comm_to_string( char*                             stringBuffer,
                       size_t                            stringBufferSize,
                       const char*                       format,
                       SCOREP_LocalMPICommunicatorHandle commHandle )
{
    return scorep_any_handle_to_string( stringBuffer,
                                        stringBufferSize,
                                        format,
                                        commHandle,
                                        SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR );
}


/**
 * Converts a SCOREP_MPIWindowHandle into a string.
 */
extern const char*
scorep_window_to_string( char*                  stringBuffer,
                         size_t                 stringBufferSize,
                         const char*            format,
                         SCOREP_MPIWindowHandle windowHandle )
{
    return scorep_any_handle_to_string( stringBuffer,
                                        stringBufferSize,
                                        format,
                                        windowHandle,
                                        SCOREP_INVALID_MPI_WINDOW );
}

/**
 * Converts a SCOREP_MPICartTopolHandle into a string.
 */
const char*
scorep_mpi_cart_topol_to_string( char*                             stringBuffer,
                                 size_t                            stringBufferSize,
                                 const char*                       format,
                                 SCOREP_MPICartesianTopologyHandle cartHandle )
{
    return scorep_any_handle_to_string( stringBuffer,
                                        stringBufferSize,
                                        format,
                                        cartHandle,
                                        SCOREP_INVALID_CART_TOPOLOGY );
}
