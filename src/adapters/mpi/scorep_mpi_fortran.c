/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017, 2019,
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
 * @ingroup    MPI_Wrapper
 *
 * @brief Contains the initialization function implementations for the
 * MPI fortran support.
 */

#include <config.h>

#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include "scorep_mpi_fortran.h"
#include "SCOREP_Fmpi.h"
#include "SCOREP_Mpi.h"

#include <stdlib.h>

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUS_SIZE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_status_size.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUS_SIZE \
    F77_FUNC_( scorep_fortran_get_mpi_status_size, \
               SCOREP_FORTRAN_GET_MPI_STATUS_SIZE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_BOTTOM
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_bottom.
 */
#define SCOREP_FORTRAN_GET_MPI_BOTTOM \
    F77_FUNC_( scorep_fortran_get_mpi_bottom, \
               SCOREP_FORTRAN_GET_MPI_BOTTOM )

/**
 * @def SCOREP_FORTRAN_GET_MPI_IN_PLACE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_in_place.
 */
#define SCOREP_FORTRAN_GET_MPI_IN_PLACE \
    F77_FUNC_( scorep_fortran_get_mpi_in_place, \
               SCOREP_FORTRAN_GET_MPI_IN_PLACE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_status_ignore.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE \
    F77_FUNC_( scorep_fortran_get_mpi_status_ignore, \
               SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_statuses_ignore.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE \
    F77_FUNC_( scorep_fortran_get_mpi_statuses_ignore, \
               SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_UNWEIGHTED
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_unweighted.
 */
#define SCOREP_FORTRAN_GET_MPI_UNWEIGHTED \
    F77_FUNC_( scorep_fortran_get_mpi_unweighted, \
               SCOREP_FORTRAN_GET_MPI_UNWEIGHTED )

/**
   address of fortran variable used as reference for MPI_BOTTOM
 */
void* scorep_mpi_fortran_bottom = NULL;

/**
   address of fortran variable used as reference for MPI_IN_PLACE
 */
void* scorep_mpi_fortran_in_place = NULL;

/**
   address of fortran variable used as reference for MPI_STATUS_IGNORE
 */
void* scorep_mpi_fortran_status_ignore = NULL;

/**
   address of fortran variable used as reference for MPI_STATUSES_IGNORE
 */
void* scorep_mpi_fortran_statuses_ignore = NULL;

/**
   address of fortran variable used as reference for MPI_UNWEIGHTED
 */
void* scorep_mpi_fortran_unweighted = NULL;

/**
   External fortran function to retrieve the constant value
   MPI_STATUS_SIZE defined in Fortran MPI. It is used for Fortran-C
   conversions.
   @param mpi_status_size Address of a variable where the value is to
   be stored.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUS_SIZE( int* status_size );

/**
   External fortran function to trigger a callback which sets MPI_BOTTOM.
 */
extern void
SCOREP_FORTRAN_GET_MPI_BOTTOM( void );

/**
   External fortran function to trigger a callback which sets MPI_IN_PLACE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_IN_PLACE( void );

/**
   External fortran function to trigger a callback which sets MPI_STATUS_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE( void );

/**
   External fortran function to trigger a callback which sets
   MPI_STATUSES_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE( void );

/**
   External fortran function to trigger a callback which sets
   MPI_STATUSES_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_UNWEIGHTED( void );

void
scorep_mpi_fortran_init( void )
{
    UTILS_DEBUG_ENTRY();

    /* Set Fortran constants */
    /* The ifdef guard makes sure we skip the Fortran things in case we have no Fortran */
    SCOREP_FORTRAN_GET_MPI_STATUS_SIZE( &scorep_mpi_status_size );
#if HAVE( MPI_BOTTOM )
    SCOREP_FORTRAN_GET_MPI_BOTTOM();
#endif
#if HAVE( MPI_IN_PLACE )
    SCOREP_FORTRAN_GET_MPI_IN_PLACE();
#endif
#if HAVE( MPI_STATUS_IGNORE )
    SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE();
#endif
#if HAVE( MPI_STATUSES_IGNORE )
    SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE();
#endif
#if HAVE( MPI_UNWEIGHTED )
    SCOREP_FORTRAN_GET_MPI_UNWEIGHTED();
#endif
}
