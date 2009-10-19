#include "Silc_Measurement.h"


Silc_Measurement::Silc_Measurement()
{
    printf(
        "calling the measurement system by parsing the instrumented user application \n" );
}


virtual bool Silc_Measurement::readConfigFile()
{
    bool exitStatus = true;
    printf( "reads the global configuration file: \n " );

    return exitStatus;
}
