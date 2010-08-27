/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @ file      SILC_Application.hpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Class SILC_Application
 *         base class for SILC user interaction tools
 */


#ifndef SILC_APPLICATION_H_
#define SILC_APPLICATION_H_


#include <iostream>
#include <string>

#include <silc_utility/SILC_Utils.h>


class SILC_Application
{
public:
    /* ****************************************** Implemented public methods */

    SILC_Application ();

    virtual ~
    SILC_Application();

    /**
       Reads the configuration data from a file. To handle the read data
       you need to implement the AddLibDir() AddIncDir() AddLib() and
       SetCompilerFlags() methods.
       @param arg0  The first argument to the toll call. Should contain the
                    tool name. Needed to find the executable path.
       @returns SILC_SUCCESS if the file was successfully parsed.
     */
    SILC_Error_Code
    ReadConfigFile( std::string arg0 );

    /* ********************************************** Public virtual methods */
public:
    /**
       virtual base class functin to parse command line attributes
       @param argc
       @param argv
       @return SILC_ERROR_CODE
     */
    virtual SILC_Error_Code
    ParseCmdLine( int    argc,
                  char** argv ) = 0;


    /**
       virtual base method  to perform the action.
     */
    virtual int
    Run() = 0;


    /**
       virtual base method to print out parameters.
     */
    virtual void
    PrintParameter() = 0;

    /* ******************************************* Protected virtual methods */
protected:
    /**
       This function is called from ReadConfigFile. It should set the
       compiler instrumentation flags.
       @param flags A string containing the instrumentation flags.
     */
    virtual void
    SetCompilerFlags( std::string flags ) = 0;

    /**
       This function is called from ReadConfigFile. It should add one include
       directory.
       @param dir One directory name.
     */
    virtual void
    AddIncDir( std::string dir ) = 0;

    /**
       This function is called from ReadConfigFile. It should add one library
       directory.
       @param dir One directory name.
     */
    virtual void
    AddLibDir( std::string dir ) = 0;

    /**
       This function is called from ReadConfigFile. It should add libraries.
       The string is of format -l<name>.
       @param dir One or more libraries.
     */
    virtual void
    AddLib( std::string lib ) = 0;

    /* **************************************** Protected implmented methods */
protected:
    /**
       Checks during command line parsing for common arguments.
       @param arg One argument which is checked.
       @returns true if the argument was a common argument, else it returns
                false.
     */
    bool
    CheckForCommonArg( std::string arg );

    /* ***************************************************** Private methods */
private:
    /**
       Tries to open a configuration file for instrumentation.
       @param inFile   Pointer to a stream which returns the open file.
       @param filename Name of a config file. If it is empty, the config file
                       will be searched in the current path. If it is set,
                       It tries to open the given file.
       @returns SILC_SUCCESS if the file was successfully opened.
     */
    SILC_Error_Code
    open_config_file( std::ifstream* inFile,
                      std::string    filename );

    /**
       Extracts parameter from configuration file
       It expects lines of the format key=value. Furthermore it truncates line
       at the scrpit comment character '#'.
       @param line    input line from the config file
       @returns SILC_SUCCESS if the line was successfully parsed. Else it
                returns an error code.
     */
    SILC_Error_Code
    read_parameter( std::string line );


    /* *************************************************** Protected members */
protected:
    /**
       Config file name. Can be set with the -config parameter.
       If it is empty, a config file is searched at standard locations.
     */
    std::string config_file;

    /**
       Verbosity level. The following levels are currently possible:
       0 = No output.
       1 = Executed commands are printed
       2 = All analysis results are printed.
     */
    int verbosity;
};

#endif /*SILC_APPLICATION_H_*/
