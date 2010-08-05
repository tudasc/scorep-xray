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


#include <SILC_Allocator.h>
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
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_destroy_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    omp_destroy_lock( ( omp_lock_t* )silc_memory_guard_object_ptr );
#endif
}
