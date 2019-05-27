/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SCOREP_FMPI_H
#define SCOREP_FMPI_H

/** @file
    @brief Main file for Fortran interface measurement wrappers.

    Includes all Fortran wrappers for MPI functions. Fortran compilers decorate the
    C-calls in different ways. Thus, the macro FSUB must be set to SCOREP_FORTRAN_SUB1,
    SCOREP_FORTRAN_SUB2, SCOREP_FORTRAN_SUB3, or SCOREP_FORTRAN_SUB4 to obtain correctly
    decorated functions. The form of the decoration depends on the used compiler.

    You can compile this wrappers four times with different decorations, to have all
    possible names for the Fortran wrappers available.

    The wrapped decorated functions call the C-wrappers for MPI. Thus, the same set of
    events are generated as with the C functions.
 */

#include "scorep_mpi_fortran.h"
#include "SCOREP_Mpi.h"
#include <SCOREP_Fortran_Wrapper.h>
#include <SCOREP_InMeasurement.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#if defined( __sun ) || defined( _SX ) || defined( OPEN_MPI ) || defined( HP_MPI )
#  define NEED_F2C_CONV
#endif

char*
scorep_mpi_f2c_string( const char*              f_string,
                       scorep_fortran_charlen_t length );


#if HAVE( MPI_BOTTOM )
/** pointer representing the Fortran value for MPI_BOTTOM */
extern void* scorep_mpi_fortran_bottom;
#endif

#if HAVE( MPI_IN_PLACE )
/** pointer representing the Fortran value for MPI_IN_PLACE */
extern void* scorep_mpi_fortran_in_place;
#endif

#if HAVE( MPI_STATUS_IGNORE )
/** pointer representing the Fortran value for MPI_STATUS_IGNORE */
extern void* scorep_mpi_fortran_status_ignore;
#endif

#if HAVE( MPI_STATUSES_IGNORE )
/** pointer representing the Fortran value for MPI_STATUSES_IGNORE */
extern void* scorep_mpi_fortran_statuses_ignore;
#endif

#if HAVE( MPI_UNWEIGHTED )
/** pointer representing the Fortran value for MPI_STATUSES_IGNORE */
extern void* scorep_mpi_fortran_unweighted;
#endif

#if !( HAVE( DECL_PMPI_STATUS_F2C ) )
#define PMPI_Status_f2c( f, c ) memcpy( ( c ), ( f ), scorep_mpi_status_size )
#endif /* !HAVE( DECL_PMPI_STATUS_F2C ) */

#if !( HAVE( DECL_PMPI_STATUS_C2F ) )
#define PMPI_Status_c2f( c, f ) memcpy( ( f ), ( c ), scorep_mpi_status_size )
#endif /* !HAVE( DECL_PMPI_STATUS_C2F ) */

#endif /* SCOREP_FMPI_H */
