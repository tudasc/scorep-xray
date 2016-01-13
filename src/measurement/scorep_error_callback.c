/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


#include <config.h>

#include <SCOREP_ErrorCodes.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stddef.h>


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
//#if HAVE( SCOREP_DEBUG )
    const char* description        = "";
    const char* description_prefix = "";
//#endif
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
//#if HAVE( SCOREP_DEBUG )
    else
    {
        description        = SCOREP_Error_GetDescription( errorCode );
        description_prefix = ": ";
    }
//#endif

//#if HAVE( SCOREP_DEBUG )
    fprintf( stderr, "[%s] %s:%" PRIu64 ": %s%s%s%s",
             PACKAGE_NAME, file, line,
             type, description_prefix, description,
             msg_format_string_length ? ": " : "\n" );
//#else
//
//    /*
//     * @todo extract module from filename (ie. last directory component
//     * or scorep_<module>_foo.c)
//     */
//    fprintf( stderr, "[%s] %s%s",
//             PACKAGE_NAME,
//             type,
//             msg_format_string_length ? ": " : "\n" );
//#endif

    if ( msg_format_string_length )
    {
        vfprintf( stderr, msgFormatString, va );
        fprintf( stderr, "\n" );
    }

    /* This could only be printed from production code */
    if ( errorCode == SCOREP_ABORT )
    {
        fprintf( stderr, "[%s] Please report this to %s. Thank you.\n",
                 PACKAGE_NAME,
                 PACKAGE_BUGREPORT );
        fprintf( stderr, "[%s] Try also to preserve any generated core dumps.\n",
                 PACKAGE_NAME );
    }

    return errorCode;
}


void
SCOREP_InitErrorCallback( void )
{
    SCOREP_Error_RegisterCallback( scorep_error_callback, NULL );
}
