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
 * @file       test/measurement/config/error_callback.c
 *
 *
 */

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>

#include <CuTest.h>

#include <UTILS_Error.h>

#include "error_callback.h"


SCOREP_ErrorCode
cutest_scorep_error_callback( void*            userData,
                              const char*      file,
                              uint64_t         line,
                              const char*      function,
                              SCOREP_ErrorCode errorCode,
                              const char*      msgFormatString,
                              va_list          va )
{
    CuTest*  the_test = userData;
    CuString message2, message;

    CuStringInit( &message2 );
    CuStringAppendFormat( &message2, "In function '%s': error: %s",
                          function,
                          SCOREP_Error_GetDescription( errorCode ) );

    CuStringInit( &message );
    CuStringAppendVFormat( &message, msgFormatString, va );

    CuFail_Line( the_test, file, line,
                 message2.buffer, message.buffer );
    CuStringClear( &message2 );
    CuStringClear( &message );

    return errorCode;
}
