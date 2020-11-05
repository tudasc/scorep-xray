/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020,
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

#define MAX_UINT32_STRING_LENGTH 12

static inline void*
allocate_lum( void )
{
    size_t v1 = sizeof( struct lov_user_md_v1 ) +
                LOV_MAX_STRIPE_COUNT * sizeof( struct lov_user_ost_data_v1 );

    size_t v3 = sizeof( struct lov_user_md_v3 ) +
                LOV_MAX_STRIPE_COUNT * sizeof( struct lov_user_ost_data_v1 );

    return malloc( v1 > v3 ? v1 : v3 );
}

void
SCOREP_Platform_AddLustreProperties( SCOREP_IoFileHandle ioFileHandle )
{
    const char*         file_path = SCOREP_Definitions_GetIoFileName( ioFileHandle );
    struct lov_user_md* lum_file  = allocate_lum();
    int                 ret       = llapi_file_get_stripe( file_path, lum_file );
    if ( ret == 0 )
    {
        char count_str[ MAX_UINT32_STRING_LENGTH ];
        char size_str[ MAX_UINT32_STRING_LENGTH ];

        snprintf( count_str, MAX_UINT32_STRING_LENGTH, "%" PRIu32, lum_file->lmm_stripe_count );
        snprintf( size_str, MAX_UINT32_STRING_LENGTH, "%" PRIu32, lum_file->lmm_stripe_size );

        SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Stripe Count", count_str );
        SCOREP_IoFileHandle_AddProperty( ioFileHandle, "Stripe Size", size_str );
    }
    free( lum_file );
}
