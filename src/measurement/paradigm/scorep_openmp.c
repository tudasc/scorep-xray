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

/**
 * @file scorep_openmp.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @status alpha
 *
 * Abstacts the usage of some OpenMP routines by providing an implementation in case
 * no OpenMP is used.
 *
 */

#include <config.h>
#include <scorep_utility/SCOREP_Omp.h>
#include <scorep_openmp.h>

int
SCOREP_Omp_InParallel()
{
    return omp_in_parallel();
}
