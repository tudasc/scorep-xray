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
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup         SCOREP_Hashtab_module
 *
 * @brief           Provides simple hash functions for standard data types.
 *
 * Standard data types like integers or strings, are frequently used
 * as keys in the hash table. Thus, comparison functions for simple types are
 * provided at a central place.
 */

#include <config.h>
#include <SCOREP_Hashtab.h>

#include <string.h>


int32_t
SCOREP_Hashtab_CompareStrings( const void* key,
                               const void* item_key )
{
    return strcmp( ( const char* )key, ( const char* )item_key );
}

int32_t
SCOREP_Hashtab_CompareInt8( const void* key,
                            const void* item_key )
{
    return *( const int8_t* )key == *( const int8_t* )item_key ? 0 : 1;
}

int32_t
SCOREP_Hashtab_CompareInt16( const void* key,
                             const void* item_key )
{
    return *( const int16_t* )key == *( const int16_t* )item_key ? 0 : 1;
}

int32_t
SCOREP_Hashtab_CompareInt32( const void* key,
                             const void* item_key )
{
    return *( const int32_t* )key == *( const int32_t* )item_key ? 0 : 1;
}

int32_t
SCOREP_Hashtab_CompareInt64( const void* key,
                             const void* item_key )
{
    return *( const int64_t* )key == *( const int64_t* )item_key ? 0 : 1;
}

int32_t
SCOREP_Hashtab_CompareUint32( const void* key,
                              const void* item_key )
{
    return *( const uint32_t* )key != *( const uint32_t* )item_key;
}

int32_t
SCOREP_Hashtab_CompareUint64( const void* key,
                              const void* item_key )
{
    return *( const uint64_t* )key != *( const uint64_t* )item_key;
}

int32_t
SCOREP_Hashtab_ComparePointer( const void* key,
                               const void* item_key )
{
    return key == item_key ? 0 : 1;
}
