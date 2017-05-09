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

/**
 * @file
 *
 * When linking applications with icc and the -tcollect flag for compiler
 * instrumentation, icc tries to link a libVT. To avoid linking problems,
 * if the original libVT is not correctly set, we provide a dummy libVT.
 * No functions from libVT are ever used. This file contains only a dummy
 * functions in order to build the library.
 * If a real libVT is available, it does not matter, because no functions
 * of it are used, we overwrote the compiler instrumentation functions.
 */

#include <config.h>
#include <stdio.h>

/**
 * Dummy function for the VT dummy library. Should never be called.
 */
void
scorep_vt_dummy( void )
{
    printf( "Score-P dummy library for libVT\n" );
}
