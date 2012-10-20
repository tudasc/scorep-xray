/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file scorep_profile_collapse.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
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
#include <inttypes.h>

static SCOREP_RegionHandle scorep_profile_collapse_region = SCOREP_INVALID_REGION;

static void
substitute_collapse( SCOREP_Location* location, scorep_profile_node* node, void* param )
{
    if ( node->node_type == scorep_profile_node_collapse )
    {
        node->node_type = scorep_profile_node_regular_region;
        scorep_profile_type_set_region_handle( &node->type_specific_data,
                                               scorep_profile_collapse_region );
    }
}

void
scorep_profile_process_collapse( SCOREP_Location* location )
{
    if ( !scorep_profile.has_collapse_node )
    {
        return;
    }

    fprintf( stderr,
             "Score-P callpath depth limitation of %" PRIu64 " exceeded.\n"
             "Reached callpath depth was %" PRIu64 "\n",
             scorep_profile.max_callpath_depth, scorep_profile.reached_depth );

    scorep_profile_collapse_region = SCOREP_DefineRegion( "COLLAPSE",
                                                          NULL,
                                                          SCOREP_INVALID_SOURCE_FILE,
                                                          0,
                                                          0,
                                                          SCOREP_ADAPTER_USER,
                                                          SCOREP_REGION_UNKNOWN );

    scorep_profile_for_all( location,
                            scorep_profile.first_root_node,
                            &substitute_collapse,
                            NULL );
}
