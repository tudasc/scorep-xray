/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOERP_FORTRAN_H
#define SCOERP_FORTRAN_H

/**
 * @file       scorep_mpi_fortran.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief Helper functions to convert fortran and C data types.
 */

#include <stdint.h>

/**
 * Converts a Fortran string into a C-string.
 * @param f_string Pointer to the fortran string.
 * @param length   Number of bytes in the fortran string.
 * @returns the trimmed C-string. The allocated memory must be freed by the caller.
 */
char*
scorep_f2c_string( const char* f_string,
                   uint32_t    length );


#endif
