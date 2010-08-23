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
 *         runs the instrumented user program and steers the measurements system
 *
 *  This class examines system and user settings to run the measurement system
 */

#include <iostream>
#include <string>

#include "SILC_Application.hpp"

class SILC_Measurement : public SILC_Application
{
public:

    SILC_Measurement ();

    virtual ~
    SILC_Measurement ()
    {
        ;
    };

/** @brief
 * reads the defined input data file
 */
    virtual SILC_Error_Code
    ReadConfigFile( std::string fileName );


/** @brief
 * get command line attributes
 */
    virtual SILC_Error_Code
    ParseCmdLine( int    argc,
                  char** argv );

/** @brief
 * perform instrumentation stage
 */
    virtual int
    Run();



/** @brief
 *  prints all measurement parameters as read from input file
 */
    virtual void
    PrintParameter();


private:

/** @brief
 *  used user command
 */
    std::string _userCommand;
};
#endif /*SILC_MEASUREMENT_H_*/
