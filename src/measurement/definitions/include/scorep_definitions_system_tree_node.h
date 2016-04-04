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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_SYSTEM_TREE_NODE_H
#define SCOREP_PRIVATE_DEFINITIONS_SYSTEM_TREE_NODE_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( SystemTreeNode )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SystemTreeNode );

    SCOREP_SystemTreeNodeHandle parent_handle;
    SCOREP_SystemTreeDomain     domains;
    SCOREP_StringHandle         name_handle;
    SCOREP_StringHandle         class_handle;

    /* Chain of all properties for this node. */
    SCOREP_SystemTreeNodePropertyHandle  properties;
    SCOREP_SystemTreeNodePropertyHandle* properties_tail;
};


/**
 * Associate a system tree node with a process unique system tree node handle.
 *
 * @param parent Parent node in system tree.
 *
 * @param domains Bit set of domains this node spans.
 *
 * @param klass Class of the system tree node.
 *
 * @param name A meaningful name for the system tree node.
 *
 * @return A process unique system tree node handle.
 *
 */
SCOREP_SystemTreeNodeHandle
SCOREP_Definitions_NewSystemTreeNode( SCOREP_SystemTreeNodeHandle parent,
                                      SCOREP_SystemTreeDomain     domains,
                                      const char*                 klass,
                                      const char*                 name );


void
scorep_definitions_unify_system_tree_node( SCOREP_SystemTreeNodeDef*            definition,
                                           struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_SYSTEM_TREE_NODE_H */
