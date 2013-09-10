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

#ifndef SCOREP_PROFILE_EVENT_BASE_H
#define SCOREP_PROFILE_EVENT_BASE_H

/**
 * @file
 *
 *
 *
 * Declaration of the base event functions for enter, exit, trigger metric, ...
 */

#include <scorep_profile_node.h>

scorep_profile_node*
scorep_profile_enter( SCOREP_Profile_LocationData* location,
                      scorep_profile_node*         current_node,
                      SCOREP_RegionHandle          region,
                      SCOREP_RegionType            type,
                      uint64_t                     timestamp,
                      uint64_t*                    metrics );

scorep_profile_node*
scorep_profile_exit( SCOREP_Profile_LocationData* location,
                     scorep_profile_node*         node,
                     SCOREP_RegionHandle          region,
                     uint64_t                     timestamp,
                     uint64_t*                    metrics );

void
scorep_profile_trigger_int64( SCOREP_Profile_LocationData* location,
                              SCOREP_MetricHandle          metric,
                              uint64_t                     value,
                              scorep_profile_node*         node );

void
scorep_profile_trigger_double(  SCOREP_Profile_LocationData* location,
                                SCOREP_MetricHandle          metric,
                                double                       value,
                                scorep_profile_node*         node );


#endif /* SCOREP_PROFILE_EVENT_BASE_H */
