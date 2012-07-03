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
 *  @file       SCOREP_User_RegionF.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the implementation of user adapter functions concerning
 *  regions.
 */

#include <config.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Types.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Filter.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_CStr.h>
#include <SCOREP_IO.h>
#include <SCOREP_Fortran_Wrapper.h>
#include <scorep_selective_region.h>

#define SCOREP_F_Begin_U SCOREP_F_BEGIN
#define SCOREP_F_Init_U SCOREP_F_INIT
#define SCOREP_F_RegionEnd_U SCOREP_F_REGIONEND
#define SCOREP_F_RegionEnter_U SCOREP_F_REGIONENTER

#define SCOREP_F_Begin_L scorep_f_begin
#define SCOREP_F_Init_L scorep_f_init
#define SCOREP_F_RegionEnd_L scorep_f_regionend
#define SCOREP_F_RegionEnter_L scorep_f_regionenter

#define SCOREP_FILTERED_USER_REGION ( ( void* )-1 )

extern SCOREP_Hashtab* scorep_user_file_table;
extern SCOREP_Mutex    scorep_user_file_table_mutex;
extern SCOREP_Mutex    scorep_user_region_mutex;
extern SCOREP_Hashtab* scorep_user_region_table;

extern SCOREP_RegionType
scorep_user_to_scorep_region_type( const SCOREP_User_RegionType user_type );


static inline SCOREP_User_RegionHandle
scorep_user_find_region( char* name )
{
    SCOREP_Hashtab_Entry* entry = SCOREP_Hashtab_Find( scorep_user_region_table, name, NULL );
    if ( !entry )
    {
        return SCOREP_USER_INVALID_REGION;
    }

    return ( SCOREP_User_RegionHandle )entry->value;
}

static void
scorep_user_add_region( SCOREP_User_RegionHandle region,
                        char*                    name )
{
    assert( region != SCOREP_USER_INVALID_REGION );
    assert( name );

    SCOREP_Hashtab_Insert( scorep_user_region_table, ( void* )SCOREP_CStr_dup( name ), region, NULL );
}


void
FSUB( SCOREP_F_Init )( SCOREP_Fortran_RegionHandle* handle,
                       char*                        name_f,
                       int32_t*                     type,
                       char*                        fileName_f,
                       int32_t*                     lineNo,
                       int                          nameLen,
                       int                          fileNameLen )
{
    char*                   name;
    char*                   fileName;
    SCOREP_SourceFileHandle fileHandle;

    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Copy strings */
    name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
    strncpy( name, name_f, nameLen );
    name[ nameLen ] = '\0';

    fileName = ( char* )malloc( ( fileNameLen + 1 ) * sizeof( char ) );
    strncpy( fileName, fileName_f, fileNameLen );
    fileName[ fileNameLen ] = '\0';
    SCOREP_IO_SimplifyPath( fileName );

    /* Get source file handle.
       The definitions check for double double entries. */
    fileHandle = SCOREP_DefineSourceFile( fileName );

    /* Lock region definition */
    SCOREP_MutexLock( scorep_user_region_mutex );

    /* Lookup the region name in the region table */
    SCOREP_User_RegionHandle region = scorep_user_find_region( name );

    /* Test wether the handle is still invalid, or if it was initialized in the mean
       time. If the handle is invalid, register a new region */
    if ( region == SCOREP_USER_INVALID_REGION )
    {
        /* Translate region type from user adapter type to SCOREP measurement type */
        SCOREP_RegionType region_type = scorep_user_to_scorep_region_type( *type );

        /* Check for filters */
        if ( SCOREP_Filter_Match( fileName, name, false ) )
        {
            region = SCOREP_FILTERED_USER_REGION;
            scorep_user_add_region( region, name );
        }
        else
        {
            region = scorep_user_create_region( name );

            if ( region != SCOREP_USER_INVALID_REGION )
            {
                region->handle = SCOREP_DefineRegion( name,
                                                      fileHandle,
                                                      *lineNo,
                                                      SCOREP_INVALID_LINE_NO,
                                                      SCOREP_ADAPTER_USER,
                                                      region_type );

                scorep_user_add_region( region, name );
            }
        }
    }
    *handle = SCOREP_C2F_REGION( region );

    /* Unlock region definition */
    SCOREP_MutexUnlock( scorep_user_region_mutex );

    /* Cleanup */
    free( name );
    free( fileName );
}

void
FSUB( SCOREP_F_Begin )( SCOREP_Fortran_RegionHandle* handle,
                        char*                        name_f,
                        int32_t*                     type,
                        char*                        fileName_f,
                        int32_t*                     lineNo,
                        int                          nameLen,
                        int                          fileNameLen )
{
    SCOREP_USER_ASSERT_NOT_FINALIZED;

    /* Make sure the handle is initialized */
    FSUB( SCOREP_F_Init )( handle, name_f, type, fileName_f,
                           lineNo, nameLen, fileNameLen );

    /* Generate region event */
    SCOREP_User_RegionEnter( SCOREP_F2C_REGION( *handle ) );
}

void
FSUB( SCOREP_F_RegionEnd )( SCOREP_Fortran_RegionHandle* handle )
{
    SCOREP_USER_ASSERT_NOT_FINALIZED;
    SCOREP_User_RegionEnd( SCOREP_F2C_REGION( *handle ) );
}

void
FSUB( SCOREP_F_RegionEnter )( SCOREP_Fortran_RegionHandle* handle )
{
    SCOREP_USER_ASSERT_NOT_FINALIZED;
    SCOREP_User_RegionEnter( SCOREP_F2C_REGION( *handle ) );
}
