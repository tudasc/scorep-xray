/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @status      alpha
 * @file        scorep_system_tree.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Declaration of the system tree creation function.
 *
 *
 */

#include <SCOREP_Platform.h>

SCOREP_Platform_SystemTreePathElement*
SCOREP_BuildSystemTree();

SCOREP_LocationGroupHandle
SCOREP_Definitions_NewSystemTree( SCOREP_Platform_SystemTreePathElement* path );

void
SCOREP_FreeSystemTree( SCOREP_Platform_SystemTreePathElement* path );

void
SCOREP_FinalizeLocationGroup();

#endif // SCOREP_SYSTEM_TREE_H
