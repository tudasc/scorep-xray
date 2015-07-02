/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#include <config.h>

#include <stdio.h>

#include "foo.h"

#include <SCOREP_Libwrap_Macros.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Libwrap.h>

/* Library wrapper object */
static SCOREP_LibwrapHandle* lw = SCOREP_LIBWRAP_NULL;

const char* my_lib_name[ 1 ] = { "foo.so" };

/* Library wrapper attributes */
static SCOREP_LibwrapAttributes lw_attr =
{
    SCOREP_LIBWRAP_MODE,
    1,
    my_lib_name
};

#ifdef SCOREP_LIBWRAP_STATIC

void
__real_foo( void );

#endif

/*
 * Function wrapper
 */
void
SCOREP_LIBWRAP_FUNC_NAME( foo ) ( void )
{
    /* Initialize the measurement system */
    SCOREP_InitMeasurement();
    SCOREP_Libwrap_Initialize();

    printf( "Wrapped function 'foo'\n" );

    SCOREP_LIBWRAP_FUNC_INIT( lw, lw_attr,
                              void, foo, ( void ),
                              "example.h", 0 );

    SCOREP_LIBWRAP_FUNC_ENTER

    SCOREP_LIBWRAP_FUNC_CALL( lw,
                              foo,
                              ( ) );

    SCOREP_LIBWRAP_FUNC_EXIT

    SCOREP_Libwrap_Finalize();
}
