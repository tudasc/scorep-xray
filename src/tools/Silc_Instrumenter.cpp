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


/** @file Silc_Instrumenter.cpp
 * @brief
 *
 */

#include "Silc_Instrumenter.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <istream>

extern "C" {
#include <SILC_Utils.h>
#include <SILC_Error.h>
}



/**
 * @brief constructor: calls the member function to initialize privates via config file
 */
Silc_Instrumenter::Silc_Instrumenter
(
)
{
    /* call the configuration reader
     * this is hardcoded, since the file location and name should be fixed during configure stage
     */
    if (    silc_readConfigFile( "../src/tools/silc.conf" ) == SILC_SUCCESS )
    {
        std::cout << "calling the instrumentation phase \n";
    }
    else
    {
        SILC_ERROR( SILC_ERROR_ENOENT, "" );
    }
}



/**
 *  @brief default destructor
 */
Silc_Instrumenter::~
Silc_Instrumenter
    ()
{
}

/**
 * @brief reads the configuration file for the instrumentation stage
 * @param  fileName   name of the configuration file
 *
 * The configuration file should exist in a fixed location to be found by the
 * instrumentation tool. Idealy it is installed in a global path and examined
 * during configure phase.
 */
SILC_Error_Code
Silc_Instrumenter::silc_readConfigFile
(
    std::string fileName
)
{
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

        if ( !( inFile.good() ) )
        {
            return SILC_ERROR_ENOENT;
        }

        while ( inFile.good() && index < length )
        {
            char        line[ 256 ];
            inFile.getline( line, 256 );
            std::string linStr( line );
            int         found = linStr.find( "#" );
            if ( !( found != std::string::npos ) )
            {
                if ( silc_readParameter( linStr, parameters[ index ], value ) == SILC_SUCCESS )
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
                            _instString = value;
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
                            _instAvail = value;
                            break;
                        }
                        case 10:
                        {
                            _instGnu = value;
                            break;
                        }
                        case 11:
                        {
                            _instPgi = value;
                            break;
                        }
                        case 12:
                        {
                            _instSun = value;
                            break;
                        }
                        case 13:
                        {
                            _instXl = value;
                            break;
                        }
                        case 14:
                        {
                            _instFtrace = value;
                            break;
                        }
                        case 15:
                        {
                            _instOpenuh = value;
                            break;
                        }
                    }
                } /* of silc_readParameter */
                index++;
            }                              /* of while loop */
            silc_instType( _instString );
        }
        return SILC_SUCCESS;
    }
    else
    {
        return SILC_ERROR_ENOENT;
    }
}



SILC_Error_Code
Silc_Instrumenter::silc_readEnvVars
(
)
{
    printf( "reads the enviroment variables  \n " );

    return SILC_SUCCESS;
}


/**
 * @brief parses command line to the instrumenter
 *
 *
 */
SILC_Error_Code
Silc_Instrumenter::silc_parseCmdLine
(
    int    argc,
    char** argv
)
{
    SILC_Error_Code exitStatus =  SILC_ERROR_ENOTSUP;
    std::string     instStr( argv[ 1 ] );
    int             pos = instStr.find( "--instrument", 0 );
    if ( pos != std::string::npos
         //		 && _instType == INST_TYPE_GNU
         && argc > 3
         )
    {
        for ( int loop = 3; loop < argc; loop++ )
        {
            std::string flag( argv[ loop ] );
            _compFlags += flag + " ";
            std::cout << flag << "   " << argv[ loop ] << std::endl;
        }
        exitStatus = SILC_SUCCESS;
    }


    return exitStatus;
}

/**
 * @brief runs the user specified instrumentation command
 *
 * Depending on the instrumentation type the compiler command gets defined and the user
 * code gets compiled. The return value of the function delivers the error code of the
 * system call of the compiler command.
 */
