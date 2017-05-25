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

#ifndef SCOREP_INFO_COMMAND_CONFIG_SUMMARY_HPP
#define SCOREP_INFO_COMMAND_CONFIG_SUMMARY_HPP

/**
 * @file
 *
 * config-summary
 */

#include "scorep_info_command.hpp"

#include <string>
#include <vector>

/* **************************************************************************************
 * class SCOREP_Info_Command_ConfigSummary
 * *************************************************************************************/

class SCOREP_Info_Command_ConfigSummary
    : public SCOREP_Info_Command
{
public:
    SCOREP_Info_Command_ConfigSummary();

    virtual
    ~SCOREP_Info_Command_ConfigSummary();

    int
    run( const std::vector<std::string>& args );
};

#endif // SCOREP_INFO_COMMAND_CONFIG_SUMMARY_HPP
