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
 * @ file      tau_test.c
 *
 * @brief Test program for the TAU adapter.
 *
 * This is compile time test to insure that all the functions and types defined in the
 * SCOREP_TAU.h include file are compatible with their usage in TAU. Currently this
 * test initialization and creating starting and stopping Regions. More test
 * will be added as the TAU-SCOREP integration continues.
 */

#include <config.h>

#include <scorep/SCOREP_Tau.h>

int
main( int   argc,
      char* argv[] )
{
    SCOREP_Tau_InitMeasurement();

    long FunctionId;
    FunctionId =  SCOREP_Tau_DefineRegion( "test",
                                           SCOREP_TAU_INVALID_SOURCE_FILE,
                                           SCOREP_TAU_INVALID_LINE_NO,
                                           SCOREP_TAU_INVALID_LINE_NO,
                                           SCOREP_TAU_ADAPTER_COMPILER,
                                           SCOREP_TAU_REGION_FUNCTION
                                           );

    SCOREP_Tau_EnterRegion( FunctionId );
    SCOREP_Tau_ExitRegion( FunctionId );

    SCOREP_Tau_AddLocationProperty( "key1", "value1" );
    SCOREP_Tau_AddLocationProperty( "key2", "value2" );
    SCOREP_Tau_AddLocationProperty( "key3", "value3" );
    SCOREP_Tau_AddLocationProperty( "key4", "value4" );

    return 0;
}
