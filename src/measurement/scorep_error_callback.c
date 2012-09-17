/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       src/measurement/scorep_error_callback.c
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>


#include <UTILS_Error.h>


SCOREP_ErrorCode
scorep_error_callback( void*            userData,
                       const char*      file,
                       uint64_t         line,
                       const char*      function,
                       SCOREP_ErrorCode errorCode,
                       const char*      msgFormatString,
                       va_list          va )
{
    size_t msg_format_string_length = msgFormatString ?
                                      strlen( msgFormatString ) : 0;
    const char* type = "Error";
#if HAVE( SCOREP_DEBUG )
    const char* description        = "";
    const char* description_prefix = "";
#endif
    if ( errorCode == SCOREP_WARNING )
    {
        /* return, if the verbosity level is too low */
        type = "Warning";
    }
    else if ( errorCode == SCOREP_DEPRECATED )
    {
        /* return, if the verbosity level is too low */
        /* This should not happen */
        type = "Deprecated";
    }
    else if ( errorCode == SCOREP_ABORT )
    {
        type = "Fatal";
    }
    /* SCOREP_ERROR_* => return, if the verbosity level is too low */
#if HAVE( SCOREP_DEBUG )
    else
    {
        description        = SCOREP_Error_GetDescription( errorCode );
        description_prefix = ": ";
    }
#endif

#if HAVE( SCOREP_DEBUG )
    fprintf( stderr, "[%s] %s:%" PRIu64 ": %s%s%s%s",
             PACKAGE_NAME, file, line,
             type, description_prefix, description,
             msg_format_string_length ? ": " : "\n" );
#else

    /*
     * @todo extract module from filename (ie. last directory component
     * or scorep_<module>_foo.c)
     */
    fprintf( stderr, "[%s] %s%s",
             PACKAGE_NAME,
             type,
             msg_format_string_length ? ": " : "\n" );
#endif

    if ( msg_format_string_length )
    {
        vfprintf( stderr, msgFormatString, va );
        fprintf( stderr, "\n" );
    }

    return errorCode;
}


void
SCOREP_InitErrorCallback( void )
{
    SCOREP_Error_RegisterCallback( scorep_error_callback, NULL );
}
