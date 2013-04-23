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

#ifndef SCOREP_PRIVATE_DEFINITIONS_RMA_WINDOW_H
#define SCOREP_PRIVATE_DEFINITIONS_RMA_WINDOW_H


/**
 * @file       src/measurement/definitions/scorep_definitions_rma_window.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


#include <jenkins_hash.h>


SCOREP_DEFINE_DEFINITION_TYPE( RmaWindow )
{
    SCOREP_DEFINE_DEFINITION_HEADER( RmaWindow );

    // Add SCOREP_RmaWindow stuff from here on.
    SCOREP_StringHandle              name_handle;
    SCOREP_InterimCommunicatorHandle communicator_handle;
};


#endif /* SCOREP_PRIVATE_DEFINITIONS_RMA_WINDOW_H */