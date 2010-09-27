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
 * @ file      dummy_ms.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Dummy measurement system for compiler adapter test.
 *
 * Implements the functions called by the compiler adapter for the compiler
 * adapter test. The intention of the test is to check whether the compiler
 * adapter generates events (enter/exit). In order to archive this, we needed
 * to replace the original measurement system by an implementation which
 * allows to run a sequential compiler instrumented program. The dummy enter
 * sets a flag. The dummy exit tests the flag and exits with exit code success.
 * The instrumneted program exits with an error code when it reaches its end.
 * Thus, the test script will pass if enter and exit events are generated and
 * fail otherwise.
 */

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <SCOREP_Events.h>
#include <SCOREP_Adapter.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Compiler_Init.h>

int test = 0;

void
SCOREP_LockSourceFileDefinition()
{
}

void
SCOREP_UnlockSourceFileDefinition()
{
}

void
SCOREP_LockRegionDefinition()
{
}

void
SCOREP_UnlockRegionDefinition()
{
}

SCOREP_SourceFileHandle
SCOREP_DefineSourceFile
(
    const char* fileName
)
{
    return ( SCOREP_SourceFileHandle )1;
}

SCOREP_RegionHandle
SCOREP_DefineRegion
(
    const char*             regionName,
    SCOREP_SourceFileHandle fileHandle,
    SCOREP_LineNo           beginLine,
    SCOREP_LineNo           endLine,
    SCOREP_AdapterType      adapter,
    SCOREP_RegionType       regionType
)
{
    return ( SCOREP_RegionHandle )1;
}

void
SCOREP_InitMeasurement( void )
{
    SCOREP_Compiler_Adapter.adapter_register();
    SCOREP_Compiler_Adapter.adapter_init();
    SCOREP_Compiler_Adapter.adapter_init_location();
}

SCOREP_Error_Code
SCOREP_ConfigRegister
(
    const char*            nameSpace,
    SCOREP_ConfigVariable* variables
)
{
    return SCOREP_SUCCESS;
}

void
SCOREP_EnterRegion
(
    SCOREP_RegionHandle regionHandle
)
{
    printf( "Enter Region\n" );
    test = 1;
}

void
SCOREP_ExitRegion
(
    SCOREP_RegionHandle regionHandle
)
{
    printf( "Exit Region\n" );
    if ( test == 1 )
    {
        exit( EXIT_SUCCESS );
    }
}
