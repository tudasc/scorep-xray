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

#ifndef SCOREP_PRIVATE_DEFINITIONS_IO_FILE_GROUP_H
#define SCOREP_PRIVATE_DEFINITIONS_IO_FILE_GROUP_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( IOFileGroup )
{
    SCOREP_DEFINE_DEFINITION_HEADER( IOFileGroup );

    // Add SCOREP_IOFileGroup stuff from here on.
};


/**
 * Associate a name with a process unique I/O file group handle.
 *
 * @param name A meaningfule name of the I/O file group.
 *
 * @return A process unique file I/O file group handle to be used in calls to
 * SCOREP_Definitions_NewIOFile().
 *
 * @planned To be implemented in milestone 2
 */
SCOREP_IOFileGroupHandle
SCOREP_Definitions_NewIOFileGroup( const char* name );


#endif /* SCOREP_PRIVATE_DEFINITIONS_IO_FILE_GROUP_H */
