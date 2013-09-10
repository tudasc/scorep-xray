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

#ifndef SCOREP_SYSTEM_TREE_H
#define SCOREP_SYSTEM_TREE_H

/**
 * @file
 *
 * @brief   Declaration of the system tree creation function.
 *
 *
 */

#include <SCOREP_Platform.h>

SCOREP_Platform_SystemTreePathElement*
SCOREP_BuildSystemTree( void );

SCOREP_LocationGroupHandle
SCOREP_DefineSystemTree( SCOREP_Platform_SystemTreePathElement* path );

void
SCOREP_FreeSystemTree( SCOREP_Platform_SystemTreePathElement* path );

SCOREP_SystemTreeNodeHandle
SCOREP_GetSystemTreeRootNodeHandle( void );

SCOREP_SystemTreeNodeHandle
SCOREP_GetSystemTreeNodeHandleForSharedMemory( void );

void
SCOREP_FinalizeLocationGroup( void );

#endif // SCOREP_SYSTEM_TREE_H
