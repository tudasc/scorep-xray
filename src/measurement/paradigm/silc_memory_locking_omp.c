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
 * @file       silc_memory_locking_omp.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <SILC_AllocatorImpl.h>
#include <omp.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


static omp_lock_t          silc_memory_guard_object;
SILC_Allocator_GuardObject silc_memory_guard_object_ptr = ( SILC_Allocator_GuardObject )( &silc_memory_guard_object );
SILC_Allocator_Guard       silc_memory_lock             = ( SILC_Allocator_Guard )omp_set_lock;
SILC_Allocator_Guard       silc_memory_unlock           = ( SILC_Allocator_Guard )omp_unset_lock;


void
silc_memory_guard_initialze()
{
    omp_init_lock( ( omp_lock_t* )silc_memory_guard_object_ptr );
}


void
silc_memory_guard_finalize()
{
    omp_destroy_lock( ( omp_lock_t* )silc_memory_guard_object_ptr );
}
