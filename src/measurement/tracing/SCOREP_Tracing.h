/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_TRACING_H
#define SCOREP_TRACING_H


/**
 * @file
 *
 */


#include <stdlib.h>


#include <SCOREP_ErrorCodes.h>


SCOREP_ErrorCode
SCOREP_Tracing_Register( void );


/**
   Initializes the tracing substrate.
   @param substrateId Id to access tracing-specific location and task data.
 */
void
SCOREP_Tracing_Initialize( size_t substrateId );


/**
   Finalizes the tracing substrate.
   @return The substrate id that was passed to SCOREP_Tracing_Initialize().
 */
size_t
SCOREP_Tracing_Finalize( void );


/**
 * Called when the MPP was initialized.
 */
void
SCOREP_Tracing_OnMppInit( void );


/**
 *  Closes all event writers and collect the number of written events for each.
 *
 *  Needs to be called before @a SCOREP_Tracing_WriteDefinitions.
 */
void
SCOREP_Tracing_FinalizeEventWriters( void );

/**
 *  Writes properties and definitions into the OTF2 archive.
 */
void
SCOREP_Tracing_Write( void );


#endif /* SCOREP_TRACING_H */
