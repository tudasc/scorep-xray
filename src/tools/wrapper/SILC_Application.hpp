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
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @brief Class Silc_Application
 *         base class for SILC user interaction tools
 */


#ifndef SILC_APPLICATION_H_
#define SILC_APPLICATION_H_


#include <iostream>
#include <string>

#include <SILC_Utils.h>


class Silc_Application
{
public:

    Silc_Application
        ();
    virtual ~
    Silc_Application
        ();


    virtual SILC_Error_Code
    silc_readConfigFile
    (
        std::string fileName
    ) = 0;


/**
 * @brief virtual base class functin to parse command line attributes
 *
 * @param argc
 * @param argv
 *
 * @return SILC_ERROR_CODE
 */
    virtual SILC_Error_Code
    silc_parseCmdLine
    (
        int    argc,
        char** argv
    ) = 0;


/**
 * @brief virtual base class to perform instrumentation stage
 */
    virtual int
    silc_run
    (
    ) = 0;


/**
 * @brief virtual base class to print out parameters
 */
    virtual void
    silc_printParameter
    (
    ) = 0;

private:
};

#endif /*SILC_APPLICATION_H_*/
