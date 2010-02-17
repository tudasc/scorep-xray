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
#include "SILC_Definitions.h"
#include "SILC_DefinitionLocking.h"
#include "SILC_Events.h"
#include "SILC_Error.h"
#include "SILC_User_Init.h"
#include "SILC_Types.h"
#include "SILC_Utils.h"

/** @internal
    Hash table for mapping source file names to SILC file handles.
 */
SILC_Hashtab* silc_user_file_table;

void
silc_user_delete_file_entry( SILC_Hashtab_Entry* entry )
{
    SILC_ASSERT( entry );

    free( ( SILC_SourceFileHandle* )entry->value );
    free( ( char* )entry->key );
}

void
silc_user_init_regions()
{
    silc_user_file_table = SILC_Hashtab_CreateSize( 10, &SILC_Hashtab_HashString,
                                                    &SILC_Hashtab_CompareStrings );
}

void
silc_user_final_regions()
{
    SILC_Hashtab_Foreach( silc_user_file_table, &silc_user_delete_file_entry );
    SILC_Hashtab_Free( silc_user_file_table );
    silc_user_file_table = NULL;
}

SILC_SourceFileHandle
silc_user_get_file( const char*            file,
                    const char**           lastFileName,
                    SILC_SourceFileHandle* lastFile )
{
    size_t index;

    /* In most cases, it is expected that in most cases no regions are in included
       files. If the compiler inserts always the same string adress for file names,
       one static variable in a source file can remember the last used filename from
       a source file and sting comparisons can be avoided.

       However, if regions are defined in included header files, one must lookup
       file handles.
     */
    if ( *lastFileName == file )
    {
        return *lastFile;
    }

    /* Else store file name as last searched for and search in the hashtable */
    *lastFileName = file;
    SILC_Hashtab_Entry* entry = SILC_Hashtab_Find( silc_user_file_table, file, &index );

    /* If not found register new file */
    if ( !entry )
    {
        /* Reserve own storage for file name */
        char* fileName = ( char* )malloc( ( strlen( file ) + 1 ) * sizeof( char ) );
        strcpy( fileName, file );

        /* Register file to measurement system */
        SILC_SourceFileHandle* handle = malloc( sizeof( SILC_SourceFileHandle ) );
        *handle = SILC_DefineSourceFile( fileName );

        /* Store handle in hashtable */
        SILC_Hashtab_Insert( silc_user_file_table, ( void* )fileName, handle, &index );

        *lastFile = *handle;
    }

    else
    {
        /* Else store last used handle */
        *lastFile = *( SILC_SourceFileHandle* )entry->value;
    }
    return *lastFile;
}


/** @internal
    Translates the region type of the user adapter to the silc region types.
    The user adapter uses a bitvector for the type, silc has an enum. Where possible
    combinations are explicit.
    @param user_type The region type in the user adapter.
    @returns The region type in SILC measurement definitions. If the combination is
             invalid, an subset of the combinations is selected.
 */
SILC_RegionType
silc_user_to_silc_region_type( const SILC_User_RegionType user_type )
{
    switch ( user_type )
    {
        case 0:  // SILC_USER_REGION_TYPE_COMMON
            return SILC_REGION_USER;
        case 1:  // FUNCTION
            return SILC_REGION_FUNCTION;
        case 2:  // LOOP
            return SILC_REGION_LOOP;
        case 3:  // FUNCTION + LOOP -> Invalid -> use loop
            return SILC_REGION_LOOP;
        case 4:  // DYNAMIC
            return SILC_REGION_DYNAMIC;
        case 5:  // DYNAMIC + FUNCTION
            return SILC_REGION_DYNAMIC_FUNCTION;
        case 6:  // DYNAMIC + LOOP
            return SILC_REGION_DYNAMIC_LOOP;
        case 7:  // DYNAMIC + FUNCTION + LOOP -> Invalid -> use dynamic loop
            return SILC_REGION_DYNAMIC_LOOP;
        case 8:  // PHASE
            return SILC_REGION_PHASE;
        case 9:  // PHASE + FUNCTION -> use phase
            return SILC_REGION_PHASE;
        case 10: // PHASE + LOOP -> use phase
            return SILC_REGION_PHASE;
        case 11: // PHASE + FUNCTION + LOOP -> Invalid -> use phase
            return SILC_REGION_PHASE;
        case 12: // PHASE + DYNAMIC
            return SILC_REGION_DYNAMIC_PHASE;
        case 13: // PHASE + DYNAMIC + FUNCTION -> use dynamic phase
            return SILC_REGION_DYNAMIC_PHASE;
        case 14: // PHASE + DYNAMIC + LOOP
            return SILC_REGION_DYNAMIC_LOOP_PHASE;
        case 15: // PHASE + DYNAMIC + LOOP + FUNCTION -> Invalid -> use dynamic, phase, loop
            return SILC_REGION_DYNAMIC_LOOP_PHASE;
        default: // Not known
            return SILC_REGION_UNKNOWN;
    }
}

void
SILC_User_RegionBegin
(
    SILC_RegionHandle*         handle,
    const char**               lastFileName,
    SILC_SourceFileHandle*     lastFile,
    const char*                name,
    const SILC_User_RegionType regionType,
    const char*                fileName,
    const uint32_t             lineNo
)
{
    /* Make sure that the region is initialized */
    if ( *handle == SILC_INVALID_REGION )
    {
        SILC_User_RegionInit( handle, lastFileName, lastFile,
                              name, regionType, fileName, lineNo );
    }

    /* Generate region event */
    SILC_EnterRegion( *handle );
}


void
SILC_User_RegionEnd
(
    const SILC_RegionHandle handle
)
{
    /* Generate exit event */
    SILC_ExitRegion( handle );
}

void
SILC_User_RegionInit
(
    SILC_RegionHandle*         handle,
    const char**               lastFileName,
    SILC_SourceFileHandle*     lastFile,
    const char*                name,
    const SILC_User_RegionType regionType,
    const char*                fileName,
    const uint32_t             lineNo
)
{
    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Get source file handle */
    SILC_LockSourceFileDefinition();
    SILC_SourceFileHandle file = silc_user_get_file( fileName, lastFileName, lastFile );
    SILC_UnlockSourceFileDefinition();

    /* Lock region definition */
    SILC_LockRegionDefinition();

    /* Test wether the handle is still invalid, or if it was initialized in the mean time.
       If the handle is invalid, register a new region */
    if ( *handle == SILC_INVALID_REGION )
    {
        /* Translate region type from user adapter type to SILC measurement type */
        SILC_RegionType region_type = silc_user_to_silc_region_type( regionType );

        /* Register new region */
        *handle = SILC_DefineRegion( name,
                                     file,
                                     lineNo,
                                     SILC_INVALID_LINE_NO,
                                     SILC_ADAPTER_USER,
                                     region_type );
    }
    /* Release lock */
    SILC_UnlockRegionDefinition();
}

void
SILC_User_RegionEnter
(
    const SILC_RegionHandle handle
)
{
    /* Check for intialization */
    SILC_USER_ASSERT_INITIALIZED;

    /* Generate exit event */
    SILC_EnterRegion( handle );
}
