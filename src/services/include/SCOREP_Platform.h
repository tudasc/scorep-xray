#ifndef SCOREP_PLATFORM_H
#define SCOREP_PLATFORM_H

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
 * @file       SCOREP_Platform.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

UTILS_BEGIN_C_DECLS

/**
 * Contains one element of the system tree path. Used to specifiy a path in the
 * system tree
 */
typedef struct SCOREP_Platform_SystemTreePathElement
{
    struct SCOREP_Platform_SystemTreePathElement* next;
    char*                                         node_name;
    char*                                         node_class;
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
 * Deletes the path returned from @a SCOREP_Platform_GetPathInSystemTree().
 *
 * @param path Pointer to the returned path from @a SCOREP_Platform_GetPathInSystemTree
 *             that should be freed.
 */
extern void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreePathElement* path );

/**
 * Used to iterate over a system tree path starting at the root.
 */
#define SCOREP_PLATFORM_SYSTEM_TREE_FORALL( _path, _node ) \
    for ( _node = _path; _node; _node = _node->next )

UTILS_END_C_DECLS

#endif /* SCOREP_PLATFORM_H */
