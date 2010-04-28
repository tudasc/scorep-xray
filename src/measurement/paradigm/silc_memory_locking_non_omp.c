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
 * @file       silc_memory_locking_non_omp.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <SILC_AllocatorImpl.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


SILC_Allocator_Guard       silc_memory_lock             = 0;
SILC_Allocator_Guard       silc_memory_unlock           = 0;
SILC_Allocator_GuardObject silc_memory_guard_object_ptr = 0;


void
silc_memory_guard_initialze()
{
}


void
silc_memory_guard_finalize()
{
}
