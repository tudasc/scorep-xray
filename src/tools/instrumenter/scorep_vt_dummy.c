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

/**
 * @status     alpha
 * @file       scorep_vt_dummy.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
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
