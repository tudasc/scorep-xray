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

#ifndef SCOREP_PRIVATE_DEFINITIONS_MARKER_GROUP_H
#define SCOREP_PRIVATE_DEFINITIONS_MARKER_GROUP_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( MarkerGroup )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MarkerGroup );

    // Add SCOREP_MarkerGroup stuff from here on.
};


/**
 * Associate a name with a process unique marker group handle.
 *
 * @param name A meaningfule name of the marker group.
 *
 * @return A process unique marker group handle to be used in calls to
 * SCOREP_Definitions_NewMarker().
 *
 * @planned To be implemented in milestone 2
 */
SCOREP_MarkerGroupHandle
SCOREP_Definitions_NewMarkerGroup( const char* name );


#endif /* SCOREP_PRIVATE_DEFINITIONS_MARKER_GROUP_H */
