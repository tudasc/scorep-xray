#include <Silc_Instrumenter.h>

Silc_Instrumenter::Silc_Instrumenter()
{
    printf( "calling the instrumentation phase \n" );
}


Silc_Instrumenter::~Silc_Instrumenter()
{
}

virtual bool
Silc_Instrumenter::readConfigFile
(
)
{
    bool exitStatus = true;
    printf( "reads the global configuration file: \n " );

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
);

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
    bool exitStatus = true;
    printf( "compiler command: %s  \n ", cmd );

    return exitStatus;
}

void
Silc_Instrumenter::silc_compilerArg
(
    const std::string arg
)
{
    bool exitStatus = true;
    printf( "compiler arguments: %s  \n ", arg );

    return exitStatus;
}

void
Silc_Instrumenter::silc_compilerLib
(
    const std::string lib
)
{
    bool exitStatus = true;
    printf( "used libraries: %s  \n ", lib );

    return exitStatus;
}
