#ifndef SILC_DEFINITIONHANDLES_H
#define SILC_DEFINITIONHANDLES_H

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       SILC_DefinitionHandles.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdint.h>


#include <SILC_Allocator.h>


typedef SILC_Allocator_MovableMemory SILC_StringHandle;


typedef SILC_Allocator_MovableMemory SILC_LocationHandle;


/* MPI Communicators are represented as an OTF2 group */
typedef SILC_Allocator_MovableMemory SILC_GroupHandle;
typedef SILC_GroupHandle             SILC_MPICommunicatorHandle;


typedef SILC_Allocator_MovableMemory SILC_MPIWindowHandle;


typedef SILC_Allocator_MovableMemory SILC_MPICartesianTopologyHandle;


typedef SILC_Allocator_MovableMemory SILC_MPICartesianCoordsHandle;


typedef SILC_Allocator_MovableMemory SILC_IOFileGroupHandle;


typedef SILC_Allocator_MovableMemory SILC_IOFileHandle;


typedef SILC_Allocator_MovableMemory SILC_MarkerGroupHandle;


typedef SILC_Allocator_MovableMemory SILC_MarkerHandle;


typedef SILC_Allocator_MovableMemory SILC_ParameterHandle;


typedef SILC_Allocator_MovableMemory SILC_CallpathHandle;


#endif /* SILC_DEFINITIONHANDLES_H */
