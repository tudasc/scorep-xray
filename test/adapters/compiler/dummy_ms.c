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
#include <SILC_Events.h>
#include <SILC_Adapter.h>
#include <silc_utility/SILC_Utils.h>
#include <SILC_Compiler_Init.h>

int test = 0;

void
SILC_LockSourceFileDefinition()
{
}

void
SILC_UnlockSourceFileDefinition()
{
}

void
SILC_LockRegionDefinition()
{
}

void
SILC_UnlockRegionDefinition()
{
}

SILC_SourceFileHandle
SILC_DefineSourceFile
(
    const char* fileName
)
{
    return ( SILC_SourceFileHandle )1;
}

SILC_RegionHandle
SILC_DefineRegion
(
    const char*           regionName,
    SILC_SourceFileHandle fileHandle,
    SILC_LineNo           beginLine,
    SILC_LineNo           endLine,
    SILC_AdapterType      adapter,
    SILC_RegionType       regionType
)
{
    return ( SILC_RegionHandle )1;
}

void
SILC_InitMeasurement( void )
{
    SILC_Compiler_Adapter.adapter_register();
    SILC_Compiler_Adapter.adapter_init();
    SILC_Compiler_Adapter.adapter_init_location();
}

SILC_Error_Code
SILC_ConfigRegister
(
    const char*          nameSpace,
    SILC_ConfigVariable* variables
)
{
    return SILC_SUCCESS;
}

void
SILC_EnterRegion
(
    SILC_RegionHandle regionHandle
)
{
    printf( "Enter Region\n" );
    test = 1;
}

void
SILC_ExitRegion
(
    SILC_RegionHandle regionHandle
)
{
    printf( "Exit Region\n" );
    if ( test == 1 )
    {
        exit( EXIT_SUCCESS );
    }
}
