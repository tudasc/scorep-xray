/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 *
 * Processes collapse nodes in order to prepare them to be written in any format.
 * For this, it registers a region named "COLLAPSE" and substitutes all collapse
 * nodes by regular_region nodes with this new region.
 *
 */

#include <config.h>
#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

static SCOREP_RegionHandle scorep_profile_collapse_region = SCOREP_INVALID_REGION;

static void
substitute_collapse( scorep_profile_node* node, void* param )
{
    if ( node->node_type == SCOREP_PROFILE_NODE_COLLAPSE )
    {
        node->node_type = SCOREP_PROFILE_NODE_REGULAR_REGION;
        memset( &node->type_specific_data, 0, sizeof( node->type_specific_data ) );
        scorep_profile_type_set_region_handle( &node->type_specific_data,
                                               scorep_profile_collapse_region );
    }
}

void
scorep_profile_process_collapse( void )
{
    if ( !scorep_profile.has_collapse_node )
    {
        return;
    }

    UTILS_WARNING( "Score-P callpath depth limitation of %" PRIu64 " exceeded.\n"
                   "Reached callpath depth was %" PRIu64 ".\n"
                   "Consider setting SCOREP_PROFILING_MAX_CALLPATH_DEPTH to %" PRIu64 ".\n",
                   scorep_profile.max_callpath_depth,
                   scorep_profile.reached_depth,
                   scorep_profile.reached_depth );

    scorep_profile_collapse_region = SCOREP_Definitions_NewRegion( "COLLAPSE",
                                                                   NULL,
                                                                   SCOREP_INVALID_SOURCE_FILE,
                                                                   0,
                                                                   0,
                                                                   SCOREP_PARADIGM_USER,
                                                                   SCOREP_REGION_UNKNOWN );

    for ( scorep_profile_node* current = scorep_profile.first_root_node;
          current != NULL;
          current = current->next_sibling )
    {
        scorep_profile_for_all( current,
                                &substitute_collapse,
                                NULL );
    }
}
