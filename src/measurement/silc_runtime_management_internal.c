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
 * @file       silc_runtime_management_internal.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 */


#include "silc_runtime_management_internal.h"

#include <SILC_Timing.h>

char              silc_experiment_dir[] = "silc";

OTF2_ArchiveData* silc_otf2_archive = 0;

bool              silc_profiling_enabled = false;

bool              silc_tracing_enabled = true;

bool              flush_done = false;

uint64_t
post_flush( void )
{
    /* remember that we have flushed the first time
     * after this point, we can't switch into MPI mode anymore
     */
    flush_done = true;

    return SILC_GetClockTicks();
}
