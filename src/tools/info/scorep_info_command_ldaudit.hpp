/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INFO_COMMAND_LDAUDIT_HPP
#define SCOREP_INFO_COMMAND_LDAUDIT_HPP

/**
 * @file
 */

#include "scorep_info_command.hpp"

/* **************************************************************************************
 * class SCOREP_Info_Command_Ldaudit
 * *************************************************************************************/

class SCOREP_Info_Command_Ldaudit
    : public SCOREP_Info_Command
{
public:
    SCOREP_Info_Command_Ldaudit();

    int
    run( const std::vector<std::string>& args ) override;
};


#endif // SCOREP_INFO_COMMAND_LDAUDIT_HPP
