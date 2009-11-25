/** @file SILC_User_Region.c
    @author Daniel Lorenz

    This file containes the implementation of user adapter functions concerning
    regions.
 */

#include "SILC_User_Functions.h"
#include "SILC_User_Init.h"
#include "SILC_Types.h"
#include "SILC_Events.h"
#include "SILC_Definitions.h"
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
FSUB( SILC_User_RegionBeginF ) ( SILC_Fortran_RegionHandle * handle,
                                 char*    name_f,
                                 int32_t * type,
                                 char*    fileName_f,
                                 int32_t * lineNo,
                                 int nameLen,
                                 int fileNameLen )
{
    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Test if the region is visited for the first time */
    if ( *handle == SILC_FORTRAN_INVALID_REGION )
    {
        char*                  name;
        char*                  fileName;
        SILC_Hashtab_Entry*    entry;
        size_t                 index;
        SILC_SourceFileHandle* fileHandle;

        /* Copy strings */
        name = ( char* )malloc( ( nameLen + 1 ) * sizeof( char ) );
        strncpy( name, name_f, nameLen );
        name[ nameLen ] = '\0';

        fileName = ( char* )malloc( ( fileNameLen + 1 ) * sizeof( char ) );
        strncpy( fileName, fileName_f, fileNameLen );
        name[ nameLen ] = '\0';

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

        /* Translate region type from user adapter type to SILC measurement type */
        SILC_RegionType region_type = silc_user_to_silc_region_type( *type );

        /* Register new region */
        *handle = SILC_DefineRegion( name,
                                     *fileHandle,
                                     *lineNo,
                                     SILC_INVALID_LINE_NO,
                                     SILC_ADAPTER_USER,
                                     region_type );

        /* Cleanup */
        free( name );
    }

    /* Generate region event */
    SILC_EnterRegion( *handle );
}

void
FSUB( SILC_User_RegionEndF ) ( SILC_Fortran_RegionHandle * handle )
{
    SILC_User_RegionEnd( *handle );
}
