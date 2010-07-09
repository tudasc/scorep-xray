/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       test/measurement/config/error_callback.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>


#include <CuTest.h>


#include <SILC_Error.h>


#include "error_callback.h"


/* global, so that the error callback can access it */
CuTest* the_test = NULL;


int64_t
cutest_silc_error_callback( const char*           function,
                            const char*           file,
                            const uint64_t        line,
                            const SILC_Error_Code errorCode,
                            const char*           msgFormatString,
                            va_list               va )
{
    CuString message2, message;

    CuStringInit( &message2 );
    CuStringAppendFormat( &message2, "In function '%s': error: %s",
                          function,
                          SILC_Error_GetDescription( errorCode ) );

    CuStringInit( &message );
    CuStringAppendVFormat( &message, msgFormatString, va );

    CuFail_Line( the_test, file, line,
                 message2.buffer, message.buffer );
    CuStringClear( &message2 );
    CuStringClear( &message );
}
