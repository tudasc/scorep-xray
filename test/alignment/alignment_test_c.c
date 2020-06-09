/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2020,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
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


#include <config.h>
#include <stdint.h>
#include <assert.h>


/* *INDENT-OFF* */
int64_t FORTRAN_ALIGNED F77_FUNC( foo, FOO )    = INT64_MAX;
int64_t FORTRAN_ALIGNED F77_FUNC( bar, BAR )    = INT64_MIN;
int64_t FORTRAN_ALIGNED F77_FUNC( foobar, FOOBAR ) = 0;
int64_t FORTRAN_ALIGNED F77_FUNC( baz, BAZ )    = 42;
int64_t FORTRAN_ALIGNED F77_FUNC( foobaz, FOOBAZ ) = -42;


void
F77_FUNC(check,CHECK)()
{
  assert( F77_FUNC( foo, FOO )    == INT64_MAX - 1 );
  assert( F77_FUNC( bar, BAR )    == INT64_MIN + 1 );
  assert( F77_FUNC( foobar, FOOBAR ) == -1 );
  assert( F77_FUNC( baz, BAZ )    == -1 );
  assert( F77_FUNC( foobaz, FOOBAZ ) == 1 );
}
/* *INDENT-ON* */
