/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * Contains the implementation to make all visit counts inclusive. It expects that
 * the visit counts are exclusive.
 */

#include <config.h>
#include <SCOREP_Profile.h>
#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>

static void
scorep_profile_inclusify_visits_rek( scorep_profile_node* node )
{
    if ( node->first_child != NULL )
    {
        scorep_profile_inclusify_visits_rek( node->first_child );
        node->count += scorep_profile_get_number_of_child_calls( node );
    }
    if ( node->next_sibling != NULL )
    {
        scorep_profile_inclusify_visits_rek( node->next_sibling );
    }
}

void
scorep_profile_inclusify_visits( void )
{
    if ( scorep_profile.first_root_node != NULL )
    {
        scorep_profile_inclusify_visits_rek( scorep_profile.first_root_node );
    }
}
