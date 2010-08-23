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


/*
 *  @file      SILC_Application.hpp
 *  @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 *  @brief Class SILC_ApplicationType
 *         Selector class of the user instrumentation stage
 *
 *  This class provides a mechanism to switch between the different stages of SILC tools.
 */

#ifndef SILC_APPLICATIONTYPE_H_
#define SILC_APPLICATIONTYPE_H_

#include <iostream>
#include <string>

#include "SILC_Application.hpp"


class SILC_ApplicationType
{
public:

    ~
    SILC_ApplicationType
        ()
    {
    }

    static SILC_ApplicationType&
    getInstance
    (
    );

    SILC_Application*
    getSilcStage
    (
        std::string type
    );


private:
/** @brief
 *  protect the constructor to ensure only one instance at a time
 */
    SILC_ApplicationType
        ();

/** @brief
 *  do not allow the copy constructor to be called unwillingly
 */
    SILC_ApplicationType
    (
        const SILC_ApplicationType &type
    );

/** @brief
 *  returns the pointer to a new SILC application (instrumenter, measurement, (visualizer)? )
 */
    static SILC_ApplicationType* _appType;
};


#endif /*SILC_APPLICATIONTYPE_H_*/
