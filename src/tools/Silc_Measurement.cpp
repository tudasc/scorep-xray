#include "Silc_Measurement.hpp"


Silc_Measurement::Silc_Measurement
    ()
{
    printf(
        "calling the measurement system by parsing the instrumented user application \n" );
}


SILC_Error_Code
Silc_Measurement::silc_readConfigFile
(
    std::string fileName
)
{
    bool exitStatus = true;
    printf( "reads the global configuration file: %s \n ", fileName.c_str() );

    return exitStatus;
}

void
Silc_Measurement::silc_printParameter
(
)
{
    printf( "print the measurement parameter: \n " );
}
