/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if SCOREP_COMPILER_CONSTRUCTOR_MODE == SCOREP_COMPILER_CONSTRUCTOR_MODE_ATTRIBUTE

void
__attribute__( ( constructor ) )
test_constructor( SCOREP_COMPILER_CONSTRUCTOR_PROTO_ARGS );

#elif SCOREP_COMPILER_CONSTRUCTOR_MODE == SCOREP_COMPILER_CONSTRUCTOR_MODE_PRAGMA

void
test_constructor( SCOREP_COMPILER_CONSTRUCTOR_PROTO_ARGS );

#pragma init(test_constructor)

#endif

void
test_constructor( SCOREP_COMPILER_CONSTRUCTOR_PROTO_ARGS )
{
#if HAVE( COMPILER_CONSTRUCTOR_ARGS )
    /* inverted expectations */
    if ( argc != 1 )
    {
        _Exit( EXIT_SUCCESS );
    }

    const char* argv0 = argv[ 0 ];
    if ( strrchr( argv0, '/' ) )
    {
        argv0 = strrchr( argv0, '/' ) + 1;
    }

    /* inverted expectations */
    if ( 0 != strncmp( argv0, "test_constructor_check_", 23 ) )
    {
        _Exit( EXIT_SUCCESS );
    }
#endif

    /* inverted expectations */
    _Exit( EXIT_FAILURE );
}
