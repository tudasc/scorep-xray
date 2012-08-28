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
 * @file        src/services/scorep_platform_system_tree.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
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


void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreePathElement* path )
{
    while ( path )
    {
        SCOREP_Platform_SystemTreePathElement* next = path->next;

        free( path );

        path = next;
    }
}



/**
 *
 * @param node_name_len Total space alloacted for the node_name, including trailing NUL.
 */
static SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_create_element( const char* node_class,
                                            size_t      node_name_len,
                                            const char* node_name_fmt,
                                            va_list     vl )
{
    SCOREP_Platform_SystemTreePathElement* new_element;
    size_t                                 total_size, name_offset, class_offset;
    size_t                                 class_len;

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

    name_offset  = sizeof( *new_element );
    class_offset = name_offset + node_name_len;
    class_len    = strlen( node_class ) + 1;
    total_size   = class_offset + class_len;

    new_element = malloc( total_size );
    if ( !new_element )
    {
        return NULL;
    }

    new_element->next       = NULL;
    new_element->node_name  = ( char* )new_element + name_offset;
    new_element->node_class = ( char* )new_element + class_offset;

    /* set node name */
    if ( use_vl )
    {
        /* The caller provided a format string suitable for snprintf */
        vsnprintf( new_element->node_name, node_name_len, node_name_fmt, vl );
    }
    else
    {
        /* The caller proviede a fixed string */
        memcpy( new_element->node_name, node_name_fmt, node_name_len );
    }

    /* set class name */
    memcpy( new_element->node_class, node_class, class_len );

    return new_element;
}


SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_top_down_add(
    SCOREP_Platform_SystemTreePathElement*** tail,
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
        scorep_platform_system_tree_create_element( node_class,
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
        scorep_platform_system_tree_create_element( node_class,
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
