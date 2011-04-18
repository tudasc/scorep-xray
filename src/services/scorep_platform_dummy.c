/*
 * This file is part of the Scorep-P project (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */



/**
 * @file       scorep_platform_dummy.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <SCOREP_Platform.h>


void
SCOREP_Platform_Initialize()
{
}


void
SCOREP_Platform_Finalize()
{
}


bool
SCOREP_Platform_ClockIsGlobal()
{
    return true;
}


const char*
SCOREP_Platform_GetName()
{
    return "foo";
}


uint32_t
SCOREP_Platform_GetNumberOfNodes()
{
    return 42;
}


uint64_t
SCOREP_Platform_GetNodeId()
{
    return 1;
}


const char*
SCOREP_Platform_GetNodeName()
{
    return "hot node";
}


uint32_t
SCOREP_Platform_GetNumberOfCPUs()
{
    return 4;
}


SCOREP_Platform_HardwareTopology*
SCOREP_Platform_GetHardwareTopology()
{
    return 0;
}
