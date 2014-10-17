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

#ifndef SCOREP_PRIVATE_DEFINITIONS_PARAMETER_H
#define SCOREP_PRIVATE_DEFINITIONS_PARAMETER_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( Parameter )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Parameter );

    // Add SCOREP_Parameter stuff from here on.
    SCOREP_StringHandle  name_handle;
    SCOREP_ParameterType parameter_type;
};


/**
 * Associate a name and a type with a process unique parameter handle.
 *
 * @param name A meaningful name for the parameter.
 *
 * @param type The type of the parameter.
 *
 * @return A process unique parameter handle to be used in calls to
 * SCOREP_TriggerParameter().
 *
 * @planned To be implemented in milestone 3
 *
 */
SCOREP_ParameterHandle
SCOREP_Definitions_NewParameter( const char*          name,
                                 SCOREP_ParameterType type );


const char*
SCOREP_ParameterHandle_GetName( SCOREP_ParameterHandle handle );


SCOREP_ParameterType
SCOREP_ParameterHandle_GetType( SCOREP_ParameterHandle handle );


void
scorep_definitions_unify_parameter( SCOREP_ParameterDef*                 definition,
                                    struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_PARAMETER_H */
