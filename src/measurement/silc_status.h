#ifndef SILC_STATUS_H
#define SILC_STATUS_H

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_status.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdbool.h>

typedef struct silc_status silc_status;
struct silc_status
{
    int  mpi_rank;
    bool mpi_rank_is_set;
    bool mpi_is_initialized;
    bool mpi_is_finalized;
    int  mpi_comm_world_size;
    bool is_experiment_dir_created;
    bool is_profiling_enabled;
    bool is_tracing_enabled;
    bool otf2_has_flushed;
};

extern silc_status status;

// different impl for MPI and non-MPI
void
SILC_Status_Initialize();


bool
SILC_IsTracingEnabled();


bool
SILC_IsProfilingEnabled();


void
SILC_Otf2_OnFlush();


bool
SILC_Otf2_HasFlushed();


bool
SILC_IsExperimentDirCreated();


void
SILC_OnExperimentDirCreation();

#endif /* SILC_STATUS_H */
