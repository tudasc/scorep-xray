/*
 * This file is part of the SILC project (http://www.silc.de)
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
 * @ file      tau_test.c
 * @maintainer Scott Biersdorff <scottb@cs.uoregon.edu>
 *
 * @brief Test program for the TAU adapter.
 *
 * This is compile time test to insure that all the functions and types defined in the
 * SILC_TAU.h include file are compatible with their usage in TAU. Currently this
 * test initialization and creating starting and stopping Regions. More test
 * will be added as the TAU-SILC integration continues.
 */

#include <config.h>

#include "SILC_Tau.h"

int
main( int   argc,
      char* argv[] )
{
    SILC_Tau_InitMeasurement();

    long FunctionId;
    FunctionId =  SILC_Tau_DefineRegion( "test",
                                         SILC_TAU_INVALID_SOURCE_FILE,
                                         SILC_TAU_INVALID_LINE_NO,
                                         SILC_TAU_INVALID_LINE_NO,
                                         SILC_TAU_ADAPTER_COMPILER,
                                         SILC_TAU_REGION_FUNCTION
                                         );

    SILC_Tau_EnterRegion( FunctionId );
    SILC_Tau_ExitRegion( FunctionId );
}
