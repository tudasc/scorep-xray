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
 * @status alpha
 *
 * @maintainer Johannes Spazier <johannes.spazier@tu-dresden.de>
 * @autors     Johannes Spazier <johannes.spazier@tu-dresden.de>
 *
 */

#include <config.h>

#include <string.h>

#include "silc_config.hpp"

#define MODE_SEQ 0
#define MODE_OMP 1
#define MODE_MPI 2
#define MODE_HYB 3

#define ACTION_LIBS 1
#define ACTION_CFLAGS 2
#define ACTION_CC 3
#define ACTION_CXX 4

#define HELPTEXT "\nUsage: silc_config [--seq|--omp|--mpi|--hyb] (--cflags|--libs|--cc|--cxx)\n"

int
main( int    argc,
      char** argv )
{
    int         i;
    /* set default mode to mpi */
    int         mode   = MODE_MPI;
    int         action = 0;
    int         ret;

    const char* silc_libs[ 4 ] = { "silc_serial", "silc_omp", "silc_mpi", "silc_mpi_omp" };

    SILC_Config app;

    /* read config file */
    app.ParseConfigFile( argv[ 0 ] );

    /* parsing the command line */
    for ( i = 1; i < argc; i++ )
    {
        if ( strcmp( argv[ i ], "--help" ) == 0 || strcmp( argv[ i ], "-h" ) == 0 )
        {
            std::cout << HELPTEXT << std::endl;
            return EXIT_SUCCESS;
        }
        else if ( strcmp( argv[ i ], "--seq" ) == 0 )
        {
            mode = MODE_SEQ;
        }
        else if ( strcmp( argv[ i ], "--omp" ) == 0 )
        {
            mode = MODE_OMP;
        }
        else if ( strcmp( argv[ i ], "--mpi" ) == 0 )
        {
            mode = MODE_MPI;
        }
        else if ( strcmp( argv[ i ], "--hyb" ) == 0 )
        {
            mode = MODE_HYB;
        }
        else if ( strcmp( argv[ i ], "--libs" ) == 0 )
        {
            action = ACTION_LIBS;
        }
        else if ( strcmp( argv[ i ], "--cflags" ) == 0 )
        {
            action = ACTION_CFLAGS;
        }
        else if ( strcmp( argv[ i ], "--cc" ) == 0 )
        {
            action = ACTION_CC;
        }
        else if ( strcmp( argv[ i ], "--cxx" ) == 0 )
        {
            action = ACTION_CXX;
        }
        else
        {
            std::cerr << "\nUnknown option " << argv[ i ] << ". Abort.\n" << std::endl;
            return EXIT_FAILURE;
        }
    }

    switch ( action )
    {
        case ACTION_LIBS:
            std::cout << app.str_libdir << " -l" << silc_libs[ mode ] << app.str_libs;
            std::cout.flush();

            ret = system( "otf2_config --libs" );

            break;

        case ACTION_CFLAGS:
            std::cout << app.str_flags << app.str_incdir;
            std::cout.flush();

            ret = system( "otf2_config --cflags" );

            break;

        case ACTION_CC:
            std::cout << app.str_cc << std::endl;

            break;

        case ACTION_CXX:
            std::cout << CXX << std::endl;

            break;

        default:
            std::cout << HELPTEXT << std::endl;

            break;
    }

    return 0;
}


/** constructor and destructor */
SILC_Config::SILC_Config()
{
    this->config_file = "";
}

SILC_Config::~SILC_Config()
{
}


/** */
int
SILC_Config::ParseConfigFile( char* arg0 )
{
    std::string arg = std::string( arg0 );

    this->ReadConfigFile( arg );
}


/** these methods must be overwritten, but they are not needed in class SILC_Config */
SILC_Error_Code
SILC_Config::ParseCmdLine( int    argc,
                           char** argv )
{
}

int
SILC_Config::Run()
{
}

void
SILC_Config::PrintParameter()
{
}


/** callbacks */
void
SILC_Config::SetCompilerFlags( std::string flags )
{
    this->str_flags += flags + " ";
}

void
SILC_Config::AddIncDir( std::string dir )
{
    this->str_incdir += " -I" + dir;
}

void
SILC_Config::AddLibDir( std::string dir )
{
    this->str_libdir += " -L" + dir;
}

void
SILC_Config::AddLib( std::string lib )
{
    this->str_libs += " " + lib;
}

void
SILC_Config::SetCompiler( std::string value )
{
    this->str_cc = value;
}
