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


#include <scorep/SCOREP_PublicTypes.h>


typedef SCOREP_AnyHandle SCOREP_StringHandle;


typedef SCOREP_AnyHandle SCOREP_SystemTreeNodeHandle;


typedef SCOREP_AnyHandle SCOREP_LocationGroupHandle;


typedef SCOREP_AnyHandle SCOREP_LocationHandle;


typedef SCOREP_AnyHandle SCOREP_GroupHandle;


typedef SCOREP_AnyHandle SCOREP_LocalMPICommunicatorHandle;


typedef SCOREP_AnyHandle SCOREP_MPICommunicatorHandle;


typedef SCOREP_AnyHandle SCOREP_MPIWindowHandle;


typedef SCOREP_AnyHandle SCOREP_MPICartesianTopologyHandle;


typedef SCOREP_AnyHandle SCOREP_MPICartesianCoordsHandle;


typedef SCOREP_AnyHandle SCOREP_IOFileGroupHandle;


typedef SCOREP_AnyHandle SCOREP_IOFileHandle;


typedef SCOREP_AnyHandle SCOREP_MarkerGroupHandle;


typedef SCOREP_AnyHandle SCOREP_MarkerHandle;


typedef SCOREP_AnyHandle SCOREP_ParameterHandle;


typedef SCOREP_AnyHandle SCOREP_CallpathHandle;


#endif /* SCOREP_DEFINITIONHANDLES_H */
