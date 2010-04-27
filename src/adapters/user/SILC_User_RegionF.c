/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/** @file SILC_User_Region.c
    @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
    @status     ALPHA

    This file containes the implementation of user adapter functions concerning
    regions.
 */

#include "SILC_User_Functions.h"
#include "SILC_User_Init.h"
#include "SILC_Types.h"
#include "SILC_Events.h"
#include "SILC_Definitions.h"
#include "SILC_DefinitionLocking.h"
#include "SILC_Utils.h"
#include "SILC_Error.h"
#include "SILC_Fortran_Wrapper.h"

#include <string.h>

#define SILC_User_RegionBeginF_U SILC_USER_REGIONBEGINF
#define SILC_User_RegionEndF_U SILC_USER_REGIONENDF

#define SILC_User_RegionBeginF_L silc_user_regionbeginf
#define SILC_User_RegionEndF_L silc_user_regionendf

extern SILC_Hashtab* silc_user_file_table;

extern SILC_RegionType
silc_user_to_silc_region_type( const SILC_User_RegionType user_type );

void
FSUB( SILC_User_RegionInitF )( SILC_Fortran_RegionHandle* handle,
                               char*                      name_f,
                               int32_t*                   type,
                               char*                      fileName_f,
                               int32_t*                   lineNo,
                               int                        nameLen,
                               int                        fileNameLen )
{
    char*                  name;
    char*                  fileName;
    SILC_Hashtab_Entry*    entry;
    size_t                 index;
    SILC_SourceFileHandle* fileHandle;

    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Copy strings */
    name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
    strncpy( name, name_f, nameLen );
    name[ nameLen ] = '\0';

    fileName = ( char* )malloc( ( fileNameLen + 1 ) * sizeof( char ) );
    strncpy( fileName, fileName_f, fileNameLen );
    name[ nameLen ] = '\0';

    /* Lock file definition */
    SILC_LockSourceFileDefinition();

    /* Search for source file handle */
    entry = SILC_Hashtab_Find( silc_user_file_table, fileName, &index );

    /*  If not found register new file */
    if ( !entry )
    {
        /* Register file to measurement system */
        fileHandle  = malloc( sizeof( SILC_SourceFileHandle ) );
        *fileHandle = SILC_DefineSourceFile( fileName );

        /* Store handle in hashtable */
        SILC_Hashtab_Insert( silc_user_file_table,
                             ( void* )fileName,
                             fileHandle,
                             &index );
    }
    else
    {
        /* If found the reserved space for the file name should be freed */
        free( fileName );

        fileHandle = ( SILC_SourceFileHandle* )entry->value;
    }
    /* Unlock file defintion */
    SILC_UnlockSourceFileDefinition();

    /* Lock region definition */
    SILC_LockRegionDefinition();


    /* Test wether the handle is still invalid, or if it was initialized in the mean
       time. If the handle is invalid, register a new region */
    if ( *handle == SILC_FORTRAN_INVALID_REGION )
    {
        /* Translate region type from user adapter type to SILC measurement type */
        SILC_RegionType region_type = silc_user_to_silc_region_type( *type );

        /* Register new region */
        *handle = SILC_C2F_REGION( SILC_DefineRegion( name,
                                                      *fileHandle,
                                                      *lineNo,
                                                      SILC_INVALID_LINE_NO,
                                                      SILC_ADAPTER_USER,
                                                      region_type ) );

        /* Cleanup */
        free( name );
    }
    /* Unlock region definition */
    SILC_UnlockRegionDefinition();
}

void
FSUB( SILC_User_RegionBeginF )( SILC_Fortran_RegionHandle* handle,
                                char*                      name_f,
                                int32_t*                   type,
                                char*                      fileName_f,
                                int32_t*                   lineNo,
                                int                        nameLen,
                                int                        fileNameLen )
{
    /* Make sure the region handle is already defined */
    if ( *handle == SILC_FORTRAN_INVALID_REGION )
    {
        /* Make sure the handle is initialized */
        FSUB( SILC_User_RegionInitF )( handle, name_f, type, fileName_f,
                                       lineNo, nameLen, fileNameLen );
    }

    /* Generate region event */
    SILC_EnterRegion( SILC_F2C_REGION( *handle ) );
}

void
FSUB( SILC_User_RegionEndF )( SILC_Fortran_RegionHandle* handle )
{
    SILC_User_RegionEnd( SILC_F2C_REGION( *handle ) );
}

void
FSUB( SILC_User_RegionEnterF )( SILC_Fortran_RegionHandle* handle )
{
    SILC_User_RegionEnter( SILC_F2C_REGION( *handle ) );
}
