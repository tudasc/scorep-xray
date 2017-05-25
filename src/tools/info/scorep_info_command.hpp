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

#ifndef SCOREP_INFO_COMMAND_HPP
#define SCOREP_INFO_COMMAND_HPP

/**
 * @file
 *
 * Manages the scorep-info sub-commands
 */

#include <string>
#include <vector>
#include <map>

/* **************************************************************************************
 * class SCOREP_Info_Command
 * *************************************************************************************/

/**
 * The class SCOREP_Info_Command manages all sub-commands and delegates the run.
 */
class SCOREP_Info_Command
{
public:

    /**
     * Prints for all commands the help message.
     * @param full  Print also command options.
     * @return Success.
     */
    static int
    allPrintHelp( bool full );

    /**
     * Prints for the specified command the help message.
     * @param full  Print also command options.
     * @return Failure if the command was not found.
     */
    static int
    onePrintHelp( const std::string& command,
                  bool               full );

    /**
     * Runs the command @p command with the arguments in @p args.
     * @param command  The command to run.
     * @param args     The arguments for this command.
     * @return         Success.
     */
    static int
    run( const std::string&              command,
         const std::vector<std::string>& args );

    /**
     * Constructs a SCOREP_Info_Command.
     * @param command     The command name.
     * @param help        A one line help messages for this command.
     */
    SCOREP_Info_Command( const std::string& command,
                         const std::string& help );

    /**
     * Destructs the adapter.
     */
    virtual
    ~SCOREP_Info_Command();

protected:
    /**
     * Prints standard help output for this adapter, based on the name.
     * Overwrite this functions if you have command options.
     * @param full  Print also command options.
     */
    virtual void
    printHelp( bool full );

    /**
     * Runs the command.
     * @return The intended exit code by this command.
     */
    virtual int
    run( const std::vector<std::string>& args ) = 0;

    /**
     * The command name.
     */
    std::string m_command;

    /**
     * The help.
     */
    std::string m_help;

private:
    /**
     * List of all commands
     */
    static std::map<std::string, SCOREP_Info_Command*> all;
};

#endif // SCOREP_INFO_COMMAND_HPP
