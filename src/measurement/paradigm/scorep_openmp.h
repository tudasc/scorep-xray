/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_OPENMP_H
#define SCOREP_OPENMP_H

/**
 * @file scorep_openmp.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @status alpha
 *
 * This file contain abstraction functions for omp functions. In order to use the
 * omp functions, the code that calls these functions must be compiled in an omp and
 * an non-omp version. This is still true when using the macros in SCOREP_Omp.h from
 * the utilities, else the calls are substituted by 0 by the compiler. In order to
 * avoid this, we can provide. an abstraction.
 */

/**
   Provides an abstraction for omp_in_parallel().
 */
int
SCOREP_Omp_InParallel();

#endif
