/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 *  @file       src/adapters/user/SCOREP_User_Region.c
 *
 *  This file contains the implementation of user adapter functions concerning
 *  regions.
 */

#include <config.h>
#include <scorep/SCOREP_User_Functions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Events.h>
#include "SCOREP_User_Init.h"
#include <SCOREP_Types.h>
#include <SCOREP_Filter.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>
#include <SCOREP_OA_Functions.h>
#include "scorep_selective_region.h"

#include <stdlib.h>

#define SCOREP_FILTERED_USER_REGION ( ( void* )-1 )



static SCOREP_SourceFileHandle
scorep_user_get_file( const char*              file,
                      const char**             lastFileName,
                      SCOREP_SourceFileHandle* lastFile )
{
    /* Hashtable access must be mutual exclusive */
    SCOREP_MutexLock( scorep_user_file_table_mutex );

    /* In most cases, it is expected that no regions are in included
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

    /* Store file name as last searched for and return new file handle.
       The definitions hash entries and donot allow double entries.
       In the definitions we want to have simplified file names. */
    char* file_name = UTILS_CStr_dup( file );
    UTILS_IO_SimplifyPath( file_name );
    SCOREP_SourceFileHandle handle = SCOREP_Definitions_NewSourceFile( file_name );
    free( file_name );

    /* Cache last used file information */
    *lastFile     = handle;
    *lastFileName = file;

    SCOREP_MutexUnlock( scorep_user_file_table_mutex );
    return handle;
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
SCOREP_User_RegionBegin( SCOREP_User_RegionHandle*    handle,
                         const char**                 lastFileName,
                         SCOREP_SourceFileHandle*     lastFile,
                         const char*                  name,
                         const SCOREP_User_RegionType regionType,
                         const char*                  fileName,
                         const uint32_t               lineNo )
{
    SCOREP_USER_ASSERT_NOT_FINALIZED;

    /* Make sure that the region is initialized */
    if ( *handle == SCOREP_USER_INVALID_REGION )
    {
        SCOREP_User_RegionInit( handle, lastFileName, lastFile,
                                name, regionType, fileName, lineNo );
    }

    /* Generate region event */
    SCOREP_User_RegionEnter( *handle );
}


void
SCOREP_User_RegionEnd( const SCOREP_User_RegionHandle handle )
{
    SCOREP_USER_ASSERT_NOT_FINALIZED

    /* Generate exit event */
    if ( ( handle != SCOREP_USER_INVALID_REGION ) &&
         ( handle != SCOREP_FILTERED_USER_REGION ) )
    {
        SCOREP_ExitRegion( handle->handle );
        scorep_selective_check_exit( handle );
    }
}

void
SCOREP_User_RegionInit( SCOREP_User_RegionHandle*    handle,
                        const char**                 lastFileName,
                        SCOREP_SourceFileHandle*     lastFile,
                        const char*                  name,
                        const SCOREP_User_RegionType regionType,
                        const char*                  fileName,
                        const uint32_t               lineNo )
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
    if ( *handle == SCOREP_USER_INVALID_REGION )
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
        const char* simplified_name = SCOREP_SourceFileHandle_GetName( file );
        if ( !SCOREP_Filter_Match( simplified_name, name, NULL ) )
        {
            new_handle = scorep_user_create_region( name );
        }

        /* Register new region */
        if ( ( new_handle != SCOREP_USER_INVALID_REGION ) &&
             ( new_handle != SCOREP_FILTERED_USER_REGION ) )
        {
            new_handle->handle = SCOREP_Definitions_NewRegion( name,
                                                               NULL,
                                                               file,
                                                               lineNo,
                                                               SCOREP_INVALID_LINE_NO,
                                                               SCOREP_PARADIGM_USER,
                                                               region_type );
        }
        *handle = new_handle;
    }
    /* Release lock */
    SCOREP_MutexUnlock( scorep_user_region_mutex );
}

void
SCOREP_User_RegionEnter( const SCOREP_User_RegionHandle handle )
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Generate enter event */
    if ( ( handle != SCOREP_USER_INVALID_REGION ) &&
         ( handle != SCOREP_FILTERED_USER_REGION ) )
    {
        scorep_selective_check_enter( handle );
        SCOREP_EnterRegion( handle->handle );
    }
}

void
SCOREP_User_RewindRegionBegin
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
    SCOREP_USER_ASSERT_NOT_FINALIZED;

    /* Make sure that the rewind region is initialized */
    if ( *handle == SCOREP_USER_INVALID_REGION )
    {
        SCOREP_User_RegionInit( handle, lastFileName, lastFile,
                                name, regionType, fileName, lineNo );
    }

    /* Generate rewind point */
    SCOREP_User_RewindRegionEnter( *handle );
}

void
SCOREP_User_RewindRegionEnd
(
    const SCOREP_User_RegionHandle handle,
    bool                           value
)
{
    SCOREP_USER_ASSERT_NOT_FINALIZED

    /* Make rewind and generate exit event for this region */
    if ( ( handle != SCOREP_USER_INVALID_REGION ) &&
         ( handle != SCOREP_FILTERED_USER_REGION ) )
    {
        SCOREP_ExitRewindRegion( handle->handle, value );
        scorep_selective_check_exit( handle );
    }
}

void
SCOREP_User_RewindRegionEnter
(
    const SCOREP_User_RegionHandle handle
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    /* Generate rewind point and enter event for this region */
    scorep_selective_check_enter( handle );
    SCOREP_EnterRewindRegion( handle->handle );
}

void
SCOREP_User_OaPhaseBegin
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
    SCOREP_USER_ASSERT_NOT_FINALIZED;

    /* Make sure that the region is initialized */
    if ( *handle == SCOREP_USER_INVALID_REGION )
    {
        SCOREP_User_RegionInit( handle, lastFileName, lastFile, name,
                                regionType, fileName, lineNo );
    }

    SCOREP_OA_PhaseBegin( ( *handle )->handle );

    SCOREP_User_RegionEnter( *handle );
}

void
SCOREP_User_OaPhaseEnd
(
    const SCOREP_User_RegionHandle handle
)
{
    /* Check for intialization */
    SCOREP_USER_ASSERT_INITIALIZED;

    SCOREP_User_RegionEnd( handle );

    SCOREP_OA_PhaseEnd( handle->handle );
}
