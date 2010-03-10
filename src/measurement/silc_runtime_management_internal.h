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
 * Toplevel relative experiment directory.
 *
 * @todo add program name and at finalization time nProcs and nLocations, like
 * silc_prgName_nProcsxnLocs.
 * @todo assert that we don't overwrite if directory exists.
 */
extern char silc_experiment_dir[];

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

uint64_t
post_flush( void );

#endif /* SILC_RUNTIME_MANAGEMENT_INTERNAL_H */
