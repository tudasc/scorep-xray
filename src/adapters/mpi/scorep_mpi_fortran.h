/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOERP_FORTRAN_H
#define SCOERP_FORTRAN_H

/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Helper functions to convert fortran and C data types.
 */

#include <stddef.h>

/**
 * Converts a Fortran string into a C-string.
 * @param f_string Pointer to the fortran string.
 * @param length   Number of bytes in the fortran string.
 * @returns the trimmed C-string. The allocated memory must be freed by the caller.
 */
char*
scorep_f2c_string( const char*              f_string,
                   scorep_fortran_charlen_t length );


#endif /* SCOERP_FORTRAN_H */
