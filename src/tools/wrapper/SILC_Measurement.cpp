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

#include <config.h>
#include "SILC_Measurement.hpp"

/**
 * @status alpha
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @file SILC_Measurement.cpp
 *
 * class to steer the run of the instrumented application
 */


SILC_Measurement::SILC_Measurement()
    : user_command( "" )
{
}

SILC_Measurement::~SILC_Measurement ()
{
}

SILC_Error_Code
SILC_Measurement::ParseCmdLine
(
    int    argc,
    char** argv
)
{
    for ( int i = 2; i < argc; i++ )
    {
        user_command += " ";
        user_command +=  argv[ i ];
    }
    return SILC_SUCCESS;
}


int
SILC_Measurement::Run()
{
    return system( user_command.c_str() );
}

void
SILC_Measurement::PrintParameter()
{
}
