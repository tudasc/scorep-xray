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
 *
 */

#include <config.h>

#include "scorep_info_command.hpp"

#include <iostream>
#include <cstdlib>

std::map<std::string, SCOREP_Info_Command*> SCOREP_Info_Command::all;

int
SCOREP_Info_Command::allPrintHelp( bool full )
{
    std::cout << "Available info commands:" << std::endl;
    std::cout << std::endl;

    std::map<std::string, SCOREP_Info_Command*>::const_iterator it;
    for ( it = all.begin(); it != all.end(); ++it )
    {
        it->second->printHelp( full );
    }

    return EXIT_SUCCESS;
}

int
SCOREP_Info_Command::onePrintHelp( const std::string& command,
                                   bool               full )
{
    std::map<std::string, SCOREP_Info_Command*>::const_iterator it = all.find( command );
    if ( it != all.end() )
    {
        it->second->printHelp( full );
        return EXIT_SUCCESS;
    }

    std::cerr << "ERROR: Invalid info command: '" << command << "'" << std::endl;
    return EXIT_FAILURE;
}

int
SCOREP_Info_Command::run( const std::string&              command,
                          const std::vector<std::string>& args )
{
    std::map<std::string, SCOREP_Info_Command*>::iterator it = all.find( command );
    if ( it != all.end() )
    {
        return it->second->run( args );
    }

    std::cerr << "ERROR: Invalid info command: '" << command << "'" << std::endl;
    return EXIT_FAILURE;
}

SCOREP_Info_Command::SCOREP_Info_Command( const std::string& command,
                                          const std::string& help )
    :   m_command( command )
    ,   m_help( help )
{
    all.insert( std::make_pair( command, this ) );
}

SCOREP_Info_Command::~SCOREP_Info_Command()
{
}

void
SCOREP_Info_Command::printHelp( bool /* full */ )
{
    std::cout << "  " << m_command << ":\n"
              << "    " << m_help << "\n"
              << std::endl;
}
