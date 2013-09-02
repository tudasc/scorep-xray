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
 * @file strutil.h
 *
 * Strings utility functions
 */


#ifndef STRUTIL_H_INCLUDED
#define STRUTIL_H_INCLUDED

#include <stddef.h>

typedef struct
{
    char* key;
    char* val;
}
kv_pair;


char*
mystrndup( const char* str,
           size_t      len );

size_t
make_key_value_pairs( const char* str,
                      kv_pair**   kv );


#endif /* STRUTIL_H_INCLUDED */
