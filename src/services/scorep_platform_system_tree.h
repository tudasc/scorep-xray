#ifndef SCOREP_INTERNAL_PLATFORM_SYSTEM_TREE_H
#define SCOREP_INTERNAL_PLATFORM_SYSTEM_TREE_H

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
 * @file        src/services/scorep_platform_system_tree.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <stdarg.h>


/**
 * Use this function if you build the path top-down, ie. the first one added is
 * the root.
 *
 * @param path           The path.
 * @param node_class     Name of the class for this node.
 * @param name_len       Strict upper bound for the length of the name of this
 *                       node, including the trailing NUL.
 *                       As special case, if @a node_len is zero, than @a
 *                       node_name_fmt should not have format specifiers
 *                       and the length is taken from the @a node_name_fmt.
 * @param node_name_fmt  A @a printf-like format string which forms the name of
 *                       the node.
 * @param ...            Arguments for use in @a node_name_fmt.
 */
SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_top_down_add(
    SCOREP_Platform_SystemTreePathElement*** tail,
    const char*                              node_class,
    size_t                                   name_len,
    const char*                              node_name_fmt,
    ... );


/**
 * Use this function if you build the path bottom-up, ie. the last one added
 * is the root.
 *
 * @param path           The path.
 * @param node_class     Name of the class for this node.
 * @param name_len       Strict upper bound for the length of the name of this
 *                       node, including the trailing NUL.
 *                       As special case, if @a node_len is zero, than @a
 *                       node_name_fmt should not have format specifiers
 *                       and the length is taken from the @a node_name_fmt.
 * @param node_name_fmt  A @a printf-like format string which forms the name of
 *                       the node.
 * @param ...            Arguments for use in @a node_name_fmt.
 */
SCOREP_Platform_SystemTreePathElement*
scorep_platform_system_tree_bottom_up_add(
    SCOREP_Platform_SystemTreePathElement** head,
    const char*                             node_class,
    size_t                                  name_len,
    const char*                             node_name_fmt,
    ... );


#endif /* SCOREP_INTERNAL_PLATFORM_SYSTEM_TREE_H */
