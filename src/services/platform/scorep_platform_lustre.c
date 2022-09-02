/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020, 2022,
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

#include <SCOREP_Platform.h>
#include <SCOREP_Definitions.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>

#include <lustre/lustreapi.h>

#define SAFE_PROPERTY_STRING_LENGTH 32

#if !HAVE( DECL_LLAPI_LAYOUT_GET_BY_PATH )

static inline void*
allocate_lum( void )
{
    size_t v1 = sizeof( struct lov_user_md_v1 ) +
                LOV_MAX_STRIPE_COUNT * sizeof( struct lov_user_ost_data_v1 );

    size_t v3 = sizeof( struct lov_user_md_v3 ) +
                LOV_MAX_STRIPE_COUNT * sizeof( struct lov_user_ost_data_v1 );

    return malloc( v1 > v3 ? v1 : v3 );
}

#endif

void
SCOREP_Platform_AddLustreProperties( SCOREP_IoFileHandle ioFileHandle )
{
    const char* file_path = SCOREP_IoFileHandle_GetFileName( ioFileHandle );

#if !HAVE( DECL_LLAPI_LAYOUT_GET_BY_PATH )

    struct lov_user_md* lum_file = allocate_lum();
    int                 ret      = llapi_file_get_stripe( file_path, lum_file );
    if ( ret == 0 )
    {
        char count_str[ SAFE_PROPERTY_STRING_LENGTH ];
        char size_str[ SAFE_PROPERTY_STRING_LENGTH ];

        snprintf( count_str, SAFE_PROPERTY_STRING_LENGTH, "%" PRIu32, lum_file->lmm_stripe_count );
        snprintf( size_str, SAFE_PROPERTY_STRING_LENGTH, "%" PRIu32, lum_file->lmm_stripe_size );

        SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Stripe Count", count_str );
        SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Stripe Size", size_str );
        SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Extent Begin", "0" );
        SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Number of Extents", "1" );
    }
    free( lum_file );

#else

    struct llapi_layout* layout = llapi_layout_get_by_path( file_path, LAYOUT_GET_EXPECTED );
    if ( layout == NULL )
    {
        return;
    }

    int      rc                   = llapi_layout_comp_use( layout, LLAPI_LAYOUT_COMP_USE_FIRST );
    uint64_t number_of_components = 0;
    while ( rc == 0 )
    {
        number_of_components++;
        rc = llapi_layout_comp_use( layout, LLAPI_LAYOUT_COMP_USE_NEXT );
    }

    size_t bytes_per_buffer      = number_of_components * SAFE_PROPERTY_STRING_LENGTH * sizeof( char );
    char*  count_buffer          = calloc( 1, bytes_per_buffer );
    size_t count_buffer_position = 0;
    char*  size_buffer           = calloc( 1, bytes_per_buffer );
    size_t size_buffer_position  = 0;
    char*  begin_buffer          = calloc( 1, bytes_per_buffer );
    size_t begin_buffer_position = 0;

    rc = llapi_layout_comp_use( layout, LLAPI_LAYOUT_COMP_USE_FIRST );
    const char* sep = "";
    while ( rc == 0 )
    {
        uint64_t count;
        llapi_layout_stripe_count_get( layout, &count );
        if ( count == LLAPI_LAYOUT_DEFAULT )
        {
            count_buffer_position += snprintf( count_buffer + count_buffer_position,
                                               SAFE_PROPERTY_STRING_LENGTH,
                                               "%sDEFAULT", sep );
        }
        else if ( count == LLAPI_LAYOUT_WIDE )
        {
            count_buffer_position += snprintf( count_buffer + count_buffer_position,
                                               SAFE_PROPERTY_STRING_LENGTH,
                                               "%sWIDE", sep );
        }
        else
        {
            count_buffer_position += snprintf( count_buffer + count_buffer_position,
                                               SAFE_PROPERTY_STRING_LENGTH,
                                               "%s%" PRIu64, sep, count );
        }

        uint64_t size;
        llapi_layout_stripe_size_get( layout, &size );
        if ( size == LLAPI_LAYOUT_DEFAULT )
        {
            size_buffer_position += snprintf( size_buffer + size_buffer_position,
                                              SAFE_PROPERTY_STRING_LENGTH,
                                              "%sDEFAULT", sep );
        }
        else
        {
            size_buffer_position += snprintf( size_buffer + size_buffer_position,
                                              SAFE_PROPERTY_STRING_LENGTH,
                                              "%s%" PRIu64, sep, size );
        }

        uint64_t begin;
        uint64_t end_unused;
        llapi_layout_comp_extent_get( layout, &begin, &end_unused );
        begin_buffer_position += snprintf( begin_buffer + begin_buffer_position,
                                           SAFE_PROPERTY_STRING_LENGTH,
                                           "%s%" PRIu64, sep, begin );

        sep = ", ";

        rc = llapi_layout_comp_use( layout, LLAPI_LAYOUT_COMP_USE_NEXT );
    }

    SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Stripe Count", count_buffer );
    SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Stripe Size", size_buffer );
    SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Extent Begin", begin_buffer );

    char number_buffer[ SAFE_PROPERTY_STRING_LENGTH ];
    snprintf( number_buffer, SAFE_PROPERTY_STRING_LENGTH, "%" PRIu64, number_of_components );
    SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Number of Extents", number_buffer );

    free( count_buffer );
    free( size_buffer );
    free( begin_buffer );

    llapi_layout_free( layout );

#endif
}
