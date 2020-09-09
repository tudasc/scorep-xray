/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015, 2018-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012, 2018
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  This file containes the implementation of user adapter functions concerning
 *  regions.
 */

#include <config.h>
#include "scorep_user_region.h"

#include <string.h>

#include <scorep/SCOREP_User_Functions.h>
#include "SCOREP_User_Init.h"
#include <SCOREP_Types.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Hashtab.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>
#include <SCOREP_OA_Functions.h>
#include <SCOREP_Fortran_Wrapper.h>
#include "scorep_selective_region.h"
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Memory.h>

#define SCOREP_F_Begin_U SCOREP_F_BEGIN
#define SCOREP_F_RegionByNameBegin_U SCOREP_F_REGIONBYNAMEBEGIN
#define SCOREP_F_RewindBegin_U SCOREP_F_REWINDBEGIN
#define SCOREP_F_Init_U SCOREP_F_INIT
#define SCOREP_F_RegionEnd_U SCOREP_F_REGIONEND
#define SCOREP_F_RegionByNameEnd_U SCOREP_F_REGIONBYNAMEEND
#define SCOREP_F_RewindRegionEnd_U SCOREP_F_REWINDREGIONEND
#define SCOREP_F_RegionEnter_U SCOREP_F_REGIONENTER
#define SCOREP_F_OaBegin_U SCOREP_F_OABEGIN
#define SCOREP_F_OaEnd_U SCOREP_F_OAEND

#define SCOREP_F_Begin_L scorep_f_begin
#define SCOREP_F_RegionByNameBegin_L scorep_f_regionbynamebegin
#define SCOREP_F_RewindBegin_L scorep_f_rewindbegin
#define SCOREP_F_Init_L scorep_f_init
#define SCOREP_F_RegionEnd_L scorep_f_regionend
#define SCOREP_F_RegionByNameEnd_L scorep_f_regionbynameend
#define SCOREP_F_RewindRegionEnd_L scorep_f_rewindregionend
#define SCOREP_F_RegionEnter_L scorep_f_regionenter
#define SCOREP_F_OaBegin_L scorep_f_oabegin
#define SCOREP_F_OaEnd_L scorep_f_oaend


extern SCOREP_Mutex    scorep_user_region_mutex;
extern SCOREP_Hashtab* scorep_user_region_table;

extern SCOREP_RegionType
scorep_user_to_scorep_region_type( const SCOREP_User_RegionType user_type );

/* *INDENT-OFF* */
static void region_init_fortran( SCOREP_Fortran_RegionHandle*, char*, int32_t*, char*, int32_t*, scorep_fortran_charlen_t, scorep_fortran_charlen_t );
/* *INDENT-ON* */

static inline SCOREP_User_RegionHandle
find_region( char* region_name )
{
    SCOREP_Hashtab_Entry* entry = SCOREP_Hashtab_Find( scorep_user_region_table,
                                                       region_name, NULL );
    if ( !entry )
    {
        return SCOREP_USER_INVALID_REGION;
    }

    return ( SCOREP_User_RegionHandle )entry->value.ptr;
}

static inline void
add_region( SCOREP_User_RegionHandle region_handle,
            char*                    region_name )
{
    UTILS_BUG_ON( region_handle == SCOREP_USER_INVALID_REGION );
    UTILS_BUG_ON( region_name == NULL );

    SCOREP_Hashtab_InsertPtr( scorep_user_region_table,
                              ( void* )UTILS_CStr_dup( region_name ),
                              ( void* )region_handle,
                              NULL );
}


