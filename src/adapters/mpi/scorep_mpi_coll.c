/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#include <config.h>

#include "scorep_mpi_coll.h"

#define COUNT_T int
#define COUNT_FUN( name ) name
#define TYPE_SIZE_FUN PMPI_Type_size

#include "scorep_mpi_coll.inc.c"

#undef TYPE_SIZE_FUN
#undef COUNT_T
#undef COUNT_FUN

/*
 * Large counts
 */
#if HAVE( MPI_4_0_SYMBOL_PMPI_TYPE_SIZE_C )
#define TYPE_SIZE_FUN PMPI_Type_size_c
#elif HAVE( MPI_3_0_SYMBOL_PMPI_TYPE_SIZE_X )
#define TYPE_SIZE_FUN PMPI_Type_size_x
#endif

#if defined( TYPE_SIZE_FUN )

#define COUNT_T MPI_Count
#define COUNT_FUN( name ) name ##_c

#include "scorep_mpi_coll.inc.c"

#undef COUNT_T
#undef COUNT_FUN

#endif
