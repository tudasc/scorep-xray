/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_PLATFORM_SYSTEM_TREE_H
#define SCOREP_INTERNAL_PLATFORM_SYSTEM_TREE_H

/**
 * @file
 *
 *
 */


#include <SCOREP_Platform.h>

#include <stdarg.h>


/**
 * Platform dependent function to built-up the path from the given root node
 * (this is the machine node) down to the shared memory node.
 *
 * @param root The machine node, i.e. the root node.
 */
SCOREP_ErrorCode
scorep_platform_get_path_in_system_tree( SCOREP_Platform_SystemTreePathElement* root );


/**
 * Use this function if you build the path top-down, i.e. the first one added is
 * the root.
 *
 * @param tail           The tail of the path.
 * @param node_class     Name of the class for this node.
 * @param node_name_len  Strict upper bound for the length of the name of this
 *                       node, including the trailing NUL.
 *                       As a special case, if @a node_name_len is zero, than @a
 *                       node_name_fmt should not have format specifiers
 *                       and the length is taken from the @a node_name_fmt.
 * @param node_name_fmt  A @a printf-like format string which forms the name of
 *                       the node.
 * @param ...            Arguments for use in @a node_name_fmt.
 */
SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_top_down_add(
    SCOREP_Platform_SystemTreePathElement*** tail,
    SCOREP_SystemTreeDomain                  domains,
    const char*                              node_class,
    size_t                                   node_name_len,
    const char*                              node_name_fmt,
    ... );


/**
 * Use this function if you build the path bottom-up, i.e. the last one added
 * is the root.
 *
 * @param head           The head of the path.
 * @param node_class     Name of the class for this node.
 * @param node_name_len  Strict upper bound for the length of the name of this
 *                       node, including the trailing NUL.
 *                       As a special case, if @a node_name_len is zero, than @a
 *                       node_name_fmt should not have format specifiers
 *                       and the length is taken from the @a node_name_fmt.
 * @param node_name_fmt  A @a printf-like format string which forms the name of
 *                       the node.
 * @param ...            Arguments for use in @a node_name_fmt.
 */
SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_bottom_up_add(
    SCOREP_Platform_SystemTreePathElement** head,
    SCOREP_SystemTreeDomain                 domains,
    const char*                             node_class,
    size_t                                  node_name_len,
    const char*                             node_name_fmt,
    ... );


/**
 * Adds a property to a system tree node.
 *
 * @param node               The node in question.
 * @param property_name      Name of the property.
 * @param property_value_len Strict upper bound for the length of the property
 *                           value for this property, including the trailing NUL.
 *                           As a special case, if @a property_value_len is zero,
 *                           than @a property_value_fmt should not have format
 *                           specifiers and the length is taken from the @a
 *                           property_value_fmt.
 * @param property_value_fmt A @a printf-like format string which forms the
 *                           value of the property.
 * @param ...                Arguments for use in @a property_value_fmt.
 *
 * @return The newly created property or NULL in case of error.
 */
SCOREP_Platform_SystemTreeProperty*
scorep_platform_system_tree_add_property(
    SCOREP_Platform_SystemTreePathElement* node,
    const char*                            property_name,
    size_t                                 property_value_len,
    const char*                            property_value_fmt,
    ... );


#endif /* SCOREP_INTERNAL_PLATFORM_SYSTEM_TREE_H */