void
FSUB( SCOREP_F_Init )( SCOREP_Fortran_RegionHandle* regionHandle,
                       char*                        regionNameF,
                       int32_t*                     regionType,
                       char*                        fileNameF,
                       int32_t*                     lineNo,
                       scorep_fortran_charlen_t     regionNameLen,
                       scorep_fortran_charlen_t     fileNameLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    region_init_fortran( regionHandle, regionNameF, regionType, fileNameF, lineNo, regionNameLen, fileNameLen );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_Begin )( SCOREP_Fortran_RegionHandle* regionHandle,
                        char*                        regionNameF,
                        int32_t*                     regionType,
                        char*                        fileNameF,
                        int32_t*                     lineNo,
                        scorep_fortran_charlen_t     regionNameLen,
                        scorep_fortran_charlen_t     fileNameLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
#if !( defined( __PGI ) && __PGIC__ < 18 )
        if ( *regionHandle == SCOREP_FORTRAN_INVALID_REGION )
#endif
        {
            /* Make sure the handle is initialized */
            region_init_fortran( regionHandle,
                                 regionNameF,
                                 regionType,
                                 fileNameF,
                                 lineNo,
                                 regionNameLen,
                                 fileNameLen );
        }
        /* Generate region event */
        scorep_user_region_enter( SCOREP_F2C_REGION( *regionHandle ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_RegionByNameBegin )( char*                    regionNameF,
                                    int32_t*                 regionType,
                                    char*                    fileNameF,
                                    int32_t*                 lineNo,
                                    scorep_fortran_charlen_t regionNameLen,
                                    scorep_fortran_charlen_t fileNameLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    /* Copy strings */
    char* region_name = ( char* )malloc( ( regionNameLen + 1 ) * sizeof( char ) );
    strncpy( region_name, regionNameF, regionNameLen );
    region_name[ regionNameLen ] = '\0';

    char* file_name = ( char* )malloc( ( fileNameLen + 1 ) * sizeof( char ) );
    strncpy( file_name, fileNameF, fileNameLen );
    file_name[ fileNameLen ] = '\0';
    UTILS_IO_SimplifyPath( file_name );

    scorep_user_region_by_name_begin( region_name,
                                      ( SCOREP_User_RegionType )( *regionType ),
                                      file_name,
                                      ( uint32_t )*lineNo );

    /* Cleanup */
    free( region_name );
    free( file_name );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_RewindBegin )( SCOREP_Fortran_RegionHandle* handle,
                              char*                        name_f,
                              int32_t*                     type,
                              char*                        fileName_f,
                              int32_t*                     lineNo,
                              scorep_fortran_charlen_t     nameLen,
                              scorep_fortran_charlen_t     fileNameLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
#if !( defined( __PGI ) && __PGIC__ < 18 )
        if ( *handle == SCOREP_FORTRAN_INVALID_REGION )
#endif
        {
            /* Make sure the handle is initialized */
            region_init_fortran( handle, name_f, type, fileName_f,
                                 lineNo, nameLen, fileNameLen );
        }
        /* Generate rewind point */
        scorep_user_rewind_region_enter( SCOREP_F2C_REGION( *handle ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_RegionEnd )( SCOREP_Fortran_RegionHandle* regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        scorep_user_region_exit( SCOREP_F2C_REGION( *regionHandle ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
FSUB( SCOREP_F_RegionByNameEnd )( char*                    regionNameF,
                                  scorep_fortran_charlen_t regionNameLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        /* Copy strings */
        char* region_name = ( char* )malloc( ( regionNameLen + 1 ) * sizeof( char ) );
        strncpy( region_name, regionNameF, regionNameLen );
        region_name[ regionNameLen ] = '\0';

        scorep_user_region_by_name_end( region_name );

        free( region_name );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_RewindRegionEnd )( SCOREP_Fortran_RegionHandle* handle, int* value )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        scorep_user_rewind_region_exit( SCOREP_F2C_REGION( *handle ), *value == 1 );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_RegionEnter )( SCOREP_Fortran_RegionHandle* regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        if ( *regionHandle == SCOREP_FORTRAN_INVALID_REGION )
        {
            UTILS_FATAL( "Enter for uninitialized region handle. Use "
                         "SCOREP_USER_REGION_INIT or SCOREP_USER_BEGIN "
                         "to ensure that handles are initialized." );
        }
        scorep_user_region_enter( SCOREP_F2C_REGION( *regionHandle ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_OaBegin )( SCOREP_Fortran_RegionHandle* regionHandle,
                          char*                        regionNameF,
                          int32_t*                     regionType,
                          char*                        fileNameF,
                          int32_t*                     lineNo,
                          scorep_fortran_charlen_t     regionNameLen,
                          scorep_fortran_charlen_t     fileNameLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
#if !( defined( __PGI ) && __PGIC__ < 18 )
        if ( *regionHandle == SCOREP_FORTRAN_INVALID_REGION )
#endif
        {
            /* Make sure the handle is initialized */
            region_init_fortran( regionHandle,
                                 regionNameF,
                                 regionType,
                                 fileNameF,
                                 lineNo,
                                 regionNameLen,
                                 fileNameLen );
        }

        SCOREP_User_RegionHandle handle = SCOREP_F2C_REGION( *regionHandle );

        /* Special phase logic */
        SCOREP_OA_PhaseBegin( handle->handle );

        /* Generate region event */
        scorep_user_region_enter( handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( SCOREP_F_OaEnd )( SCOREP_Fortran_RegionHandle* regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_User_RegionHandle handle = SCOREP_F2C_REGION( *regionHandle );

        scorep_user_region_exit( handle );
        SCOREP_OA_PhaseEnd( handle->handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static void
region_init_fortran( SCOREP_Fortran_RegionHandle* regionHandle,
                     char*                        regionNameF,
                     int32_t*                     regionType,
                     char*                        fileNameF,
                     int32_t*                     lineNo,
                     scorep_fortran_charlen_t     regionNameLen,
                     scorep_fortran_charlen_t     fileNameLen )
{
    /* Copy strings */
    char* region_name = ( char* )malloc( ( regionNameLen + 1 ) * sizeof( char ) );
    strncpy( region_name, regionNameF, regionNameLen );
    region_name[ regionNameLen ] = '\0';

    char* file_name = ( char* )malloc( ( fileNameLen + 1 ) * sizeof( char ) );
    strncpy( file_name, fileNameF, fileNameLen );
    file_name[ fileNameLen ] = '\0';

    /* Get source file handle.
       The definitions check for double entries. */
    SCOREP_SourceFileHandle file_handle = SCOREP_Definitions_NewSourceFile( file_name );

    /* Lock region definition */
    SCOREP_MutexLock( scorep_user_region_mutex );

    /* Lookup the region name in the region table */
    SCOREP_User_RegionHandle region = find_region( region_name );

    /* Test whether the handle is still invalid, or if it was initialized in the mean
       time. If the handle is invalid, register a new region */
    if ( region == SCOREP_USER_INVALID_REGION )
    {
        /* Translate region type from user adapter type to SCOREP measurement type */
        SCOREP_RegionType region_type = scorep_user_to_scorep_region_type( *regionType );

        /* Check for filters */
        if ( SCOREP_Filtering_Match( file_name, region_name, NULL ) ||
             ( strncmp( region_name, "POMP", 4 ) == 0 ) ||
             ( strncmp( region_name, "Pomp", 4 ) == 0 ) ||
             ( strncmp( region_name, "pomp", 4 ) == 0 ) )
        {
            region = SCOREP_FILTERED_USER_REGION;
            add_region( region, region_name );
        }
        else
        {
            region = scorep_user_create_region( region_name );

            if ( region != SCOREP_USER_INVALID_REGION )
            {
                region->handle = SCOREP_Definitions_NewRegion( region_name,
                                                               NULL,
                                                               file_handle,
                                                               *lineNo,
                                                               SCOREP_INVALID_LINE_NO,
                                                               SCOREP_PARADIGM_USER,
                                                               region_type );

                add_region( region, region_name );
            }
        }
    }
    *regionHandle = SCOREP_C2F_REGION( region );

    /* Unlock region definition */
    SCOREP_MutexUnlock( scorep_user_region_mutex );

    /* Cleanup */
    free( region_name );
    free( file_name );
}
