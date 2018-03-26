/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Implements the class for online access selection
 */

#include <config.h>
#include "scorep_instrumenter_online_access.hpp"
#include <iostream>

/* **************************************************************************************
 * class SCOREP_Instrumenter_OnlineAccess
 * *************************************************************************************/
SCOREP_Instrumenter_OnlineAccess::SCOREP_Instrumenter_OnlineAccess( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_ONLINE_ACCESS,
                                   "online-access" )
{
}

void
SCOREP_Instrumenter_OnlineAccess::printHelp( void )
{
    std::cout
        << "\
  --online-access Enables online-access support. It is disabled by default.\n\
  --noonline-access\n\
                  Disables online-access support."
        << std::endl;
}

std::string
SCOREP_Instrumenter_OnlineAccess::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    if ( !isEnabled() )
    {
        return " --no" + m_name;
    }
    return "";
}
