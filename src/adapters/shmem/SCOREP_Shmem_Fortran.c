/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 * @ingroup    SHMEM_Wrapper
 *
 * @brief Fortran interface wrappers
 */

#include <config.h>
#include <shmem.h>

#include "scorep_shmem_internal.h"

#include "SCOREP_Fortran_Wrapper.h"

#include <SCOREP_InMeasurement.h>

/*
 * Handle complex numbers in C / C++
 */

#ifdef __cplusplus
# include <complex>
# define COMPLEXIFY( T ) std::complex < T >
#else /* _cplusplus */
# include <complex.h>
# define COMPLEXIFY( T ) T complex
#endif /* __cplusplus */

/* -- Fortran wrapper functions -- */

#include "fortran_common/scorep_fshmem_atomic.inc.c"

#include "fortran_common/scorep_fshmem_broadcast.inc.c"

#include "fortran_common/scorep_fshmem_cache.inc.c"

#include "fortran_common/scorep_fshmem_collect.inc.c"

#include "fortran_common/scorep_fshmem_data.inc.c"

#include "fortran_common/scorep_fshmem_event.inc.c"

#include "fortran_common/scorep_fshmem_init.inc.c"

#include "fortran_common/scorep_fshmem_lock.inc.c"

#include "fortran_common/scorep_fshmem_query.inc.c"

#include "fortran_common/scorep_fshmem_reduction.inc.c"

#include "fortran_common/scorep_fshmem_remote_ptr.inc.c"

//#include "fortran_common/scorep_fshmem_sym_heap.inc.c"

#include "fortran_common/scorep_fshmem_sync.inc.c"

#include "fortran_common/scorep_fshmem_team.inc.c"
