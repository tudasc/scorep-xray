/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_DEMANGLE_H
#define SCOREP_DEMANGLE_H

/**
 * @file
 *
 * @brief Provide demangling functionality.
 */

/* Options passed to SCOREP_Demangle() as 2nd parameter. */
#define SCOREP_DEMANGLE_NO_OPTS 0
#define SCOREP_DEMANGLE_PARAMS  ( 1 << 0 )  /* include function arguments */
#define SCOREP_DEMANGLE_ANSI    ( 1 << 1 )  /* include const, volatile, etc. */
/* (1 << 2) Demangle as Java rather than C++. */
#define SCOREP_DEMANGLE_VERBOSE ( 1 << 3 )  /* include implementation details */
#define SCOREP_DEMANGLE_TYPES   ( 1 << 4 )  /* include type encodings */

#define SCOREP_DEMANGLE_DEFAULT \
    ( SCOREP_DEMANGLE_PARAMS  | \
      SCOREP_DEMANGLE_ANSI    | \
      SCOREP_DEMANGLE_VERBOSE | \
      SCOREP_DEMANGLE_TYPES )

/**
 * This is a wrapper around cplus_demangle.
 * If @a mangledName is a g++ v3 ABI mangled name, return a buffer allocated
 * with malloc holding the demangled name. Otherwise, or if demangling is not
 * available, return NULL. Pass SCOREP_DEMANGLE_* to @a options to control
 * the output.
 */
static inline char*
SCOREP_Demangle( const char* mangledName,
                 int         options )
{
#if HAVE( SCOREP_DEMANGLE )
    extern char*
    cplus_demangle( const char* mangled,
                    int         options );

    return cplus_demangle( mangledName, options );
#else /* !HAVE( SCOREP_DEMANGLE ) */
    return NULL;
#endif /* !HAVE( SCOREP_DEMANGLE ) */
}

#endif /* SCOREP_DEMANGLE_H */
