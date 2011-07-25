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
 *  @file       SCOREP_User_Region.c
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 *  This file containes the implementation of user adapter functions concerning
 *  regions.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Events.h>
#include <SCOREP_User_Init.h>
#include <SCOREP_Types.h>
#include <SCOREP_Filter.h>
#include <scorep_utility/SCOREP_Utils.h>
#include "scorep_selective_region.h"

#define SCOREP_FILTERED_USER_REGION ( ( void* )-1 )

/**
    @internal
    Hash table for mapping source file names to SCOREP file handles.
 */
SCOREP_Hashtab* scorep_user_file_table = NULL;

/**
   Mutex for @ref scorep_user_file_table.
 */
SCOREP_Mutex scorep_user_file_table_mutex;

/**
   Mutex to avoid parallel assignement of region handles to the same region.
 */
SCOREP_Mutex scorep_user_region_mutex;

/**
    @internal
    Hash table for mapping regions names to the User adapte region structs.
    Needed for the fortran regions which can not be initialized in declaration. We can
    not determine by the handle value whether we initialized the region already. Thus, we need
    to lookup the name in a extra datastructure.
 */
SCOREP_Hashtab* scorep_user_region_table = NULL;

void
scorep_user_init_regions()
{
    SCOREP_MutexCreate( &scorep_user_region_mutex );
    SCOREP_MutexCreate( &scorep_user_file_table_mutex );
    scorep_user_file_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashString,
                                                        &SCOREP_Hashtab_CompareStrings );
    scorep_user_region_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashString,
                                                          &SCOREP_Hashtab_CompareStrings );
}

void
scorep_user_final_regions()
{
    /* the value entry is stored in a structure that is allocated with the scorep
       memory management system. Thus, it must not free the value. */
    SCOREP_Hashtab_FreeAll( scorep_user_region_table,
                            &SCOREP_Hashtab_DeleteFree,
                            &SCOREP_Hashtab_DeleteNone );

    SCOREP_Hashtab_FreeAll( scorep_user_file_table,
                            &SCOREP_Hashtab_DeleteNone,
                            &SCOREP_Hashtab_DeleteFree );

    scorep_user_region_table = NULL;
    scorep_user_file_table   = NULL;
    SCOREP_MutexDestroy( &scorep_user_file_table_mutex );
    SCOREP_MutexDestroy( &scorep_user_region_mutex );
}

static SCOREP_SourceFileHandle
scorep_user_get_file( const char*              file,
                      const char**             lastFileName,
                      SCOREP_SourceFileHandle* lastFile )
{
    size_t index;

    /* Hashtable access must be emutual exclusive */
    SCOREP_MutexLock( scorep_user_file_table_mutex );

    /* In most cases, it is expected that in most cases no regions are in included
       files. If the compiler inserts always the same string adress for file names,
       one static variable in a source file can remember the last used filename from
       a source file and sting comparisons can be avoided.

       However, if regions are defined in included header files, one must lookup
       file handles.
     */
    if ( *lastFileName == file )
    {
        SCOREP_MutexUnlock( scorep_user_file_table_mutex );
        return *lastFile;
    }

    /* Else store file name as last searched for and search in the hashtable */
    *lastFileName = file;
    SCOREP_Hashtab_Entry* entry = SCOREP_Hashtab_Find( scorep_user_file_table, file, &index );

    /* If not found register new file */
    if ( !entry )
    {
        /* Reserve own storage for file name */
        char* file_name = SCOREP_CStr_dup( file );
        SCOREP_IO_SimplifyPath( file_name );

        /* Register file to measurement system */
        SCOREP_SourceFileHandle* handle = malloc( sizeof( SCOREP_SourceFileHandle ) );
        *handle = SCOREP_DefineSourceFile( file_name );
        free( file_name );
        file_name = NULL;

        /* Store handle in hashtable */
        SCOREP_Hashtab_Insert( scorep_user_file_table, ( void* )file, handle, &index );

        *lastFile = *handle;
    }

    else
    {
        /* Else store last used handle */
        *lastFile = *( SCOREP_SourceFileHandle* )entry->value;
    }

    SCOREP_MutexUnlock( scorep_user_file_table_mutex );
    return *lastFile;
}


/** @internal
    Translates the region type of the user adapter to the scorep region types.
    The user adapter uses a bitvector for the type, scorep has an enum. Where possible
    combinations are explicit.
    @param user_type The region type in the user adapter.
    @returns The region type in SCOREP measurement definitions. If the combination is
             invalid, an subset of the combinations is selected.
 */
