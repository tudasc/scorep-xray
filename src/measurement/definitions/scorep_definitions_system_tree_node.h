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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_SYSTEM_TREE_NODE_H
#define SCOREP_PRIVATE_DEFINITIONS_SYSTEM_TREE_NODE_H


/**
 * @file       src/measurement/definitions/scorep_definitions_system_tree_node.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( SystemTreeNode )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SystemTreeNode );

    SCOREP_SystemTreeNodeHandle parent_handle;
    SCOREP_StringHandle         name_handle;
    SCOREP_StringHandle         class_handle;
};


SCOREP_SystemTreeNodeHandle
SCOREP_Definitions_NewSystemTreeNode( SCOREP_SystemTreeNodeHandle parent,
                                      const char*                 name,
                                      const char*                 klass );


void
SCOREP_CopySystemTreeNodeDefinitionToUnified( SCOREP_SystemTreeNodeDef*     definition,
                                              SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_SYSTEM_TREE_NODE_H */
