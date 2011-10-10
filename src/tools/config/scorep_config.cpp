/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @status alpha
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @autors     Johannes Spazier <johannes.spazier@tu-dresden.de>
 * @autors     Daniel Lorenz <d.lorenz@fz-juelich.de>
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
#define ACTION_INCDIR 3
#define ACTION_CC     4
#define ACTION_CXX    5
#define ACTION_FC     6

#define HELPTEXT \
    "\nUsage:\nscorep-config <command> [<options>]\n" \
    "  Commands:\n" \
    "   --cflags prints additional compiler flags. They already contain the\n" \
    "            include flags.\n" \
    "   --inc    prints the include flags. They are already contained in the\n" \
    "            output of the --cflags command\n" \
    "   --libs   prints the required linker flags\n" \
    "   --cc     prints the C compiler name\n" \
    "   --cxx    prints the C++ compiler name\n" \
    "   --fc     prints the Fortran compiler name\n" \
    "   --help   prints this usage information\n\n" \
    "  Options:\n" \
    "   --seq|--omp|--mpi|--hyb\n" \
    "            specifys the mode: seqential, OpenMP, MPI, or hybrid (MPI + OpenMP)\n" \
    "            Takes effect only for the --libs command. The default mode is MPI.\n\n" \
    "   --config=<config_file>\n" \
    "            Allows to specify a configuration file which overrides the data\n" \
    "            specified at build time of the tool.\n\n" \
    "   --user|--nouser\n" \
    "            Specifies whether manual user instrumentation is used. Takes effect\n" \
    "            only for the cflags. The default ist that user instrumentation is\n" \
    "            disabled\n\n" \
    "   --compiler|--nocompiler\n" \
    "            Specifies whether compiler instrumentation is used. Takes effect\n" \
    "            only for the cflags. The default is that compiler instrumentation in\n" \
    "            enabled.\n\n" \
    "   --fortran   Specifies that the required flags are for the Fortran compiler.\n\n"

