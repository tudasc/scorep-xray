/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2017,
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
 * Implements a basic config file parser.
 */

#include <config.h>
#include "scorep_tools_config_parser.hpp"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>

SCOREP_Tools_ConfigParser::SCOREP_Tools_ConfigParser()
{
}


SCOREP_ErrorCode
SCOREP_Tools_ConfigParser::readConfigFile( const std::string& file )
{
    std::ifstream in_file;
    in_file.open( file.c_str() );

    if ( in_file.good() )
    {
        while ( in_file.good() )
        {
            std::string line;
            getline( in_file, line );
            read_parameter( line );
        }
        return SCOREP_SUCCESS;
    }
    else
    {
        return SCOREP_ERROR_FILE_CAN_NOT_OPEN;
    }
}


SCOREP_ErrorCode
SCOREP_Tools_ConfigParser::read_parameter( const std::string& line )
{
    /* check for comments */
    std::string::size_type end = line.find( "#" );
    if ( end == 0 )
    {
        return SCOREP_SUCCESS;                      // Whole line commented out
    }
    if ( end == std::string::npos )
    {
        end = line.length();
    }

    /* separate value and key */
    std::string::size_type pos = line.substr( 0, end ).find( "=" );
    if ( pos == std::string::npos )
    {
        return SCOREP_ERROR_PARSE_NO_SEPARATOR;
    }
    std::string key   = line.substr( 0, pos );
    std::string value = line.substr( pos + 1, end - pos - 1 );

    set_value( key, value );

    return SCOREP_SUCCESS;
}