int
Silc_Instrumenter::silc_run
(
)
{
    int32_t exitCode = -1;
    printf( "instrument the user code \n " );


    std::string compCommand = _compiler + " ";

    if ( _instType == INST_TYPE_GNU )
    {
        compCommand += _instGnu;
    }
    else if ( _instType == INST_TYPE_PGI )
    {
        compCommand += _instPgi;
    }
    else
    {
        compCommand += _instDefault;
    }


    compCommand += " " + _compFlags;

    std::cout << "  silc_run: " << compCommand << std::endl;

    exitCode = system( compCommand.c_str() );

    return exitCode;
}


/**
 * @brief
 */
SILC_Error_Code
Silc_Instrumenter::silc_setLanguage
(
    const int lang
)
{
    printf( "used language:   \n " );
    return SILC_SUCCESS;
}


/**
 * @brief
 */
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


SILC_Error_Code
Silc_Instrumenter::silc_readParameter
(
    std::string &     instring,
    const std::string parameter,
    std::string &     value
)
{
    value = "";
    int findLang = instring.find( parameter );
    int posDelim = instring.find( "=" );
    if ( findLang != std::string::npos && posDelim != std::string::npos )
    {
        value = instring.substr( posDelim + 1 );
        std::cout << "    found user given value:    " << value << std::endl;
        return SILC_SUCCESS;
    }
    else
    {
        return SILC_ERROR_INVALID;
    }
}


void
Silc_Instrumenter::silc_instType
(
    std::string itype
)
{
    const uint32_t    length            = 12;
    const std::string instKey[ length ] = {
        "comp:gnu",
        "comp:pgi",
        "comp:sun",
        "comp:xl",
        "comp:ftrace",
        "comp:openuh",
        "bin:dyninst",
        "manual:user",
        "manual:seq",
        "manual:mpi",
        "manual:openmp",
        "manual:hybrid"
    };


    uint32_t index = 0;
    for ( index = 0; index < length; index++ )
    {
        switch ( index )
        {
            case  0:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_GNU;
                }
                break;
            }
            case  1:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_PGI;
                }
                break;
            }
            case  2:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_SUN;
                }
                break;
            }
            case  3:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_XL;
                }
                break;
            }
            case  4:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_FTRACE;
                }
                break;
            }
            case  5:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_OPENUH;
                }
                break;
            }
            case  6:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_DYNINST;
                }
                break;
            }
            case  7:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_USER;
                }
                break;
            }
            case  8:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_SEQ;
                }
                break;
            }
            case  9:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_MPI;
                }
                break;
            }
            case  10:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_OPENMP;
                }
                break;
            }
            case  11:
            {
                if ( itype == instKey[ index ] )
                {
                    _instType = INST_TYPE_HYBRID;
                }
                break;
            }
        }
    }
    std::cout << " selected instrumentation type: " << itype << "  enum: " << _instType << std::endl;
}




void
Silc_Instrumenter::silc_printParameter
(
)
{
    std::cout << " language:                                  " << _language << "\n"
              << " instrumentation type:                      " << _instString << "\n"
              << " compiler:                                  " << _compiler  << "\n"
              << " compiler flags                             " << _compFlags << "\n"
              << " linker flags :                             " << _linkerFlags << "\n"
              << " list of libraries :                        " << _libraries << "\n"
              << " include directories :                      " << _inclDir  << "\n"
              << " lib directories                            " << _libDir   << "\n"
              << " default instrumentation:                   " << _instDefault << "\n"
              << " GNU compiler instrumentation parameter:    " << _instGnu << "\n"
              << " PGI compiler instrumentation parameter:    " << _instPgi << "\n"
              << " Sun compiler instrumentation parameter:    " << _instSun << "\n"
              << " Xl compiler instrumentation parameter:     " << _instXl << "\n"
              << " FTrace compiler instrumentation parameter: " << _instFtrace << "\n"
              << " openUH compiler instrumentation parameter: " << _instOpenuh << std::endl;
}
