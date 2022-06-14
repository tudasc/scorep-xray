/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2019-2020, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 *
 */

#include <config.h>

#include <SCOREP_Thread_Mgmt.h>

#if HAVE( SYS_SYSCALL_H )
#include <sys/syscall.h>
#include <unistd.h>
#endif

uint64_t
SCOREP_Thread_GetOSId( void )
{
#if HAVE( DECL_SYS_GETTID )
    return syscall( SYS_gettid );
#elif HAVE( DECL_SYS_THREAD_SELFID )
    return syscall( SYS_thread_selfid );
#else
    return SCOREP_INVALID_TID;
#endif
}
