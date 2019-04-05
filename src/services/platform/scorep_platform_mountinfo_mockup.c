/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */
#include <config.h>

#include <SCOREP_Platform.h>
#include <SCOREP_ErrorCodes.h>

#define SCOREP_DEBUG_MODULE_NAME MOUNTINFO
#include <UTILS_Debug.h>

SCOREP_ErrorCode
SCOREP_Platform_MountInfoInitialize( void )
{
    UTILS_DEBUG( "Cannot provide mount information for this architecture" );
    return SCOREP_SUCCESS;
}

void
SCOREP_Platform_MountInfoFinalize( void )
{
}

SCOREP_MountInfo*
SCOREP_Platform_GetMountInfo( const char* filename )
{
    return NULL;
}

SCOREP_SystemTreeNodeHandle
SCOREP_Platform_GetTreeNodeHandle( SCOREP_MountInfo* mount_entry )
{
    return SCOREP_INVALID_SYSTEM_TREE_NODE;
}

void
SCOREP_Platform_AddMountInfoProperties( SCOREP_IoFileHandle io_file_handle, SCOREP_MountInfo* mnt_entry )
{
}