int
main( int    argc,
      char** argv )
{
    int           i;
    /* set default mode to mpi */
    int           mode     = MODE_MPI;
    int           action   = 0;
    int           ret      = EXIT_SUCCESS;
    bool          user     = false;
    bool          compiler = true;
    bool          fortran  = false;

    const char*   scorep_libs[ 4 ] = { "scorep_serial", "scorep_omp", "scorep_mpi", "scorep_mpi_omp" };

    SCOREP_Config app( argv[ 0 ] );

    /* parsing the command line */
    for ( i = 1; i < argc; i++ )
    {
        if ( app.CheckForCommonArg( argv[ i ] ) )
        {
            continue;
        }
        else if ( strcmp( argv[ i ], "--help" ) == 0 || strcmp( argv[ i ], "-h" ) == 0 )
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
        else if ( strcmp( argv[ i ], "--inc" ) == 0 )
        {
            action = ACTION_INCDIR;
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
        else if ( strcmp( argv[ i ], "--user" ) == 0 )
        {
            user = true;
        }
        else if ( strcmp( argv[ i ], "--nouser" ) == 0 )
        {
            user = false;
        }
        else if ( strcmp( argv[ i ], "--compiler" ) == 0 )
        {
            compiler = true;
        }
        else if ( strcmp( argv[ i ], "--nocompiler" ) == 0 )
        {
            compiler = false;
        }
        else if ( strcmp( argv[ i ], "--fortran" ) == 0 )
        {
            fortran = true;
        }

        else
        {
            std::cerr << "\nUnknown option " << argv[ i ] << ". Abort.\n" << std::endl;
            return EXIT_FAILURE;
        }
    }

    /* print data in case a config file was specified */
    if ( app.IsConfigFileSet() )
    {
        if ( app.ParseConfigFile( argv[ 0 ] ) != SCOREP_SUCCESS )
        {
            std::cerr << "Unable to open config file." << std::endl;
            abort();
        }
        switch ( action )
        {
            case ACTION_LIBS:
                std::cout << app.str_libdir << " -l" << scorep_libs[ mode ] << app.str_libs;
                std::cout.flush();
                app.str_otf2_config += " --libs";
                ret                  = system( app.str_otf2_config.c_str() );
                break;

            case ACTION_CFLAGS:
                if ( compiler )
                {
                    std::cout << app.str_flags;
                }
                if ( user )
                {
                    if ( fortran )
                    {
                       #ifdef SCOREP_COMPILER_IBM
                        std::cout << " -WF,-DSCOREP_USER_ENABLE ";
                       #else
                        std::cout << " -DSCOREP_USER_ENABLE ";
                       #endif // SCOREP_COMPILER_IBM
                    }
                    else
                    {
                        std::cout << " -DSCOREP_USER_ENABLE ";
                    }
                }
            // Append the include directories, too

            case ACTION_INCDIR:
                std::cout << app.str_incdir;
                std::cout.flush();
                //app.str_otf2_config += " --cflags";
                //ret                  = system( app.str_otf2_config.c_str() );
                break;

            case ACTION_CC:
                std::cout << app.str_cc;
                std::cout.flush();
                break;

            case ACTION_CXX:
                std::cout << app.str_cxx;
                std::cout.flush();
                break;

            case ACTION_FC:
                std::cout << app.str_fc;
                std::cout.flush();
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
                std::cout << "-L" SCOREP_LIBDIR << " -Wl,-rpath," SCOREP_LIBDIR;
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

                app.str_otf2_config += " --libs";
                ret                  = system( app.str_otf2_config.c_str() );
                break;

            case ACTION_CFLAGS:
                if ( compiler )
                {
                    std::cout << SCOREP_CFLAGS;
                }
                if ( user )
                {
                    if ( fortran )
                    {
                       #ifdef SCOREP_COMPILER_IBM
                        std::cout << " -WF,-DSCOREP_USER_ENABLE ";
                       #else
                        std::cout << " -DSCOREP_USER_ENABLE ";
                       #endif // SCOREP_COMPILER_IBM
                    }
                    else
                    {
                        std::cout << " -DSCOREP_USER_ENABLE ";
                    }
                }
            // Append the include directories, too

            case ACTION_INCDIR:
                std::cout << "-I" SCOREP_PREFIX "/include -I"
                SCOREP_PREFIX "/include/scorep ";
                std::cout.flush();
                //app.str_otf2_config += " --cflags";
                //ret                  = system( app.str_otf2_config.c_str() );
                break;

            case ACTION_CC:
                std::cout << SCOREP_CC;
                std::cout.flush();
                break;

            case ACTION_CXX:
                std::cout << SCOREP_CXX;
                std::cout.flush();
                break;

            case ACTION_FC:
                std::cout << SCOREP_FC;
                std::cout.flush();
                break;

            default:
                std::cout << HELPTEXT << std::endl;
                break;
        }
    }

    return ret;
}


/** constructor and destructor */
SCOREP_Config::SCOREP_Config( char* arg0 )
{
    char* path = SCOREP_GetExecutablePath( arg0 );
    str_otf2_config = "otf2-config";
    if ( path != NULL )
    {
        str_otf2_config = "/" + str_otf2_config;
        str_otf2_config = path + str_otf2_config;
    }
    free( path );
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

/** callbacks */
void
SCOREP_Config::SetValue( std::string key,
                         std::string value )
{
    if ( key == "COMPILER_INSTRUMENTATION_CPPFLAGS" )
    {
        this->str_flags += value + " ";
    }
    else if ( key == "CC" && value != "" )
    {
        this->str_cc = value;
    }
    else if ( key == "CXX" && value != "" )
    {
        this->str_cxx = value;
    }
    else if ( key == "FC" && value != "" )
    {
        this->str_fc = value;
    }
    else if ( key == "PREFIX" && value != "" )
    {
        AddIncDir( value + "/include/scorep" );
        AddLibDir( value + "/lib" );
    }
    else if ( key == "OTF2_CONFIG" && value != "" )
    {
        this->str_otf2_config = value;
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
    std::string libdir = "-L" + dir + " -Wl,-rpath," + dir;

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
