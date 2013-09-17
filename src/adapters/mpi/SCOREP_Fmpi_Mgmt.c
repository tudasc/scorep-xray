/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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
 *
 */


/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for environmental management
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

#include <stdlib.h>
#include <UTILS_Error.h>

/**
 * @name Fortran utility functions
 * @{
 */

/** lowercase define for initialization call for fortran MPI_BOTTOM */
#define scorep_mpi_fortran_init_bottom_L scorep_mpi_fortran_init_bottom
/** uppercase define for initialization call for fortran MPI_BOTTOM */
#define scorep_mpi_fortran_init_bottom_U SCOREP_MPI_FORTRAN_INIT_BOTTOM
/** lowercase define for initialization call for fortran MPI_IN_PLACE */
#define scorep_mpi_fortran_init_in_place_L scorep_mpi_fortran_init_in_place
/** uppercase define for initialization call for fortran MPI_IN_PLACE */
#define scorep_mpi_fortran_init_in_place_U SCOREP_MPI_FORTRAN_INIT_IN_PLACE
/** lowercase define for initialization call for fortran MPI_STATUS_IGNORE */
#define scorep_mpi_fortran_init_status_ignore_L scorep_mpi_fortran_init_status_ignore
/** uppercase define for initialization call for fortran MPI_STATUS_IGNORE */
#define scorep_mpi_fortran_init_status_ignore_U SCOREP_MPI_FORTRAN_INIT_STATUS_IGNORE
/** lowercase define for initialization call for fortran MPI_STATUSES_IGNORE */
#define scorep_mpi_fortran_init_statuses_ignore_L scorep_mpi_fortran_init_statuses_ignore
/** uppercase define for initialization call for fortran MPI_STATUSES_IGNORE */
#define scorep_mpi_fortran_init_statuses_ignore_U SCOREP_MPI_FORTRAN_INIT_STATUSES_IGNORE
/** lowercase define for initialization call for fortran MPI_UNWEIGHTED */
#define scorep_mpi_fortran_init_unweighted_L scorep_mpi_fortran_init_unweighted
/** uppercase define for initialization call for fortran MPI_UNWEIGHTED */
#define scorep_mpi_fortran_init_unweighted_U SCOREP_MPI_FORTRAN_INIT_UNWEIGHTED
/** address of fortran variable used as reference for MPI_UNWEIGHTED */

/** initialization function for fortran MPI_BOTTOM
 * @param bottom address of variable acting as reference for MPI_BOTTOM
 */
void
FSUB( scorep_mpi_fortran_init_bottom )( MPI_Fint* bottom )
{
#if HAVE( MPI_BOTTOM )
    scorep_mpi_fortran_bottom = bottom;
#endif
}

/** initialization function for fortran MPI_IN_PLACE
 * @param in_place address of variable acting as reference for MPI_IN_PLACE
 */
void
FSUB( scorep_mpi_fortran_init_in_place )( MPI_Fint* in_place )
{
#if HAVE( MPI_IN_PLACE )
    scorep_mpi_fortran_in_place = in_place;
#endif
}

/** initialization function for fortran MPI_STATUS_IGNORE
 * @param status_ignore address of variable acting as reference for MPI_STATUS_IGNORE
 */
void
FSUB( scorep_mpi_fortran_init_status_ignore )( MPI_Fint* status_ignore )
{
#if HAVE( MPI_STATUS_IGNORE )
    scorep_mpi_fortran_status_ignore = status_ignore;
#endif
}

/** initialization function for fortran MPI_STATUSES_IGNORE
 * @param statuses_ignore address of variable acting as reference for MPI_STATUSES_IGNORE
 */
void
FSUB( scorep_mpi_fortran_init_statuses_ignore )( MPI_Fint* statuses_ignore )
{
#if HAVE( MPI_STATUSES_IGNORE )
    scorep_mpi_fortran_statuses_ignore = statuses_ignore;
#endif
}

/** initialization function for fortran MPI_UNWEIGHTED
 * @param statuses_ignore address of variable acting as reference for MPI_UNWEIGHTED
 */
void
FSUB( scorep_mpi_fortran_init_unweighted )( MPI_Fint* unweighted )
{
#if HAVE( MPI_UNWEIGHTED )
    scorep_mpi_fortran_unweighted = unweighted;
#endif
}

/**
 * @}
 */
