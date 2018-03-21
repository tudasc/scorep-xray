/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SHMEM_INTERNAL_H
#define SCOREP_SHMEM_INTERNAL_H

#if HAVE( SHMEM_PROFILING_INTERFACE )
#define SHMEM_HAVE_DECL( DECL ) HAVE( SHMEM_SYMBOL_P ## DECL )
#elif HAVE( LIBWRAP_LINKTIME_SUPPORT )
#define SHMEM_HAVE_DECL( DECL ) HAVE( SHMEM_SYMBOL_ ## DECL )
#endif

#include "SCOREP_Shmem.h"
#include <scorep/SCOREP_Libwrap_Macros.h>
#include "scorep_shmem_reg.h"

#endif /* SCOREP_SHMEM_INTERNAL_H */
