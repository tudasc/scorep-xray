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

#include <mntent.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <SCOREP_Platform.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Vector.h>
#include <UTILS_CStr.h>
#include <SCOREP_ErrorCodes.h>

#define MOUNT_SRC  "/proc/self/mounts"

static bool mountinfo_initialized = false;

struct SCOREP_MountInfo
{
    char*                    mount_point;
    char*                    mount_src;
    char*                    fstype;
    struct SCOREP_MountInfo* next;
};

static SCOREP_MountInfo* mount_stack_top = NULL;

static void
mount_stack_push( SCOREP_MountInfo* entry )
{
    if ( mount_stack_top != NULL )
    {
        entry->next = mount_stack_top;
    }
    mount_stack_top = entry;
}

static SCOREP_MountInfo*
mount_stack_pop( void )
{
    if ( mount_stack_top == NULL )
    {
        return NULL;
    }

    SCOREP_MountInfo* entry = mount_stack_top;

    mount_stack_top = mount_stack_top->next;

    return entry;
}


static SCOREP_ErrorCode
read_mounts( void )
{
    FILE*          fp;
    struct mntent* fs;

    fp = setmntent( MOUNT_SRC, "r" );

    if ( fp == NULL )
    {
        return SCOREP_ERROR_INVALID;
    }

    while ( ( fs = getmntent( fp ) ) != NULL )
    {
        size_t mount_point_len = strlen( fs->mnt_dir ) + 1;
        size_t mount_src_len   = strlen( fs->mnt_fsname ) + 1;
        size_t fstype_len      = strlen( fs->mnt_type ) + 1;

        SCOREP_MountInfo* mnt = malloc( sizeof( *mnt ) + sizeof( char ) * ( mount_point_len + fstype_len + mount_src_len ) );
        UTILS_ASSERT( mnt != NULL );

        mnt->mount_point = ( char* )( mnt + 1 );
        mnt->mount_src   = mnt->mount_point + mount_point_len;
        mnt->fstype      = mnt->mount_src + mount_src_len;
        mnt->next        = NULL;

        memcpy( mnt->mount_point, fs->mnt_dir, mount_point_len );
        memcpy( mnt->mount_src, fs->mnt_fsname, mount_src_len );
        memcpy( mnt->fstype, fs->mnt_type, fstype_len );

        mount_stack_push( mnt );
    }

    endmntent( fp );

    return SCOREP_SUCCESS;
}

static void
free_mount_table_entry( SCOREP_MountInfo* entry )
{
    if ( entry != NULL )
    {
        free( entry );
    }
}

static bool
is_distributed_fs( SCOREP_MountInfo* entry )
{
    if ( strstr( entry->fstype, "nfs" )       != NULL
         || strstr( entry->fstype, "pvfs" )   != NULL
         || strstr( entry->fstype, "lustre" ) != NULL
         || strstr( entry->fstype, "gpfs" )   != NULL
         || strstr( entry->fstype, "cifs" )   != NULL
         || strstr( entry->fstype, "sshfs" )  != NULL )
    {
        return true;
    }

    return false;
}

static SCOREP_SystemTreeNodeHandle
mountinfo_get_tree_node_handle( SCOREP_MountInfo* entry )
{
    bool is_shared_fs = is_distributed_fs( entry );

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         SystemTreeNode,
                                                         system_tree_node )

    if ( is_shared_fs
         && definition->domains & SCOREP_SYSTEM_TREE_DOMAIN_MACHINE )
    {
        return handle;
    }
    if ( !is_shared_fs
         && definition->domains & SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY )
    {
        return handle;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();


    return SCOREP_INVALID_SYSTEM_TREE_NODE;
}

static SCOREP_MountInfo*
mountinfo_find_mount( const char* path )
{
    const int         path_len    = strlen( path );
    int               bestmatch   = 0;
    SCOREP_MountInfo* found_entry = NULL;
    SCOREP_MountInfo* entry       = mount_stack_top;

    while ( entry != NULL )
    {
        int tmp_len = strlen( entry->mount_point );

        if ( ( bestmatch <= tmp_len && tmp_len <= path_len )
             && ( strncmp( entry->mount_point, path, tmp_len ) == 0 ) )
        {
            bestmatch   = tmp_len;
            found_entry = entry;
        }
        entry = entry->next;
    }

    return found_entry;
}

void
SCOREP_Platform_MountInfoFinalize( void )
{
    if ( mountinfo_initialized )
    {
        while ( mount_stack_top != NULL )
        {
            SCOREP_MountInfo* entry = mount_stack_pop();
            free_mount_table_entry( entry );
        }
        mountinfo_initialized = false;
    }
}

SCOREP_ErrorCode
SCOREP_Platform_MountInfoInitialize( void )
{
    if ( mountinfo_initialized )
    {
        return SCOREP_ERROR_INVALID;
    }

    SCOREP_ErrorCode ret = read_mounts();

    if ( ret != SCOREP_SUCCESS )
    {
        SCOREP_Platform_MountInfoFinalize();
        return ret;
    }

    mountinfo_initialized = true;
    return ret;
}

SCOREP_MountInfo*
SCOREP_Platform_GetMountInfo( const char* filename )
{
    SCOREP_MountInfo* found_entry = NULL;

    if ( filename != NULL )
    {
        found_entry = mountinfo_find_mount( filename );
    }

    return found_entry;
}

SCOREP_SystemTreeNodeHandle
SCOREP_Platform_GetTreeNodeHandle( SCOREP_MountInfo* mount_entry )
{
    if ( mount_entry != NULL )
    {
        return mountinfo_get_tree_node_handle( mount_entry );
    }

    return SCOREP_INVALID_SYSTEM_TREE_NODE;
}

void
SCOREP_Platform_AddMountInfoProperties( SCOREP_IoFileHandle io_file_handle, SCOREP_MountInfo* mnt_entry )
{
    if ( mnt_entry != NULL )
    {
        SCOREP_IoFileHandle_AddProperty( io_file_handle, "Mount Point", mnt_entry->mount_point );
        SCOREP_IoFileHandle_AddProperty( io_file_handle, "Mount Source", mnt_entry->mount_src );
        SCOREP_IoFileHandle_AddProperty( io_file_handle, "File system", mnt_entry->fstype );
    }
}
