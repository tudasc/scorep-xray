#include <iostream>
#include <string>
#include <fstream>
#include <istream>

#include "SILC_Utils.h"
#include "SILC_Error.h"

#include "Silc_Instrumenter.hpp"

Silc_Instrumenter::Silc_Instrumenter
(
)
    : instType( "comp:gnu" ),
      compCmdEnv( "" ),
      compFlagsEnv( "" ),
      compCmd( "" ),
      compArgs( "" ),
      compFlags( "" ),
      compLdFlags( "" ),
      compInstFlags( "" ),
      compLibs( "" )
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

    std::ifstream inFile;
    inFile.open( fileName.c_str(), std::ios_base::in );

    if ( inFile.is_open() )
    {
        /* look for string cut after '=' into string variable */
        const uint32_t    length               = 15;
        const std::string parameters[ length ] = {
            "language",
            "inst_type",
            "compiler",
            "compiler_flags",
            "linker_flags",
            "libs",
            "includedir",
            "libdir",
            "inst_default",
            "inst_avail",
            "inst_gnu_compiler_flags",
            "inst_pgi_compiler_flags",
            "inst_sun_compiler_flags",
            "inst_xl_compiler_flags",
            "inst_ftrace_compiler_flags"
        };
        std::string       value = "";
        uint32_t          index = 0;
        while ( inFile.good() && index < length )
        {
            char line[ 256 ];
            inFile.getline( line, 256 );

            std::string
            linStr
            (
                line
            );

            int found = linStr.find( "#" );
            if ( !( found != std::string::npos ) )
            {
                if ( readParameter( linStr, parameters[ index ], value ) )
                {
                    std::cout << "found value from " << parameters[ index ] << " :\t" << value << std::endl;
                    index++;
                }
            }
        }                  /* of while loop */
    }
    else
    {
        //		SILC_ERROR( SILC_ERROR_ENOENT, "test input file error");
    }
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
    printf( "used language:   \n " );

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


bool
Silc_Instrumenter::readParameter
(
    std::string &     instring,
    const std::string parameter,
    std::string &     value
)
{
    bool retVal   = false;
    int  findLang = instring.find( parameter );
    int  posDelim = instring.find( "=" );
    if ( findLang != std::string::npos && posDelim != std::string::npos )
    {
        value = instring.substr( posDelim + 1 );
        //	  std::cout << findLang<<"  "<< instring <<" value: "<< value <<std::endl;
        retVal = true;
        return retVal;
    }
    else
    {
        return retVal;
    }
}
