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

#ifndef SCOREP_PRIVATE_DEFINITIONS_COMMUNICATOR_H
#define SCOREP_PRIVATE_DEFINITIONS_COMMUNICATOR_H


/**
 * @file       src/measurement/definitions/scorep_definitions_communicator.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <stdarg.h>

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( InterimCommunicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( InterimCommunicator );

    SCOREP_StringHandle              name_handle;
    SCOREP_InterimCommunicatorHandle parent_handle;
    SCOREP_AdapterType               adapter_type;
};


SCOREP_DEFINE_DEFINITION_TYPE( Communicator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Communicator );

    SCOREP_GroupHandle        group_handle;
    SCOREP_StringHandle       name_handle;
    SCOREP_CommunicatorHandle parent_handle;
};


SCOREP_CommunicatorHandle
SCOREP_DefineCommunicator( SCOREP_GroupHandle        group_handle,
                           const char*               name,
                           SCOREP_CommunicatorHandle parent_handle );


SCOREP_CommunicatorHandle
SCOREP_DefineUnifiedCommunicator( SCOREP_GroupHandle        group_handle,
                                  const char*               name,
                                  SCOREP_CommunicatorHandle parent_handle );


void
SCOREP_CopyCommunicatorDefinitionToUnified( SCOREP_CommunicatorDef*       definition,
                                            SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_COMMUNICATOR_H */
