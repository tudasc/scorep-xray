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
 * @file       scorep_cluster.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */
#ifndef SCOREP_PROFILE_CLUSTER_H
#define SCOREP_PROFILE_CLUSTER_H

#include "scorep_profile_node.h"

/**
 * Initializes the clustering system. Is called during the profile initialization.
 */
void
scorep_cluster_initialize( void );

/**
 * Finalizes the clsutering system. Is called during profile finalization.
 */
void
scorep_cluster_finalize( void );

/**
 * Identifies the regions that is clustered. Is called when a dynamic region
 * is entered.
 * @param location  The profile location object.
 * @param node      The profile node of the dyamic region.
 */
void
scorep_cluster_on_enter_dynamic(  SCOREP_Profile_LocationData* location,
                                  scorep_profile_node*         node );

/**
 * Integrates a new iteration into the clustering system. It is called during the
 * iteration node exit. Creates either a new cluster or merges it into an existing
 * cluster.
 * @param location  The profile location object.
 * @param node      The profile node that represents the iteration root node.
 *                  It is the instance parameter node.
 */
void
scorep_cluster_if_necessary( SCOREP_Profile_LocationData* location,
                             scorep_profile_node*         node );

/**
 * Postprocesses the clsutering. Is called during the profil epostprocessing.
 * Sorts and reenumerates the clusters.
 */
void
scorep_cluster_postprocess( void );

#endif /* SCOREP_PROFILE_CLUSTER_H */
