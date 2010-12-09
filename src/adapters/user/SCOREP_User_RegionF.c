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
#include <SCOREP_User_Functions.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Types.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Fortran_Wrapper.h>

#include <string.h>

#define SCOREP_F_Begin_U SCOREP_F_BEGIN
#define SCOREP_F_Init_U SCOREP_F_INIT
#define SCOREP_F_RegionEnd_U SCOREP_F_REGIONEND
#define SCOREP_F_RegionEnter_U SCOREP_F_REGIONENTER

#define SCOREP_F_Begin_L scorep_f_begin
#define SCOREP_F_Init_L scorep_f_init
#define SCOREP_F_RegionEnd_L scorep_f_regionend
#define SCOREP_F_RegionEnter_L scorep_f_regionenter

extern SCOREP_Hashtab* scorep_user_file_table;
extern SCOREP_Mutex    scorep_user_file_table_mutex;
extern SCOREP_Mutex    scorep_user_region_mutex;

extern SCOREP_RegionType
scorep_user_to_scorep_region_type( const SCOREP_User_RegionType user_type );

void
FSUB( SCOREP_User_RegionInitF )( SCOREP_Fortran_RegionHandle* handle,
                                 char*                        name_f,
                                 int32_t*                     type,
                                 char*                        fileName_f,
                                 int32_t*                     lineNo,
                                 int                          nameLen,
                                 int                          fileNameLen )
{
    char*                    name;
    char*                    fileName;
    SCOREP_Hashtab_Entry*    entry;
    size_t                   index;
    SCOREP_SourceFileHandle* fileHandle;

    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Copy strings */
    name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
    strncpy( name, name_f, nameLen );
    name[ nameLen ] = '\0';

    fileName = ( char* )malloc( ( fileNameLen + 1 ) * sizeof( char ) );
    strncpy( fileName, fileName_f, fileNameLen );
    name[ nameLen ] = '\0';

    /* Lock file definition */
    SCOREP_MutexLock( scorep_user_file_table_mutex );

    /* Search for source file handle */
    entry = SCOREP_Hashtab_Find( scorep_user_file_table, fileName, &index );

    /*  If not found register new file */
    if ( !entry )
    {
        /* Register file to measurement system */
        fileHandle  = malloc( sizeof( SCOREP_SourceFileHandle ) );
        *fileHandle = SCOREP_DefineSourceFile( fileName );

        /* Store handle in hashtable */
        SCOREP_Hashtab_Insert( scorep_user_file_table,
                               ( void* )fileName,
                               fileHandle,
                               &index );
    }
    else
    {
        /* If found the reserved space for the file name should be freed */
        free( fileName );

        fileHandle = ( SCOREP_SourceFileHandle* )entry->value;
    }
    /* Unlock file defintion */
    SCOREP_MutexUnlock( scorep_user_file_table_mutex );

    /* Lock region definition */
    SCOREP_MutexLock( scorep_user_region_mutex );

    /* Test wether the handle is still invalid, or if it was initialized in the mean
       time. If the handle is invalid, register a new region */
    if ( *handle == SCOREP_FORTRAN_INVALID_REGION )
    {
        /* Translate region type from user adapter type to SCOREP measurement type */
        SCOREP_RegionType region_type = scorep_user_to_scorep_region_type( *type );

        /* Register new region */
        *handle = SCOREP_C2F_REGION( SCOREP_DefineRegion( name,
                                                          *fileHandle,
                                                          *lineNo,
                                                          SCOREP_INVALID_LINE_NO,
                                                          SCOREP_ADAPTER_USER,
                                                          region_type ) );

        /* Cleanup */
        free( name );
    }
    /* Unlock region definition */
    SCOREP_MutexUnlock( scorep_user_region_mutex );
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
    /* Make sure the region handle is already defined */
    if ( *handle == SCOREP_FORTRAN_INVALID_REGION )
    {
        /* Make sure the handle is initialized */
        FSUB( SCOREP_User_RegionInitF )( handle, name_f, type, fileName_f,
                                         lineNo, nameLen, fileNameLen );
    }

    /* Generate region event */
    SCOREP_EnterRegion( SCOREP_F2C_REGION( *handle ) );
}

void
FSUB( SCOREP_F_RegionEnd )( SCOREP_Fortran_RegionHandle* handle )
{
    SCOREP_User_RegionEnd( SCOREP_F2C_REGION( *handle ) );
}

void
FSUB( SCOREP_F_RegionEnter )( SCOREP_Fortran_RegionHandle* handle )
{
    SCOREP_User_RegionEnter( SCOREP_F2C_REGION( *handle ) );
}
