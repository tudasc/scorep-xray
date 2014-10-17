/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
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

#ifndef SCOREP_PRIVATE_DEFINITIONS_LOCATION_H
#define SCOREP_PRIVATE_DEFINITIONS_LOCATION_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( Location )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Location );

    /* don't use the sequence number for the id, this is generated */
    uint64_t            global_location_id;
    SCOREP_StringHandle name_handle;
    SCOREP_LocationType location_type;
    uint64_t            number_of_events;        // only known after measurement
    uint32_t            location_group_id;
};


SCOREP_LocationHandle
SCOREP_Definitions_NewLocation( SCOREP_LocationType type,
                                const char*         name,
                                size_t              sizeOfPayload,
                                void**              payload );


void
scorep_definitions_unify_location( SCOREP_LocationDef*                  definition,
                                   struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_LOCATION_H */
