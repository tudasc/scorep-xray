/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file       scorep_memory_locking_non_omp.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <scorep_utility/SCOREP_Allocator.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


SCOREP_Allocator_Guard       scorep_memory_lock             = 0;
SCOREP_Allocator_Guard       scorep_memory_unlock           = 0;
SCOREP_Allocator_GuardObject scorep_memory_guard_object_ptr = 0;


void
scorep_memory_guard_initialze()
{
}


void
scorep_memory_guard_finalize()
{
}
