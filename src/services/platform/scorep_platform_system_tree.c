/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 * Platform independent code for building up the path in the system tree.
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


#include <UTILS_Error.h>


#include <SCOREP_Platform.h>


#include "scorep_platform_system_tree.h"


SCOREP_ErrorCode
SCOREP_Platform_GetPathInSystemTree( SCOREP_Platform_SystemTreePathElement** root,
                                     const char*                             machineName,
                                     const char*                             platformName )
{
    UTILS_ASSERT( root );
    *root = NULL;
    /* Create toplevel machine node machine */
    scorep_platform_system_tree_bottom_up_add( root,
                                               SCOREP_SYSTEM_TREE_DOMAIN_MACHINE,
                                               "machine",
                                               0, machineName );
    if ( !*root )
    {
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Failed to build system tree root" );
    }

    SCOREP_Platform_SystemTreeProperty* property =
        scorep_platform_system_tree_add_property( *root,
                                                  "platform",
                                                  0, platformName );
    if ( !property )
    {
        SCOREP_Platform_FreePath( *root );
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "Failed to build system tree path" );
    }

    /* Obtain system tree information from platform dependent implementation */
    SCOREP_ErrorCode err = scorep_platform_get_path_in_system_tree( *root );
    if ( err != SCOREP_SUCCESS )
    {
        /* scorep_platform_get_path_in_system_tree already dropped the path */
        return UTILS_ERROR( err, "Failed to obtain system tree information." );
    }

    return SCOREP_SUCCESS;
}

void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreePathElement* path )
{
    while ( path )
    {
        SCOREP_Platform_SystemTreePathElement* next = path->next;

        while ( path->properties )
        {
            SCOREP_Platform_SystemTreeProperty* next_property =
                path->properties->next;
            free( path->properties );
            path->properties = next_property;
        }

        free( path );

        path = next;
    }
}



/**
 * For parameter documentation see
 * scorep_platform_system_tree_bottom_up_add() and
 * scorep_platform_system_tree_top_down_add().
 */
static SCOREP_Platform_SystemTreePathElement*
create_element( SCOREP_SystemTreeDomain domains,
                const char*             node_class,
                size_t                  node_name_len,
                const char*             node_name_fmt,
                va_list                 vl )
{
    if ( !node_class || !node_name_fmt )
    {
        return NULL;
    }

    bool use_vl = true;
    if ( node_name_len == 0 )
    {
        node_name_len = strlen( node_name_fmt ) + 1;
        use_vl        = false;
    }

    SCOREP_Platform_SystemTreePathElement* new_element;
    size_t                                 node_class_offset = sizeof( *new_element );
    size_t                                 node_class_len    = strlen( node_class ) + 1;
    size_t                                 node_name_offset  = node_class_offset + node_class_len;
    size_t                                 total_size        = node_name_offset + node_name_len;

    new_element = malloc( total_size );
    if ( !new_element )
    {
        return NULL;
    }

    new_element->next       = NULL;
    new_element->domains    = domains;
    new_element->node_class = ( char* )new_element + node_class_offset;
    new_element->node_name  = ( char* )new_element + node_name_offset;

    /* set class name */
    memcpy( new_element->node_class, node_class, node_class_len );

    /* set node name */
    if ( use_vl )
    {
        /* The caller provided a format string suitable for snprintf */
        vsnprintf( new_element->node_name, node_name_len, node_name_fmt, vl );
    }
    else
    {
        /* The caller provided a fixed string */
        memcpy( new_element->node_name, node_name_fmt, node_name_len );
    }

    new_element->properties      = NULL;
    new_element->properties_tail = &new_element->properties;

    return new_element;
}


SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_top_down_add(
    SCOREP_Platform_SystemTreePathElement*** tail,
    SCOREP_SystemTreeDomain                  domains,
    const char*                              node_class,
    size_t                                   node_name_len,
    const char*                              node_name_fmt,
    ... )
{
    if ( !tail )
    {
        return NULL;
    }

    va_list vl;
    va_start( vl, node_name_fmt );

    SCOREP_Platform_SystemTreePathElement* new_element =
        create_element( domains,
                        node_class,
                        node_name_len,
                        node_name_fmt,
                        vl );

    va_end( vl );

    if ( !new_element )
    {
        return NULL;
    }

    /* append to path */
    **tail = new_element;
    *tail  = &new_element->next;

    return new_element;
}

SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_bottom_up_add(
    SCOREP_Platform_SystemTreePathElement** head,
    SCOREP_SystemTreeDomain                 domains,
    const char*                             node_class,
    size_t                                  node_name_len,
    const char*                             node_name_fmt,
    ... )
{
    if ( !head )
    {
        return NULL;
    }

    va_list vl;
    va_start( vl, node_name_fmt );

    SCOREP_Platform_SystemTreePathElement* new_element =
        create_element( domains,
                        node_class,
                        node_name_len,
                        node_name_fmt,
                        vl );

    va_end( vl );

    if ( !new_element )
    {
        return NULL;
    }

    /* prepend to path */
    new_element->next = *head;
    *head             = new_element;

    return new_element;
}

SCOREP_Platform_SystemTreeProperty*
scorep_platform_system_tree_add_property(
    SCOREP_Platform_SystemTreePathElement* node,
    const char*                            property_name,
    size_t                                 property_value_len,
    const char*                            property_value_fmt,
    ... )
{
    if ( !node || !property_name || !property_value_fmt )
    {
        return NULL;
    }

    bool use_vl = true;
    if ( property_value_len == 0 )
    {
        property_value_len = strlen( property_value_fmt ) + 1;
        use_vl             = false;
    }

    SCOREP_Platform_SystemTreeProperty* new_property;
    size_t                              property_name_offset  = sizeof( *new_property );
    size_t                              property_name_len     = strlen( property_name ) + 1;
    size_t                              property_value_offset = property_name_offset + property_name_len;
    size_t                              total_size            = property_value_offset + property_value_len;

    new_property = malloc( total_size );
    if ( !new_property )
    {
        return NULL;
    }

    new_property->next           = NULL;
    new_property->property_name  = ( char* )new_property + property_name_offset;
    new_property->property_value = ( char* )new_property + property_value_offset;

    /* set class name */
    memcpy( new_property->property_name, property_name, property_name_len );

    /* set node name */
    if ( use_vl )
    {
        va_list vl;
        va_start( vl, property_value_fmt );

        /* The caller provided a format string suitable for snprintf */
        vsnprintf( new_property->property_value, property_value_len, property_value_fmt, vl );

        va_end( vl );
    }
    else
    {
        /* The caller provided a fixed string */
        memcpy( new_property->property_value, property_value_fmt, property_value_len );
    }

    /* chain into property list of node */
    new_property->next     = NULL;
    *node->properties_tail = new_property;
    node->properties_tail  = &new_property->next;

    return new_property;
}
