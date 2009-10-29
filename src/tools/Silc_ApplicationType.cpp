#include "Silc_ApplicationType.hpp"
#include "Silc_Instrumenter.hpp"
#include "Silc_Measurement.hpp"

Silc_ApplicationType* Silc_ApplicationType::_appType = 0;

Silc_ApplicationType::Silc_ApplicationType
    ()
{
}

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

/** @brief
 * get the instance
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
