/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>
#include "scorep_instrumenter_linktime_wrapping.hpp"
#include "scorep_instrumenter_adapter.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_LinktimeWrappingAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_LinktimeWrappingAdapter::SCOREP_Instrumenter_LinktimeWrappingAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING, "linktime-wrapping" )
{
#if !HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
    unsupported();
#endif
}


void
SCOREP_Instrumenter_LinktimeWrappingAdapter::printHelp( void )
{
}

bool
SCOREP_Instrumenter_LinktimeWrappingAdapter::checkCommand( const std::string& current,
                                                           const std::string& next )
{
    if ( SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( current ) )
    {
        m_usage = disabled;
    }

    return false;
}

std::string
SCOREP_Instrumenter_LinktimeWrappingAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    return "";
}
