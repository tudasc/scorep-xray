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

#ifndef SCOREP_INFO_COMMAND_OPEN_ISSUES_HPP
#define SCOREP_INFO_COMMAND_OPEN_ISSUES_HPP

/**
 * @file
 *
 * open-issues
 */

#include "scorep_info_command.hpp"

#include <string>
#include <vector>

/* **************************************************************************************
 * class SCOREP_Info_Command_OpenIssues
 * *************************************************************************************/

class SCOREP_Info_Command_OpenIssues
    : public SCOREP_Info_Command
{
public:
    SCOREP_Info_Command_OpenIssues();

    virtual
    ~SCOREP_Info_Command_OpenIssues();

    int
    run( const std::vector<std::string>& args );
};

#endif // SCOREP_INFO_COMMAND_OPEN_ISSUES_HPP
