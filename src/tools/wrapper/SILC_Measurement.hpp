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


#ifndef SILC_MEASUREMENT_H_
#define SILC_MEASUREMENT_H_

/**
 * @status alpha
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @brief Class SILC_Measurement
 *
 *  This class examines system and user settings to run the measurement system.
 */

#include <iostream>
#include <string>

#include <silc_utility/SILC_ParserBase.hpp>

/* ****************************************************************************
   Class SILC_Measurement
******************************************************************************/
/**
 *  @brief performes measurement stage
 *
 *  This class performs a measurement run. Currently, it only invokes the
 *  user command.
 */
class SILC_Measurement : public SILC_ParserBase
{
    /* ****************************************************** Public methods */
public:

    /**
       Creates a new SILC_Measurement object.
     */
    SILC_Measurement ();

    /**
       Destroys a SILC_Measurement object.
     */
    virtual ~
    SILC_Measurement ();

    /**
       Parses the command line.
       @param argc The number of arguments.
       @param argv List of arguments. It assumes, that the first argument is
                   the tool name and the second argument is the action.
       @return SILC_SUCCESS if the parsing was successful. Else an error
               code is returned.
     */
    virtual SILC_Error_Code
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
       This function is called from ReadConfigFile. It should set the
       compiler instrumentation flags.
       @param flags A string containing the instrumentation flags.
     */
    virtual void
    SetCompilerFlags( std::string flags );

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

    /* ***************************************************** Private members */
private:
    /**
     *  The user command.
     */
    std::string user_command;
    std::string silc_library_path;
};
#endif /*SILC_MEASUREMENT_H_*/
