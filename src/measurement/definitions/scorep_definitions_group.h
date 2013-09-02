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

#ifndef SCOREP_PRIVATE_DEFINITIONS_GROUP_H
#define SCOREP_PRIVATE_DEFINITIONS_GROUP_H


/**
 * @file       src/measurement/definitions/scorep_definitions_group.h
 *
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( Group )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Group );

    /* OTF2 groups have a uint64_t as id, we use the sequences number,
     * until we know, that this may clash with location ids
     */

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
scorep_definitions_unify_group( SCOREP_GroupDef*              definition,
                                SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_GROUP_H */
