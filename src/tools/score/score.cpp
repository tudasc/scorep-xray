/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file       score.cxx
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Implements the main function of the scorep-score tool.
 */


#include <config.h>
#include <SCOREP_Score_Profile.hpp>
#include <SCOREP_Score_Estimator.hpp>

int
main( int argc, char** argv )
{
    SCOREP_Score_Profile profile( argv[ 1 ] );
    profile.Print();

    SCOREP_Score_Estimator estimator( &profile );
    estimator.DumpEventSizes();

    return 0;
}
