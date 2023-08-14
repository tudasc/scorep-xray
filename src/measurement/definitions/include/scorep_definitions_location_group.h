/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2023,
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

#ifndef SCOREP_PRIVATE_DEFINITIONS_LOCATION_GROUP_H
#define SCOREP_PRIVATE_DEFINITIONS_LOCATION_GROUP_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( LocationGroup )
{
    SCOREP_DEFINE_DEFINITION_HEADER( LocationGroup );

    SCOREP_StringHandle         name_handle;
    SCOREP_SystemTreeNodeHandle system_tree_parent;
    SCOREP_LocationGroupType    location_group_type;
    SCOREP_LocationGroupHandle  creating_location_group;

    /* Tracking info to avoid writing out empty locations groups. */
    bool has_children;
};


SCOREP_LocationGroupHandle
SCOREP_Definitions_NewLocationGroup( const char*                 name,
                                     SCOREP_SystemTreeNodeHandle systemTreeParent,
                                     SCOREP_LocationGroupType    locationGroupType,
                                     SCOREP_LocationGroupHandle  creatingLocationGroup );


void
SCOREP_LocationGroupHandle_SetName( SCOREP_LocationGroupHandle handle,
                                    const char*                name );


void
scorep_definitions_unify_location_group( SCOREP_LocationGroupDef*             definition,
                                         struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_LOCATION_GROUP_H */
