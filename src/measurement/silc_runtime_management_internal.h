#ifndef SILC_RUNTIME_MANAGEMENT_INTERNAL_H
#define SILC_RUNTIME_MANAGEMENT_INTERNAL_H

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_runtime_management_internal.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 */

#include <OTF2_Archive.h>

/**
 * Toplevel relative experiment directory. In the non MPI case a valid name is
 * available after the first call to SILC_CreateExperimentDir(). In the MPI
 * case a valid and unique name is available on all processes after the call
 * to SILC_CreateExperimentDir() from within SILC_InitMeasurementMPI().
 *
 * @note The name is a temporary name used during measurement. At
 * silc_finalize() we rename the temporary directory to something like
 * silc_<prgname>_nProcs_x_nLocations.
 *
 * @todo rename directory in silc_finalize().
 */
char*
SILC_GetExperimentDirName();


/**
 * Create a directory with a temporary name (accessible via
 * SILC_GetExperimentDirName()) to store all measurment data inside.
 *
 * @note There is a MPI implementation and a non-MPI implementation.
 *
 */
void
SILC_CreateExperimentDir();


bool
SILC_ExperimentDirIsCreated();


/**
 * Archive for trace data. One per process, can contain multiple "location"
 * writers.
 */
extern OTF2_ArchiveData* silc_otf2_archive;

extern bool              silc_profiling_enabled;

extern bool              silc_tracing_enabled;

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
silc_on_trace_post_flush( void );


/**
 * Called per location by OTF2 before the location buffer will be flushed.
 *
 * @param filePath huh, I don't know
 *
 * @return The path to the experiment archive.
 */
//char*
//silc_on_trace_pre_flush( const char* filePath );


#endif /* SILC_RUNTIME_MANAGEMENT_INTERNAL_H */
