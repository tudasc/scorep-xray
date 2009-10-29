#include <iostream>
#include <string>
#include <fstream>
#include <istream>

extern "C" {
#include "SILC_Utils.h"
#include "SILC_Error.h"
}


#include "Silc_Instrumenter.hpp"

Silc_Instrumenter::Silc_Instrumenter
(
)
{
    printf( "calling the instrumentation phase \n" );

    /* call the configuration reader
     * this is hardcoded, since the file location and name should be fixed
     */
    silc_readConfigFile( "silc.conf" );
}


Silc_Instrumenter::~
Silc_Instrumenter
    ()
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
            char        line[ 256 ];
            inFile.getline( line, 256 );
            std::string linStr( line );
            int         found = linStr.find( "#" );
            if ( !( found != std::string::npos ) )
            {
                if ( silc_readParameter( linStr, parameters[ index ], value ) )
                {
                    // set member variables
                    switch ( index )
                    {
                        case 0:
                        {
                            _language = value;
                            break;
                        }
                        case 1:
                        {
                            _instType = value;
                            break;
                        }
                        case 2:
                        {
                            _compiler = value;
                            break;
                        }
                        case 3:
                        {
                            _compFlags = value;
                            break;
                        }
                        case 4:
                        {
                            _linkerFlags = value;
                            break;
                        }
                        case 5:
                        {
                            _libraries = value;
                            break;
                        }
                        case 6:
                        {
                            _inclDir = value;
                            break;
                        }
                        case 7:
                        {
                            _libDir = value;
                            break;
                        }
                        case 8:
                        {
                            _instDefault = value;
                            break;
                        }
                        case 9:
                        {
                            _instGnu = value;
                            break;
                        }
                        case 10:
                        {
                            _instPgi = value;
                            break;
                        }
                        case 11:
                        {
                            _instSun = value;
                            break;
                        }
                        case 12:
                        {
                            _instXl = value;
                            break;
                        }
                        case 13:
                        {
                            _instFtrace = value;
                            break;
                        }
                        case 14:
                        {
                            _instOpenuh = value;
                            break;
                        }
                    }

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
    /** get the instrumentation type
     *  examine compiler command including additional input path and linker option
     *  perform
     */

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
Silc_Instrumenter::silc_readParameter
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
        value  = instring.substr( posDelim + 1 );
        retVal = true;
        return retVal;
    }
    else
    {
        return retVal;
    }
}


void
Silc_Instrumenter::silc_printParameter
(
)
{
    std::cout << " language:                                  " << _language << "\n"
              << " instrumentation type:                      " << _instType << "\n"
              << " compiler:                                  " << _compiler  << "\n"
              << " compiler flags                             " << _compFlags << "\n"
              << " linker flags :                             " << _linkerFlags << "\n"
              << " list of libraries :                        " << _libraries << "\n"
              << " include directories :                      " << _inclDir  << "\n"
              << " lib directories                            " << _libDir   << "\n"
              << " default instrumentation:                   " << _instDefault << "\n"
              << " GNU compiler instrumantation parameter:    " << _instGnu << "\n"
              << " PGI compiler instrumantation parameter:    " << _instPgi << "\n"
              << " Sun compiler instrumantation parameter:    " << _instSun << "\n"
              << " Xl compiler instrumantation parameter:     " << _instXl << "\n"
              << " FTrace compiler instrumantation parameter: " << _instFtrace << "\n"
              << " openUH compiler instrumantation parameter: " << _instOpenuh << std::endl;
}
