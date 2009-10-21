#include "Silc_Instrumenter.hpp"

Silc_Instrumenter::Silc_Instrumenter()
{
    printf( "calling the instrumentation phase \n" );
}


Silc_Instrumenter::~Silc_Instrumenter()
{
}

bool
Silc_Instrumenter::silc_readConfigFile
(
    std::string fileName
)
{
    bool exitStatus = true;
    std::cout << "reads the global configuration file: " << fileName << " \n " << std::endl;

    return exitStatus;
}



bool
Silc_Instrumenter::silc_readEnvVars
(
)
{
    bool exitStatus = true;
    printf( "reads the enviroment variables  \n " );

    return exitStatus;
}

bool
Silc_Instrumenter::silc_parseCmdLine
(
    int    argc,
    char** argv
)
{
    bool exitStatus = true;
    printf( "examines the command line arguments \n " );

    return exitStatus;
}

int
Silc_Instrumenter::silc_run
(
)
{
    bool exitStatus = true;
    printf( "compiles the instrumented user code \n " );

    return exitStatus;
}


bool
Silc_Instrumenter::silc_setLanguage
(
    const int lang
)
{
    bool exitStatus = true;
    printf( "sets used language:   \n " );

    return exitStatus;
}


void
Silc_Instrumenter::silc_compilerCmd
(
    const std::string cmd
)
{
    printf( "compiler command: %s  \n ", cmd.c_str() );
}

void
Silc_Instrumenter::silc_compilerArg
(
    const std::string arg
)
{
    printf( "compiler arguments: %s  \n ", arg.c_str() );
}

void
Silc_Instrumenter::silc_compilerLib
(
    const std::string lib
)
{
    printf( "used libraries: %s  \n ", lib.c_str() );
}
