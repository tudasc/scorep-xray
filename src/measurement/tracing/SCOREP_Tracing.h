#ifndef SCOREP_TRACING_H
#define SCOREP_TRACING_H

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
 * @file       SCOREP_Tracing.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <otf2/otf2.h>
#include <stdint.h>


void
SCOREP_Tracing_Initialize( void );


void
SCOREP_Tracing_Finalize( void );


/**
 * Signals, that this process is the master for the OTF2 archive.
 */
SCOREP_Error_Code
SCOREP_Tracing_SetIsMaster( bool isMaster );


/**
 * Get an event writer.
 */
OTF2_EvtWriter*
SCOREP_Tracing_GetEventWriter( void );

/**
 *  Closes all event writers and collect the number of written events for each.
 *
 *  Needs to be called before @a SCOREP_Tracing_WriteDefinitions.
 */
void
SCOREP_Tracing_FinalizeEventWriters( void );

/**
 *  Writes all definitions into the OTF2 archive.
 */
void
SCOREP_Tracing_WriteDefinitions( void );


#endif /* SCOREP_TRACING_H */
