/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_PRIVATE_DEFINITIONS_MARKER_H
#define SCOREP_PRIVATE_DEFINITIONS_MARKER_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( Marker )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Marker );

    // Add SCOREP_Marker stuff from here on.
};


/**
 * Associate a name and a group handle with a process unique marker handle.
 *
 * @param name A meaningful name for the marker
 *
 * @param markerGroup Handle to the group the marker is associated to.
 *
 * @return A process unique marker handle to be used in calls to
 * SCOREP_TriggerMarker().
 *
 * @planned To be implemented in milestone 2
 */
SCOREP_MarkerHandle
SCOREP_Definitions_NewMarker( const char*              name,
                              SCOREP_MarkerGroupHandle markerGroup );


#endif /* SCOREP_PRIVATE_DEFINITIONS_MARKER_H */
