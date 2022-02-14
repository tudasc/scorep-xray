/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 * @brief Demangling
 */

#ifndef SCOREP_COMPILER_DEMANGLE_H
#define SCOREP_COMPILER_DEMANGLE_H

#include <stdlib.h>
#include <SCOREP_Demangle.h>


#define scorep_compiler_demangle( mangled, demangled ) \
    do \
    { \
        demangled = SCOREP_Demangle( mangled, SCOREP_DEMANGLE_DEFAULT ); \
        if ( demangled == NULL ) \
        { \
            demangled = ( char* )mangled; \
            mangled   = NULL; \
        } \
    } \
    while ( 0 )


#define scorep_compiler_demangle_free( mangled, demangled ) \
    do \
    { \
        if ( mangled && demangled ) \
        { \
            free( demangled ); \
        } \
    } \
    while ( 0 )

#endif /* SCOREP_COMPILER_DEMANGLE_H */
