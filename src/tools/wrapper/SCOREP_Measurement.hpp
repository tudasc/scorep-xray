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


#ifndef SCOREP_MEASUREMENT_H_
#define SCOREP_MEASUREMENT_H_

/**
 * @status alpha
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @brief Class SCOREP_Measurement
 *
 *  This class examines system and user settings to run the measurement system.
 */

#include <iostream>
#include <string>

#include <scorep_utility/SCOREP_ParserBase.hpp>

/* ****************************************************************************
   Class SCOREP_Measurement
******************************************************************************/
/**
 *  @brief performes measurement stage
 *
 *  This class performs a measurement run. Currently, it only invokes the
 *  user command.
 */
class SCOREP_Measurement : public SCOREP_ParserBase
{
    /* ****************************************************** Public methods */
public:

    /**
       Creates a new SCOREP_Measurement object.
     */
    SCOREP_Measurement ();

    /**
       Destroys a SCOREP_Measurement object.
     */
    virtual ~
    SCOREP_Measurement ();

    /**
       Parses the command line.
       @param argc The number of arguments.
       @param argv List of arguments. It assumes, that the first argument is
                   the tool name and the second argument is the action.
       @return SCOREP_SUCCESS if the parsing was successful. Else an error
               code is returned.
     */
    virtual SCOREP_Error_Code
    ParseCmdLine( int    argc,
                  char** argv );

    /**
     * Perform measurement run.
     */
    virtual int
    Run();

    /**
     *  Prints all measurement parameters as read from input file
     */
    virtual void
    PrintParameter();

    /* *************************************************** Protected methods */
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
              std::string value );

    /* ***************************************************** Private members */
private:
    /**
     *  The user command.
     */
    std::string user_command;
    std::string scorep_library_path;
};
#endif /*SCOREP_MEASUREMENT_H_*/
