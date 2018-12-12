/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Implements the class for cobi instrumentation.
 */

#include <config.h>
#include "scorep_instrumenter_cobi.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_CobiAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_CobiAdapter::SCOREP_Instrumenter_CobiAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_COBI, "cobi" )
{
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_COMPILER );
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );

    m_cobi            = SCOREP_COBI_PATH;
    m_cobi_config_dir = SCOREP_DATADIR;
    m_use_params      = true;

#if !HAVE( COBI )
    unsupported();
#endif
}

std::string
SCOREP_Instrumenter_CobiAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    return "";
}

void
SCOREP_Instrumenter_CobiAdapter::postlink( SCOREP_Instrumenter&         instrumenter,
                                           SCOREP_Instrumenter_CmdLine& cmdLine )
{
    std::string output_name = cmdLine.getOutputName();
    std::string orig_name   = output_name + ".orig";
    if ( cmdLine.getVerbosity() >= 1 )
    {
        std::cerr << "mv "
                  << output_name
                  << " " << orig_name << std::endl;
    }
    if ( !cmdLine.isDryRun() )
    {
        if ( rename( output_name.c_str(), orig_name.c_str() ) != 0 )
        {
            UTILS_ERROR_POSIX( "Failed to rename binary" );
            exit( EXIT_FAILURE );
        }
    }

    std::string adapter_base = m_cobi_config_dir + "/SCOREP_Cobi_Adapter.xml";
    std::string command;

    /* Construct adapter file */
    std::string adapter = output_name + ".cobi.xml";
    instrumenter.executeCommand( "head -n9 " + adapter_base + " > " + adapter );
    instrumenter.executeCommand( instrumenter.getConfigBaseCall() +
                                 " --cobi-deps >> " + adapter );
    instrumenter.executeCommand( "tail -n+11 " + adapter_base + " >> " + adapter );

    /* Call cobi */
    command = m_cobi + " " + m_params
              + " -a " + adapter
              + " -b " + orig_name
              + " -f " + m_cobi_config_dir + "/SCOREP_Cobi_Filter.xml"
              + " -o " + output_name;

    instrumenter.executeCommand( command );

    instrumenter.addTempFile( orig_name );
    instrumenter.addTempFile( adapter );
}

void
SCOREP_Instrumenter_CobiAdapter::setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine )
{
    m_cobi_config_dir = cmdLine.getPathToBinary() + "../share";
}

void
SCOREP_Instrumenter_CobiAdapter::setConfigValue( const std::string& key,
                                                 const std::string& value )
{
    if ( key == "COBI_CONFIG_DIR" && value != "" )
    {
        m_cobi_config_dir = value;
    }
}
