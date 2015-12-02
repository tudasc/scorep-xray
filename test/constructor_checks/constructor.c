/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
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

#if SCOREP_COMPILER_CONSTRUCTOR_MODE == SCOREP_COMPILER_CONSTRUCTOR_MODE_ATTRIBUTE

void
__attribute__( ( constructor ) )
test_constructor( void );

#elif SCOREP_COMPILER_CONSTRUCTOR_MODE == SCOREP_COMPILER_CONSTRUCTOR_MODE_PRAGMA

void
test_constructor( void );

#pragma init(test_constructor)

#endif

void
test_constructor( void )
{
    _Exit( EXIT_FAILURE );
}
