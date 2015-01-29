/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Implementation to obtain the system tree information from a Cray
 * system.
 */


#include <config.h>


#include "scorep_platform_system_tree.h"


#include <UTILS_Error.h>


#include <pmi.h>


#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#include <UTILS_IO.h>


SCOREP_ErrorCode
scorep_platform_get_path_in_system_tree( SCOREP_Platform_SystemTreePathElement* root )
{
    int              prank, x, y, z, nid;
    int              cabinet, cabinet_row, cage, slot, node_id, asic;
    pmi_mesh_coord_t max_xyz, coord;
    pmi_nic_addr_t   nic;

    SCOREP_Platform_SystemTreeProperty* property = NULL;
    int                                 fd       = -1;
    char                                buffer[ 256 ];
    size_t                              buffer_size = sizeof( buffer ) - 1;
    ssize_t                             bytes       = 0;

    /* Get hostname */
    SCOREP_Platform_SystemTreePathElement* node = NULL;
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY,
                                                      "node",
                                                      256, "" );
    if ( !node )
    {
        return UTILS_ERROR( SCOREP_ERROR_MEM_FAULT, "Failed to add hostname node" );
    }

    if ( UTILS_IO_GetHostname( node->node_name, 256 ) != 0 )
    {
        int errno_safed = errno;
        SCOREP_Platform_FreePath( node );
        errno = errno_safed;
        return UTILS_ERROR_POSIX( "UTILS_IO_GetHostname() failed." );
    }

    /* Get information from PMI interface */
    /* maximum dimension of mesh */
    /* node id */
    /* coordinates within the mesh */
    /* nic id */

    PMI_BOOL initialized;
    PMI_Initialized( &initialized );

    int spawned;
    if ( PMI_FALSE == initialized )
    {
        PMI_Init( &spawned );
    }

    PMI_Get_max_dimension( &max_xyz );

    x = ( int )max_xyz.mesh_x;
    y = ( int )max_xyz.mesh_y;
    z = ( int )max_xyz.mesh_z;

    PMI_Get_rank( &prank );
    PMI_Get_nid( prank, &nid );
    PMI_Get_meshcoord( nid, &coord );
    PMI_nid_to_nicaddrs( nid, 1, &nic );

    /*
     * PMI is also initialized by MPI_Init and finalized by
     * MPI_Finalize. We noticed that if Score-P initializes
     * and finalizes PMI here, MPI programs might end up in
     * a segmentation fault in MPI_Finalize. It seems that
     * there is a bug if PMI is initialized/finalized twice.
     * Cray support says that user code should not finalize
     * PMI. That's why the call to PMI_Finalize is skipped to
     * work around this issue.
     *
       if ( PMI_FALSE == initialized )
       {
        PMI_Finalize();
       }
     */

    /* NIC */
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                      "nic",
                                                      16, "%d", nic );
    if ( !node )
    {
        goto fail;
    }

    /* Get id of ASIC (SeaStar, Gemini, Aries) */
    asic = ( ( int )coord.mesh_x ) * ( y + 1 ) * ( z + 1 ) + ( ( int )coord.mesh_y ) * ( z + 1 ) + ( int )coord.mesh_z;


    /* ASIC */
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                      "asic",
                                                      16, "%d", asic );
    if ( !node )
    {
        goto fail;
    }


    /* Add coordinates (x,y,z) to the ASIC */
    property =
        scorep_platform_system_tree_add_property( node,
                                                  "x_coord",
                                                  4, "%3d", coord.mesh_x );
    if ( !property )
    {
        goto fail;
    }

    property =
        scorep_platform_system_tree_add_property( node,
                                                  "y_coord",
                                                  4, "%3d", coord.mesh_y );
    if ( !property )
    {
        goto fail;
    }

    property =
        scorep_platform_system_tree_add_property( node,
                                                  "z_coord",
                                                  4, "%3d", coord.mesh_z );
    if ( !property )
    {
        goto fail;
    }

    /* get cname */
    fd = open( "/proc/cray_xt/cname", O_RDONLY );
    if ( fd < 0 )
    {
        UTILS_WARNING( "Cannot open file /proc/cray_xt/cname: %s",
                       strerror( errno ) );
        goto fail;
    }

    bytes = read( fd, buffer, buffer_size );
    if ( bytes <= 0 )
    {
        UTILS_WARNING( "Cannot read file /proc/cray_xt/cname: %s",
                       strerror( errno ) );
        close( fd );
        goto fail;
    }
    close( fd );

    /* Null termination and replace possible new lines at the end */
    buffer[ bytes ] = '\0';
    bytes--;
    for (; bytes >= 0; bytes-- )
    {
        if ( buffer[ bytes ] == '\n' )
        {
            buffer[ bytes ] = '\0';
        }
    }

    /* parse cname in the following order: cabinet, cabinet row, cage, slot, node */
    errno = 0;
    int items_matched;
    items_matched = sscanf( buffer, "c%d-%dc%ds%dn%d", &cabinet, &cabinet_row, &cage, &slot, &node_id );
    if ( items_matched != 5 || errno != 0 )
    {
        UTILS_WARNING( "Error while parsing cname." );
        goto fail;
    }

    /* blade/slot */
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                      "blade",
                                                      16, "%d", slot );
    if ( !node )
    {
        goto fail;
    }

    /* cage */
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                      "cage",
                                                      16, "%d", cage );
    if ( !node )
    {
        goto fail;
    }

    /* cabinet */
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                      "cabinet",
                                                      16, "%d", cabinet );
    if ( !node )
    {
        goto fail;
    }

    /* cabinet row */
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                      "cabinet row",
                                                      16, "%d", cabinet_row );
    if ( !node )
    {
        goto fail;
    }

    /* Add system specific system tree path to generic one */
    root->next = node;

    return SCOREP_SUCCESS;

fail:
    SCOREP_Platform_FreePath( node );

    return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                        "Failed to build system tree path" );
}
