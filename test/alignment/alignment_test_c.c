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

#include <config.h>
#include <stdint.h>
#include <assert.h>


/* *INDENT-OFF* */
int64_t FORTRAN_ALIGNED FORTRAN_MANGLED( foo )    = INT64_MAX;
int64_t FORTRAN_ALIGNED FORTRAN_MANGLED( bar )    = INT64_MIN;
int64_t FORTRAN_ALIGNED FORTRAN_MANGLED( foobar ) = 0;
int64_t FORTRAN_ALIGNED FORTRAN_MANGLED( baz )    = 42;
int64_t FORTRAN_ALIGNED FORTRAN_MANGLED( foobaz ) = -42;


void
FORTRAN_MANGLED(check)()
{
    assert( FORTRAN_MANGLED( foo )    == INT64_MAX - 1 );
    assert( FORTRAN_MANGLED( bar )    == INT64_MIN + 1 );
    assert( FORTRAN_MANGLED( foobar ) == -1 );
    assert( FORTRAN_MANGLED( baz )    == -1 );
    assert( FORTRAN_MANGLED( foobaz ) == 1 );
}
/* *INDENT-ON* */
