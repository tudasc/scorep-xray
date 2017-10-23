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
 * Copyright (c) 2009-2012, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
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
scorep_profile_trigger_int64( SCOREP_Profile_LocationData*         location,
                              SCOREP_MetricHandle                  metric,
                              uint64_t                             value,
                              scorep_profile_node*                 node,
                              scorep_profile_trigger_update_scheme scheme );

void
scorep_profile_trigger_double(  SCOREP_Profile_LocationData*         location,
                                SCOREP_MetricHandle                  metric,
                                double                               value,
                                scorep_profile_node*                 node,
                                scorep_profile_trigger_update_scheme scheme );


#endif /* SCOREP_PROFILE_EVENT_BASE_H */
