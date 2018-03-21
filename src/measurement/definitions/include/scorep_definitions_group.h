/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2017,
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

#ifndef SCOREP_PRIVATE_DEFINITIONS_GROUP_H
#define SCOREP_PRIVATE_DEFINITIONS_GROUP_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( Group )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Group );

    // Add SCOREP_Group stuff from here on.
    SCOREP_GroupType    group_type;
    SCOREP_StringHandle name_handle;    // currently not used
    uint64_t            number_of_members;
    // variable array member
    uint64_t members[];
};


SCOREP_GroupHandle
SCOREP_Definitions_NewGroup( SCOREP_GroupType type,
                             const char*      name,
                             uint32_t         numberOfMembers,
                             const uint64_t*  members );


SCOREP_GroupHandle
SCOREP_Definitions_NewGroupFrom32( SCOREP_GroupType type,
                                   const char*      name,
                                   const uint32_t   numberOfRanks,
                                   const uint32_t*  ranks );


SCOREP_GroupHandle
SCOREP_Definitions_NewUnifiedGroup( SCOREP_GroupType type,
                                    const char*      name,
                                    uint32_t         numberOfMembers,
                                    const uint64_t*  members );


SCOREP_GroupHandle
SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GroupType type,
                                          const char*      name,
                                          uint32_t         numberOfMembers,
                                          const uint32_t*  members );


void
scorep_definitions_unify_group( SCOREP_GroupDef*                     definition,
                                struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_GROUP_H */
