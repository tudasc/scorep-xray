#ifndef SCOREP_PLATFORM_H
#define SCOREP_PLATFORM_H

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       SCOREP_Platform.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <UTILS_Error.h>

UTILS_BEGIN_C_DECLS

/**
 * Contains one element of the system tree path. Used to specifiy a path in the
 * system tree
 */
typedef struct SCOREP_Platform_SystemTreeProperty
{
    struct SCOREP_Platform_SystemTreeProperty* next;
    char*                                      property_name;
    char*                                      property_value;
} SCOREP_Platform_SystemTreeProperty;


/**
 * Contains one element of the system tree path. Used to specifiy a path in the
 * system tree
 */
typedef struct SCOREP_Platform_SystemTreePathElement
{
    struct SCOREP_Platform_SystemTreePathElement* next;
    SCOREP_SystemTreeDomain                       domains;
    char*                                         node_class;
    char*                                         node_name;
    SCOREP_Platform_SystemTreeProperty*           properties;
    SCOREP_Platform_SystemTreeProperty**          properties_tail;
} SCOREP_Platform_SystemTreePathElement;


/**
 * Returns the path of this process in the system tree, starting at the root.
 *
 * The path can be traversed with the @a SCOREP_PLATFORM_SYSTEM_TREE_FORALL()
 * macro.
 *
 * The path should be freed with @a SCOREP_Platform_FreePath().
 *
 * @param root A pointer to a @a SCOREP_Platform_SystemTreePathElement* element
 *             which represents the root of the system tree and the start
 *             of the path for this process in the system tree.
 *
 * @return SCOREP_SUCCESS on success else an appropriate error code.
 */
extern SCOREP_ErrorCode
SCOREP_Platform_GetPathInSystemTree( SCOREP_Platform_SystemTreePathElement** root );

/**
 * Defines a more detailed system topology at node level if available.
 *
 * @param parent A handle to a System Tree Node element which represents
 *               the node level and is used as start point of the topology
 *               information at node level.
 *
 * @return SCOREP_SUCCESS on success else an appropriate error code.
 */
extern SCOREP_ErrorCode
SCOREP_Platform_DefineNodeTree( SCOREP_SystemTreeNodeHandle parent );

/**
 * Deletes the path returned from @a SCOREP_Platform_GetPathInSystemTree().
 *
 * @param path Pointer to the returned path from @a SCOREP_Platform_GetPathInSystemTree
 *             that should be freed.
 */
extern void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreePathElement* path );

/**
 * Returns a unique node identifier.
 */
extern int32_t
SCOREP_Platform_GetNodeId( void );

/**
 * Used to iterate over a system tree path starting at the root.
 */
#define SCOREP_PLATFORM_SYSTEM_TREE_FORALL( _path, _node ) \
    for ( _node = _path; _node; _node = _node->next )

/**
 * Used to iterate over all properties of an system tree node.
 */
#define SCOREP_PLATFORM_SYSTEM_TREE_PROPERTY_FORALL( _node, _property ) \
    for ( _property = _node->properties; _property; _property = _property->next )

UTILS_END_C_DECLS

#endif /* SCOREP_PLATFORM_H */