SCOREP_RegionType
scorep_user_to_scorep_region_type( const SCOREP_User_RegionType user_type )
{
    switch ( user_type )
    {
        case 0:  // SCOREP_USER_REGION_TYPE_COMMON
            return SCOREP_REGION_USER;
        case 1:  // FUNCTION
            return SCOREP_REGION_FUNCTION;
        case 2:  // LOOP
            return SCOREP_REGION_LOOP;
        case 3:  // FUNCTION + LOOP -> Invalid -> use loop
            return SCOREP_REGION_LOOP;
        case 4:  // DYNAMIC
            return SCOREP_REGION_DYNAMIC;
        case 5:  // DYNAMIC + FUNCTION
            return SCOREP_REGION_DYNAMIC_FUNCTION;
        case 6:  // DYNAMIC + LOOP
            return SCOREP_REGION_DYNAMIC_LOOP;
        case 7:  // DYNAMIC + FUNCTION + LOOP -> Invalid -> use dynamic loop
            return SCOREP_REGION_DYNAMIC_LOOP;
        case 8:  // PHASE
            return SCOREP_REGION_PHASE;
        case 9:  // PHASE + FUNCTION -> use phase
            return SCOREP_REGION_PHASE;
        case 10: // PHASE + LOOP -> use phase
            return SCOREP_REGION_PHASE;
        case 11: // PHASE + FUNCTION + LOOP -> Invalid -> use phase
            return SCOREP_REGION_PHASE;
        case 12: // PHASE + DYNAMIC
            return SCOREP_REGION_DYNAMIC_PHASE;
        case 13: // PHASE + DYNAMIC + FUNCTION -> use dynamic phase
            return SCOREP_REGION_DYNAMIC_PHASE;
        case 14: // PHASE + DYNAMIC + LOOP
            return SCOREP_REGION_DYNAMIC_LOOP_PHASE;
        case 15: // PHASE + DYNAMIC + LOOP + FUNCTION -> Invalid -> use dynamic, phase, loop
            return SCOREP_REGION_DYNAMIC_LOOP_PHASE;
        default: // Not known
            return SCOREP_REGION_UNKNOWN;
    }
}

void
SCOREP_User_RegionBegin
(
    SCOREP_User_RegionHandle*    handle,
    const char**                 lastFileName,
    SCOREP_SourceFileHandle*     lastFile,
    const char*                  name,
    const SCOREP_User_RegionType regionType,
    const char*                  fileName,
    const uint32_t               lineNo
)
{
    /* Make sure that the region is initialized */
    if ( *handle == SCOREP_INVALID_USER_REGION )
    {
        SCOREP_User_RegionInit( handle, lastFileName, lastFile,
                                name, regionType, fileName, lineNo );
    }

    /* Generate region event */
    SCOREP_User_RegionEnter( *handle );
}


void
SCOREP_User_RegionEnd
(
    const SCOREP_User_RegionHandle handle
)
{
    /* Generate exit event */
    if ( ( handle != SCOREP_INVALID_USER_REGION ) &&
         ( handle != SCOREP_FILTERED_USER_REGION ) )
    {
        SCOREP_ExitRegion( handle->handle );
        scorep_selective_check_exit( handle );
    }
}

void
SCOREP_User_RegionInit
(
    SCOREP_User_RegionHandle*    handle,
    const char**                 lastFileName,
    SCOREP_SourceFileHandle*     lastFile,
    const char*                  name,
    const SCOREP_User_RegionType regionType,
    const char*                  fileName,
    const uint32_t               lineNo
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Get source file handle */
    SCOREP_SourceFileHandle file = scorep_user_get_file( fileName,
                                                         lastFileName,
                                                         lastFile );

    /* Lock region definition */
    SCOREP_MutexLock( scorep_user_region_mutex );

    /* Test wether the handle is still invalid, or if it was initialized in the mean time.
       If the handle is invalid, register a new region */
    if ( *handle == SCOREP_INVALID_USER_REGION )
    {
        /* Translate region type from user adapter type to SCOREP measurement type */
        SCOREP_RegionType region_type = scorep_user_to_scorep_region_type( regionType );

        /* We store the new handle in a local variable and assign it to *handle
           only after it is completly initialized. Else other threads may test in
           between whether the handle is intialized, and use it while initialization
           is not completed.
           We do not want to secure the initial test with locks.
         */
        SCOREP_User_RegionHandle new_handle = SCOREP_FILTERED_USER_REGION;

        /* Check for filters */
        char* simplified_name = SCOREP_SourceFile_GetName( file );
        if ( !SCOREP_Filter_Match( simplified_name, name, false ) )
        {
            printf( "Simplified name: %s\n", simplified_name );
            new_handle = scorep_user_create_region( name );
        }

        /* Register new region */
        if ( ( new_handle != SCOREP_INVALID_USER_REGION ) &&
             ( new_handle != SCOREP_FILTERED_USER_REGION ) )
        {
            new_handle->handle = SCOREP_DefineRegion( name,
                                                      file,
                                                      lineNo,
                                                      SCOREP_INVALID_LINE_NO,
                                                      SCOREP_ADAPTER_USER,
                                                      region_type );
        }
        *handle = new_handle;
    }
    /* Release lock */
    SCOREP_MutexUnlock( scorep_user_region_mutex );
}

void
SCOREP_User_RegionEnter
(
    const SCOREP_User_RegionHandle handle
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Generate enter event */
    if ( ( handle != SCOREP_INVALID_USER_REGION ) &&
         ( handle != SCOREP_FILTERED_USER_REGION ) )
    {
        scorep_selective_check_enter( handle );
        SCOREP_EnterRegion( handle->handle );
    }
}
