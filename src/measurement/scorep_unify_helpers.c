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


/**
 * @file       src/measurement/scorep_unify_helpers.c
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include "scorep_unify_helpers.h"

#include <stdlib.h>

#include <UTILS_Error.h>

#include <SCOREP_Definitions.h>
#include <definitions/SCOREP_Definitions.h>
#include <scorep_ipc.h>

uint32_t
scorep_unify_helper_define_comm_locations( SCOREP_GroupType type,
                                           const char*      name,
                                           uint64_t         numberOfMembers,
                                           const uint64_t*  localMembers )
{
    uint32_t size                         = SCOREP_Ipc_GetSize();
    uint32_t rank                         = SCOREP_Ipc_GetRank();
    int*     number_of_locations_per_rank = NULL;
    int*     offsets                      = NULL;

    if ( rank == 0 )
    {
        number_of_locations_per_rank =
            calloc( 2 * size, sizeof( *number_of_locations_per_rank ) );
        UTILS_ASSERT( number_of_locations_per_rank );
        offsets = number_of_locations_per_rank + size;
    }

    /* Collect the number of locations per rank into rank 0 */
    int number_of_members = numberOfMembers;
    SCOREP_Ipc_Gather( &number_of_members,
                       number_of_locations_per_rank,
                       1,
                       SCOREP_IPC_INT,
                       0 );

    /* Calculate the exclusive prefix sum for each tank */
    uint32_t total_number_of_locations = 0;
    if ( rank == 0 )
    {
        for ( uint32_t i = 0; i < size; i++ )
        {
            offsets[ i ]               = total_number_of_locations;
            total_number_of_locations += number_of_locations_per_rank[ i ];
        }
    }

    /* Distribute the per-rank offsets */
    int offset_to_global;
    SCOREP_Ipc_Scatter( offsets,
                        &offset_to_global,
                        1,
                        SCOREP_IPC_INT,
                        0 );

    /* Gather all location ids from the ranks. */
    uint64_t* all_locations = NULL;
    if ( 0 == rank )
    {
        all_locations = calloc( total_number_of_locations,
                                sizeof( *all_locations ) );
        UTILS_ASSERT( all_locations );
    }
    SCOREP_Ipc_Gatherv( ( void* )localMembers,
                        number_of_members,
                        all_locations,
                        number_of_locations_per_rank,
                        offsets,
                        SCOREP_IPC_UINT64,
                        0 );
    free( number_of_locations_per_rank );

    if ( 0 == rank )
    {
        SCOREP_Definitions_NewUnifiedGroup( type,
                                            name,
                                            total_number_of_locations,
                                            all_locations );
        free( all_locations );
    }

    return offset_to_global;
}
