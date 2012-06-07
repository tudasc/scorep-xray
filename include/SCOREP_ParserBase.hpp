/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @ file      MANGLE_NAME( ParserBase ).hpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Class MANGLE_NAME( ParserBase )
 *         base class for SCOREP user interaction tools
 */


#ifndef SCOREP_APPLICATION_H_
#define SCOREP_APPLICATION_H_


#include <iostream>
#include <string>

#if defined( PACKAGE_SCOREP )
#include <SCOREP_Error_Codes.h>
#elif defined( PACKAGE_OTF2 )
#include <otf2/OTF2_Error_Codes.h>
#else
#error Unsupported package.
#endif


class MANGLE_NAME( ParserBase )
{
public:
    /* ****************************************** Implemented public methods */

    MANGLE_NAME( ParserBase ) ();

    virtual
    ~
    MANGLE_NAME( ParserBase ) ();

    /**
       Reads the configuration data from a file. To handle the read data
       you need to implement the AddLibDir() AddIncDir() AddLib() and
       SetCompilerFlags() methods.
       @param arg0  The first argument to the toll call. Should contain the
                    tool name. Needed to find the executable path.
       @returns MANGLE_NAME( SUCCESS ) if the file was successfully parsed.
     */
    MANGLE_NAME( Error_Code )
    ReadConfigFile( std::string arg0 );

    /* ******************************************* Protected methods for file parsing */
protected:
    /**
       This function is called from ReadConfigFile. It should add one include
       directory.
       @param dir One directory name.
     */
    virtual void
    AddIncDir( std::string dir );

    /**
       This function is called from ReadConfigFile. It should add one library
       directory.
       @param dir One directory name.
     */
    virtual void
    AddLibDir( std::string dir );

    /**
       This function is called from ReadConfigFile. It should add libraries.
       The string is of format -l<name>.
       @param dir One or more libraries.
     */
    virtual void
    AddLib( std::string lib );

    /**
       This function gives a (key, value) pair found in a configuration file and not
       processed by one of the former functions.
       @param key   The key
       @param value The value
     */
    virtual void
    SetValue( std::string key,
              std::string value ) = 0;

    /* ************************************* Protected methods for command line parsing */
public:
    /**
       Checks during command line parsing for common arguments.
       @param arg One argument which is checked.
       @returns true if the argument was a common argument, else it returns
                false.
     */
    bool
    CheckForCommonArg( std::string arg );

    /**
       Returns true if a config file was specified on the command line.
     */
    bool
    IsConfigFileSet();

    /* ***************************************************** Private methods */
private:
    /**
       Tries to open a configuration file for instrumentation.
       @param inFile   Pointer to a stream which returns the open file.
       @param filename Name of a config file. If it is empty, the config file
                       will be searched in the current path. If it is set,
                       It tries to open the given file.
       @returns MANGLE_NAME( SUCCESS ) if the file was successfully opened.
     */
    MANGLE_NAME( Error_Code )
    open_config_file( std::ifstream * inFile,
                      std::string filename );

    /**
       Extracts parameter from configuration file
       It expects lines of the format key=value. Furthermore it truncates line
       at the scrpit comment character '#'.
       @param line    input line from the config file
       @returns MANGLE_NAME( SUCCESS ) if the line was successfully parsed. Else it
                returns an error code.
     */
    MANGLE_NAME( Error_Code )
    read_parameter( std::string line );


    /* *************************************************** Protected members */
protected:
    /**
       Config file name. Can be set with the -config parameter.
       If it is empty, a config file is searched at standard locations.
     */
    std::string config_file;

    /**
       Used to specify a filename. Makes it possible to read different config
       files such as "otf2_config.dat", default is set to "scorep_config.dat"
     */
    std::string config_fname;

    /**
       Verbosity level. The following levels are currently possible:
       0 = No output.
       1 = Executed commands are printed
       2 = All analysis results are printed.
     */
    int verbosity;
};

#endif /*SCOREP_APPLICATION_H_*/
