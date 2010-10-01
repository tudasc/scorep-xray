/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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

#include <scorep_config_tool_backend.h>
#include <scorep_config.hpp>

#define MODE_SEQ 0
#define MODE_OMP 1
#define MODE_MPI 2
#define MODE_HYB 3

#define ACTION_LIBS   1
#define ACTION_CFLAGS 2
#define ACTION_CC     3
#define ACTION_CXX    4
#define ACTION_FC     5

#define HELPTEXT "\nUsage: scorep_config [--seq|--omp|--mpi|--hyb] (--cflags|--libs|--cc|--cxx | --fc) [--config <config_file>]\n"

int
main( int    argc,
      char** argv )
{
    int           i;
    /* set default mode to mpi */
    int           mode   = MODE_MPI;
    int           action = 0;
    int           ret;
    char*         config_file = NULL;

    const char*   scorep_libs[ 4 ] = { "scorep_serial", "scorep_omp", "scorep_mpi", "scorep_mpi_omp" };

    SCOREP_Config app;

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
        else if ( strcmp( argv[ i ], "--fc" ) == 0 )
        {
            action = ACTION_FC;
        }
        else if ( strcmp( argv[ i ], "--config" ) == 0 )
        {
            // Expect the config file name as next parameter
            if ( argc < i + 2 || *argv[ i + 1 ] == '-' )
            {
                std::cerr << "\nConfig file name missing. Abort.\n" << std::endl;
                return EXIT_FAILURE;
            }
            config_file = argv[ i + 1 ];

            // The next parameter is already processed now.
            i++;
        }
        else
        {
            std::cerr << "\nUnknown option " << argv[ i ] << ". Abort.\n" << std::endl;
            return EXIT_FAILURE;
        }
    }

    char*       path        = SCOREP_GetExecutablePath( argv[ 0 ] );
    std::string otf2_config = "otf2_config";
    if ( path != NULL )
    {
        otf2_config = "/" + otf2_config;
        otf2_config = path + otf2_config;
    }
    free( path );

    /* print data in case a config file was specified */
    if ( config_file != NULL )
    {
        if ( app.ParseConfigFile( config_file ) != SCOREP_SUCCESS )
        {
            std::cerr << "Unable to open file '" << config_file  << std::endl;
            abort();
        }
        switch ( action )
        {
            case ACTION_LIBS:
                std::cout << app.str_libdir << " -l" << scorep_libs[ mode ] << app.str_libs;
                std::cout.flush();

                otf2_config += " --libs";
                ret          = system( otf2_config.c_str() );

                break;

            case ACTION_CFLAGS:
                std::cout << app.str_flags << app.str_incdir;
                std::cout.flush();

                otf2_config += " --cflags";
                ret          = system( otf2_config.c_str() );

                break;

            case ACTION_CC:
                std::cout << app.str_cc << std::endl;

                break;

            case ACTION_CXX:
                std::cout << app.str_cxx << std::endl;

                break;

            case ACTION_FC:
                std::cout << app.str_fc << std::endl;

                break;

            default:
                std::cout << HELPTEXT << std::endl;

                break;
        }
    }
    else
    {
        switch ( action )
        {
            case ACTION_LIBS:
                std::cout << "-L" SCOREP_LIBDIR;
                if ( CUBE_LIBDIR != "" )
                {
                    std::cout << " -L" CUBE_LIBDIR;
                }

                if ( TIMER_LIBDIR != "" )
                {
                    std::cout << " -L" TIMER_LIBDIR;
                }

                std::cout << " -l" << scorep_libs[ mode ] << " " SCOREP_LIBS " ";
                std::cout.flush();

                otf2_config += " --libs";
                ret          = system( otf2_config.c_str() );

                break;

            case ACTION_CFLAGS:
                std::cout << SCOREP_CFLAGS " -I" SCOREP_PREFIX "/include ";
                std::cout.flush();

                otf2_config += " --cflags";
                ret          = system( otf2_config.c_str() );

                break;

            case ACTION_CC:
                std::cout << CC << std::endl;

                break;

            case ACTION_CXX:
                std::cout << CXX << std::endl;

                break;

            case ACTION_FC:
                std::cout << FC << std::endl;

                break;

            default:
                std::cout << HELPTEXT << std::endl;

                break;
        }
    }

    return 0;
}


/** constructor and destructor */
SCOREP_Config::SCOREP_Config()
{
    this->config_file = "";
}

SCOREP_Config::~SCOREP_Config()
{
}


/** */
SCOREP_Error_Code
SCOREP_Config::ParseConfigFile( char* arg0 )
{
    std::string arg = std::string( arg0 );
    return this->ReadConfigFile( arg );
}


/** these methods must be overwritten, but they are not needed in class SCOREP_Config */
SCOREP_Error_Code
SCOREP_Config::ParseCmdLine( int    argc,
                             char** argv )
{
}

int
SCOREP_Config::Run()
{
}

void
SCOREP_Config::PrintParameter()
{
}


/** callbacks */
void
SCOREP_Config::SetCompilerFlags( std::string flags )
{
    if ( std::string::npos == this->str_flags.find( flags ) )
    {
        this->str_flags += flags + " ";
    }
}

void
SCOREP_Config::AddIncDir( std::string dir )
{
    std::string incdir = "-I" + dir;

    if ( std::string::npos == this->str_incdir.find( incdir ) )
    {
        this->str_incdir += " " + incdir;
    }
}

void
SCOREP_Config::AddLibDir( std::string dir )
{
    std::string libdir = "-L" + dir;

    if ( std::string::npos == this->str_libdir.find( libdir ) )
    {
        this->str_libdir += " " + libdir;
    }
}

void
SCOREP_Config::AddLib( std::string lib )
{
    if ( std::string::npos == this->str_libs.find( lib ) )
    {
        this->str_libs += " " + lib;
    }
}

void
SCOREP_Config::SetCompiler( std::string value )
{
    this->str_cc = value;
}

void
SCOREP_Config::SetCxx( std::string value )
{
    this->str_cxx = value;
}

void
SCOREP_Config::SetFc( std::string value )
{
    this->str_fc = value;
}

void
SCOREP_Config::SetPrefix( std::string value )
{
    std::string libdir = "-L" + value + "/lib";
    std::string incdir = "-I" + value + "/include/scorep";

    /* only insert libdir if the path not already exists */
    if ( std::string::npos == this->str_libdir.find( libdir ) )
    {
        this->str_libdir += " " + libdir;
    }

    if ( std::string::npos == this->str_incdir.find( incdir ) )
    {
        this->str_incdir += " " + incdir;
    }
}
