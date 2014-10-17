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

#ifndef SCOREP_PRIVATE_DEFINITIONS_IO_FILE_H
#define SCOREP_PRIVATE_DEFINITIONS_IO_FILE_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( IOFile )
{
    SCOREP_DEFINE_DEFINITION_HEADER( IOFile );

    // Add SCOREP_IOFile stuff from here on.
};


/**
 * Associate a name and a group handle with a process unique I/O file handle.
 *
 * @param name A meaningful name for the I/O file.
 *
 * @param ioFileGroup Handle to the group the I/O file is associated to.
 *
 * @return A process unique I/O file handle to be used in calls to
 * SCOREP_TriggerIOFile().
 *
 * @planned To be implemented in milestone 2
 */
SCOREP_IOFileHandle
SCOREP_Definitions_NewIOFile( const char*              name,
                              SCOREP_IOFileGroupHandle ioFileGroup );


#endif /* SCOREP_PRIVATE_DEFINITIONS_IO_FILE_H */
