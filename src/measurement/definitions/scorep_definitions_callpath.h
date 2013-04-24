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

#ifndef SCOREP_PRIVATE_DEFINITIONS_CALLPATH_H
#define SCOREP_PRIVATE_DEFINITIONS_CALLPATH_H


/**
 * @file       src/measurement/definitions/scorep_definitions_callpath.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( Callpath )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Callpath );

    // Add SCOREP_Callpath stuff from here on.
    SCOREP_CallpathHandle parent_callpath_handle;

    /* This controlls the callpath_argument union */
    bool with_parameter;
    union
    {
        SCOREP_RegionHandle    region_handle;
        SCOREP_ParameterHandle parameter_handle;
    } callpath_argument;

    /* This is controlled by the type of the parameter when
     * with_parameter is true
     */
    union
    {
        SCOREP_StringHandle string_handle;
        int64_t             integer_value;
    } parameter_value;
};


SCOREP_CallpathHandle
SCOREP_Definitions_NewCallpath( SCOREP_CallpathHandle parent,
                                SCOREP_RegionHandle   region );


SCOREP_CallpathHandle
SCOREP_Definitions_NewCallpathParameterInteger( SCOREP_CallpathHandle  parent,
                                                SCOREP_ParameterHandle param,
                                                int64_t                value );


SCOREP_CallpathHandle
SCOREP_Definitions_NewCallpathParameterString( SCOREP_CallpathHandle  parent,
                                               SCOREP_ParameterHandle param,
                                               SCOREP_StringHandle    value );


void
SCOREP_CopyCallpathDefinitionToUnified( SCOREP_CallpathDef*           definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager );


/**
 * Returns the sequence number of the unified definitions for a local callpath handle from
 * the mappings.
 * @param handle handle to local callpath handle.
 */
uint32_t
SCOREP_Callpath_GetUnifiedSequenceNumber( SCOREP_CallpathHandle handle );


/**
 * Returns the unified handle from a local handle.
 * @param handle Handle of a local callpath.
 */
SCOREP_CallpathHandle
SCOREP_Callpath_GetUnifiedHandle( SCOREP_CallpathHandle handle );


#endif /* SCOREP_PRIVATE_DEFINITIONS_CALLPATH_H */
