/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_INTERNAL_RUNTIME_MANAGEMENT_H
#define SCOREP_INTERNAL_RUNTIME_MANAGEMENT_H



/**
 * @file       scorep_runtime_management.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <otf2/OTF2_Archive.h>
#include <SCOREP_Thread_Types.h>

/**
 * Toplevel relative experiment directory. In the non MPI case a valid name is
 * available after the first call to SCOREP_CreateExperimentDir(). In the MPI
 * case a valid and unique name is available on all processes after the call
 * to SCOREP_CreateExperimentDir() from within SCOREP_InitMeasurementMPI().
 *
 * @note The name is a temporary name used during measurement. At
 * scorep_finalize() we rename the temporary directory to something like
 * scorep_<prgname>_nProcs_x_nLocations.
 *
 * @todo rename directory in scorep_finalize().
 */
char*
SCOREP_GetExperimentDirName();


/**
 * Create a directory with a temporary name (accessible via
 * SCOREP_GetExperimentDirName()) to store all measurment data inside.
 *
 * @note There is a MPI implementation and a non-MPI implementation.
 *
 */
void
SCOREP_CreateExperimentDir();


void
SCOREP_RenameExperimentDir();


uint64_t
SCOREP_CalculateGlobalLocationId( SCOREP_Thread_LocationData* locationData );


/**
 * Archive for trace data. One per process, can contain multiple "location"
 * writers.
 */
extern OTF2_Archive* scorep_otf2_archive;


/** @brief Did the first buffer flush happened, of so we can't switch to MPI
 *  anymore.
 */
extern bool flush_done;


/**
 * Called per location by OTF2 after the location buffer has been flushed.
 *
 * @return The clock ticks of this event.
 */
uint64_t
SCOREP_OnTraceAndDefinitionPostFlush( void );


/**
 * Called per location by OTF2 before the location buffer will be flushed.
 *
 */
OTF2_FlushType
SCOREP_OnTracePreFlush( void* evtWriter,
                        void* evtReader );


#endif /* SCOREP_INTERNAL_RUNTIME_MANAGEMENT_H */
