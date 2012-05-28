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
#include <SCOREP_Omp.h>
#include <scorep_openmp.h>

int
SCOREP_Omp_InParallel()
{
    /* Because we use the define from SCOREP_Omp.h from the utilities, the actual code
       looks the same for the omp and non-omp version. However, it need to be compiled
       separately for each paradigm, because we use differnt ifdef branches in
       SCOREP_Omp.h
     */
    return omp_in_parallel();
}
