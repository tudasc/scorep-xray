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


#ifndef SILC_APPLICATION_H_
#define SILC_APPLICATION_H_

/*! @brief Class Silc_Application
 *         base class for SILC user interaction tools
 *
 */

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
    )
    {
        return SILC_SUCCESS;
    };

/** @brief
 * get command line attributes
 */
    virtual SILC_Error_Code
    silc_parseCmdLine
    (
        int    argc,
        char** argv
    )
    {
        return SILC_SUCCESS;
    };


/** @brief
 * perform instrumentation stage
 */
    virtual int
    silc_run
    (
    )
    {
        return 0;
    };


    virtual void
    silc_printParameter
    (
    )
    {
    };


private:
};

#endif /*SILC_APPLICATION_H_*/
