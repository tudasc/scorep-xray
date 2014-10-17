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

#ifndef SCOREP_PRIVATE_DEFINITIONS_CALLPATH_H
#define SCOREP_PRIVATE_DEFINITIONS_CALLPATH_H


/**
 * @file
 *
 *
 */



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
scorep_definitions_unify_callpath( SCOREP_CallpathDef*                  definition,
                                   struct SCOREP_Allocator_PageManager* handlesPageManager );


/**
 * Returns the sequence number of the unified definitions for a local callpath handle from
 * the mappings.
 * @param handle handle to local callpath handle.
 */
uint32_t
SCOREP_CallpathHandle_GetUnifiedId( SCOREP_CallpathHandle handle );


/**
 * Returns the unified handle from a local handle.
 * @param handle Handle of a local callpath.
 */
SCOREP_CallpathHandle
SCOREP_CallpathHandle_GetUnified( SCOREP_CallpathHandle handle );


#endif /* SCOREP_PRIVATE_DEFINITIONS_CALLPATH_H */
