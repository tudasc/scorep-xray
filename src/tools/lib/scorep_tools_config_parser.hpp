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

#ifndef SCOREP_TOOLS_CONFIG_PARSER_HPP
#define SCOREP_TOOLS_CONFIG_PARSER_HPP

/**
 * @file
 *
 * Implements a basic config file parser.
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>

class SCOREP_Tools_ConfigParser
{
public:
    SCOREP_Tools_ConfigParser();

    /**
       Reads configuration data from a config file
     */
    SCOREP_ErrorCode
    readConfigFile( const std::string& file );

private:
    /**
       Extracts parameter from configuration file
       It expects lines of the format key=value. Furthermore it truncates line
       at the script comment character '#'.
       @param line    input line from the config file
       @returns SCOREP_SUCCESS if the line was successfully parsed. Else it
                returns an error code.
     */
    SCOREP_ErrorCode
    read_parameter( const std::string& line );

    virtual void
    set_value( const std::string& key,
               const std::string& value ) = 0;
};

#endif // SCOREP_TOOLS_CONFIG_PARSER_HPP
