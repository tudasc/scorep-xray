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

#ifndef SCOREP_PRIVATE_DEFINITIONS_TOPOLOGY_H
#define SCOREP_PRIVATE_DEFINITIONS_TOPOLOGY_H


/**
 * @file
 *
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianTopology )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPICartesianTopology );

    // Add SCOREP_MPICartesianTopology stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianCoords )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPICartesianCoords );

    // Add SCOREP_MPICartesianCoords stuff from here on.
};


#endif /* SCOREP_PRIVATE_DEFINITIONS_TOPOLOGY_H */
