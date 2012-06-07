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

#ifndef SCOREP_CSTR_H
#define SCOREP_CSTR_H

#include <stddef.h>

#ifdef __cplusplus
#  define EXTERN extern "C"
#else
#  define EXTERN extern
#endif

/**
 * @file            MANGLE_NAME( CStr.h )
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status          ALPHA
 *
 * @brief           Declare helper functions for string handling
 */

/**
   Defines a C value analog to std::string::npos
 */
EXTERN const size_t MANGLE_NAME( CStr_npos );

/**
   Duplicates the string @a source.
 */
EXTERN char*
MANGLE_NAME( CStr_dup ) ( const char* source );

/**
   Searches for the first appearance of substring @a pattern in the string
   @a str after position @a pos.
   @param str     A string where the function tries to find the given pattern.
   @param pattern The string which is searched for in @a str.
   @param pos     The index of the first character in @a str, where the search
                  is started.
   @return If an appearance of @a pattern is found in @a str. It returns the
           index of the first letter of the first appearance of @a pattern in
           @a str. If the pattern is not found in @a str, it returns
           MANGLE_NAME( CStr_npos ).
 */
EXTERN size_t
MANGLE_NAME( CStr_find ) ( const char* str,
                           const char* pattern,
                           size_t pos );

#endif /* SCOREP_CSTR_H */
