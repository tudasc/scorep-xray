/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 * @brief Demangling
 */

#ifndef SCOREP_COMPILER_DEMANGLE_H
#define SCOREP_COMPILER_DEMANGLE_H

#include <stdlib.h>

#if HAVE( DEMANGLE )
/* Declaration of external demangling function */
/* It is contained in "demangle.h" */
extern char*
cplus_demangle( const char* mangled,
                int         options );

/* cplus_demangle options */
#define SCOREP_COMPILER_DEMANGLE_NO_OPTS   0
#define SCOREP_COMPILER_DEMANGLE_PARAMS    ( 1 << 0 )  /* include function arguments */
#define SCOREP_COMPILER_DEMANGLE_ANSI      ( 1 << 1 )  /* include const, volatile, etc. */
#define SCOREP_COMPILER_DEMANGLE_VERBOSE   ( 1 << 3 )  /* include implementation details */
#define SCOREP_COMPILER_DEMANGLE_TYPES     ( 1 << 4 )  /* include type encodings */

/* Demangling style. */
static int scorep_compiler_demangle_style = SCOREP_COMPILER_DEMANGLE_PARAMS  |
                                            SCOREP_COMPILER_DEMANGLE_ANSI    |
                                            SCOREP_COMPILER_DEMANGLE_VERBOSE |
                                            SCOREP_COMPILER_DEMANGLE_TYPES;

#define scorep_compiler_demangle( mangled, demangled ) \
    do \
    { \
        demangled = cplus_demangle( mangled, scorep_compiler_demangle_style ); \
        if ( demangled == NULL ) \
        { \
            demangled = mangled; \
            mangled   = NULL; \
        } \
    } \
    while ( 0 )

#else /* !HAVE( DEMANGLE ) */

#define scorep_compiler_demangle( mangled, demangled ) \
    do \
    { \
        demangled = mangled; \
        mangled   = NULL; \
    } \
    while ( 0 )

#endif /* !HAVE( DEMANGLE ) */

#define scorep_compiler_demangle_free( mangled, demangled ) \
    do \
    { \
        if ( mangled && demangled ) \
        { \
            free( demangled ); \
        } \
    } \
    while ( 0 )

#endif /* SCOREP_COMPILER_DEMANGLE_H */
