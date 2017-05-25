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

#ifndef SCOREP_INFO_COMMAND_CONFIG_VARS_HPP
#define SCOREP_INFO_COMMAND_CONFIG_VARS_HPP

/**
 * @file
 *
 * config-vars
 */

#include "scorep_info_command.hpp"

#include <string>
#include <vector>

/* **************************************************************************************
 * class SCOREP_Info_Command_ConfigVars
 * *************************************************************************************/

class SCOREP_Info_Command_ConfigVars
    : public SCOREP_Info_Command
{
public:
    SCOREP_Info_Command_ConfigVars();

    virtual
    ~SCOREP_Info_Command_ConfigVars();

    void
    printHelp( bool full );

    int
    run( const std::vector<std::string>& args );
};

#endif // SCOREP_INFO_COMMAND_CONFIG_VARS_HPP
