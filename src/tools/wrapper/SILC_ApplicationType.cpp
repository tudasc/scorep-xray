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
 * @status     alpha
 * @file       SILC_ApplicationType.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 */


#include "SILC_ApplicationType.hpp"
#include "SILC_Instrumenter.hpp"
#include "SILC_Measurement.hpp"

SILC_ApplicationType* SILC_ApplicationType::_appType = 0;


SILC_ApplicationType::SILC_ApplicationType
    ()
{
}


/** @brief
 *  defines the right stage of SILC defined by the user,
 *
 * @param  type    string which defines the type of application
 *
 * @return (..)    returns an instance of the SILC_Application type
 */

SILC_Application*
SILC_ApplicationType::getSilcStage
(
    std::string type
)
{
    if ( type == std::string( "Instrumenter" ) )
    {
        printf( "running the instrumentation phase: \n" );
        return new SILC_Instrumenter
               (
               );
    }
    else if ( type == std::string( "Measurement" ) )
    {
        printf( "running the measurement phase: \n" );
        return new
               SILC_Measurement
               (
               );
    }
    else
    {
        // some error handling
    }

    return 0;
}

/**
 * @brief get the application instance, ensures, that only one application is running
 *
 * @return _appType    returns the type of the SILC application
 */
SILC_ApplicationType
&
SILC_ApplicationType::getInstance
(
)
{
    if ( 0 == _appType )
    {
        _appType = new
                   SILC_ApplicationType
                   (
                   );
    }

    return *_appType;
}
