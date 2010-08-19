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
 * @ file      SILC_ApplicationType.cpp
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 */


#include "SILC_ApplicationType.hpp"
#include "SILC_Instrumenter.hpp"
#include "SILC_Measurement.hpp"

Silc_ApplicationType* Silc_ApplicationType::_appType = 0;


Silc_ApplicationType::Silc_ApplicationType
    ()
{
}


/** @brief
 *  defines the right stage of SILC defined by the user,
 *
 * @param  type    string which defines the type of application
 *
 * @return (..)    returns an instance of the Silc_Application type
 */

Silc_Application*
Silc_ApplicationType::getSilcStage
(
    std::string type
)
{
    if ( type == std::string( "Instrumenter" ) )
    {
        printf( "running the instrumentation phase: \n" );
        return new Silc_Instrumenter
               (
               );
    }
    else if ( type == std::string( "Measurement" ) )
    {
        printf( "running the measurement phase: \n" );
        return new
               Silc_Measurement
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
Silc_ApplicationType
&
Silc_ApplicationType::getInstance
(
)
{
    if ( 0 == _appType )
    {
        _appType = new
                   Silc_ApplicationType
                   (
                   );
    }

    return *_appType;
}
