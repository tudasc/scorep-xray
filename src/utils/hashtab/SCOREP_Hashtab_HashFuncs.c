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
 * Copyright (c) 2009-2011, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2009-2011,
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 * @ingroup         SCOREP_Hashtab_module
 *
 * @brief           Provides simple hash functions for standard data types.
 *
 * Standard data types like integers or strings, are frequently used
 * as keys in the hash table. Thus, hash functions for simple types are
 * provided at a central place.
 */

#include <config.h>
#include <SCOREP_Hashtab.h>


size_t
SCOREP_Hashtab_HashString( const void* key )
{
    const unsigned char* str  = ( const unsigned char* )key;
    size_t               hash = 0;

    /* one-at-a-time hashing */
    while ( *str != 0 )
    {
        hash += *str++;
        hash += hash << 10;
        hash ^= hash >>  6;
    }

    hash += hash <<  3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}

size_t
SCOREP_Hashtab_HashInt64( const void* key )
{
    return *( uint64_t* )key * UINT64_C( 11400714819323199488 );
}

size_t
SCOREP_Hashtab_HashInt32( const void* key )
{
    return *( uint32_t* )key * UINT32_C( 2654435761l );
}

size_t
SCOREP_Hashtab_HashPointer( const void* key )
{
    if ( sizeof( size_t ) == sizeof( uint64_t ) )
    {
        return ( ( ( size_t )key ) / 8 ) * UINT64_C( 11400714819323199488 );
    }
    else
    {
        return ( ( ( size_t )key ) / 8 ) * UINT32_C( 2654435761l );
    }
}
