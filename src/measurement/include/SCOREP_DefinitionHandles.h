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

#ifndef SCOREP_DEFINITIONHANDLES_H
#define SCOREP_DEFINITIONHANDLES_H



/**
 * @file       SCOREP_DefinitionHandles.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdint.h>


#include <scorep_utility/SCOREP_Allocator.h>


typedef SCOREP_Allocator_MovableMemory SCOREP_StringHandle;

typedef SCOREP_Allocator_MovableMemory SCOREP_SystemTreeNodeHandle;

typedef SCOREP_Allocator_MovableMemory SCOREP_LocationGroupHandle;

typedef SCOREP_Allocator_MovableMemory SCOREP_LocationHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_GroupHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_MPICommunicatorHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_MPIWindowHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_MPICartesianTopologyHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_MPICartesianCoordsHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_IOFileGroupHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_IOFileHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_MarkerGroupHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_MarkerHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_ParameterHandle;


typedef SCOREP_Allocator_MovableMemory SCOREP_CallpathHandle;


#endif /* SCOREP_DEFINITIONHANDLES_H */
