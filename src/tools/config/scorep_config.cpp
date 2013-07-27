/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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

#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

#include <UTILS_IO.h>

#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "SCOREP_Config_LibraryDependencies.hpp"
#include "scorep_config_adapter.hpp"
#include "scorep_config_mpp.hpp"
#include "scorep_config_thread.hpp"
#include "scorep_config_utils.hpp"

#define ACTION_LIBS           1
#define ACTION_CFLAGS         2
#define ACTION_INCDIR         3
#define ACTION_LDFLAGS        4
#define ACTION_CC             5
#define ACTION_CXX            6
#define ACTION_FC             7
#define ACTION_MPICC          8
#define ACTION_MPICXX         9
#define ACTION_MPIFC         10
#define ACTION_COBI_DEPS     11

#define SHORT_HELP \
    "\nUsage:\nscorep-config <command> [<options>]\n\n" \
    "To print out more detailed help information on available parameters, type\n" \
    "scorep-config --help\n"


#define HELPTEXT \
    "\nUsage:\nscorep-config <command> [<options>]\n" \
    "  Commands:\n" \
    "   --cflags    prints additional compiler flags. They already contain the\n" \
    "               include flags.\n" \
    "   --cppflags  prints the include flags. They are already contained in the\n" \
    "               output of the --cflags command\n" \
    "   --ldflags   prints the library path flags for the linker\n" \
    "   --libs      prints the required linker flags\n" \
    "   --cc        prints the C compiler name\n" \
    "   --cxx       prints the C++ compiler name\n" \
    "   --fc        prints the Fortran compiler name\n" \
    "   --mpicc     prints the MPI C compiler name\n" \
    "   --mpicxx    prints the MPI C++ compiler name\n" \
    "   --mpifc     prints the MPI Fortran compiler name\n" \
    "   --cobi-deps prints the dependency library part for the Cobi adapter file\n" \
    "   --help      prints this usage information\n" \
    "   --version   prints the version number of the scorep package\n" \
    "   --scorep-revision prints the revision number of the scorep package\n" \
    "   --common-revision prints the revision number of the common package\n" \
    "  Options:\n" \
    "   --fortran   Specifies that the required flags are for the Fortran compiler.\n" \
    "   --nvcc      Convert flags to be suitable for the nvcc compiler.\n"

std::string m_rpath_head      = "";
std::string m_rpath_delimiter = "";
std::string m_rpath_tail      = "";

static void
print_help( void )
{
    std::cout << HELPTEXT;
    for ( std::deque<SCOREP_Config_Adapter*>::iterator i = scorep_adapters.begin();
          i != scorep_adapters.end(); i++ )
    {
        ( *i )->printHelp();
    }
    std::cout << "   --thread=<threading system>[:<variant>]\n"
              << "            Available threading systems are:\n";
    for ( std::deque<SCOREP_Config_ThreadSystem*>::iterator i = scorep_thread_systems.begin();
          i != scorep_thread_systems.end(); i++ )
    {
        ( *i )->printHelp();
    }
    std::cout << "            If no variant is specified the first matching\n"
              << "            threading system is used.\n";
    std::cout << "   --mpp=<multi-process paradigm>\n"
              << "            Available multi-process paradigms are:\n";
    for ( std::deque<SCOREP_Config_MppSystem*>::iterator i = scorep_mpp_systems.begin();
          i != scorep_mpp_systems.end(); i++ )
    {
        ( *i )->printHelp();
    }
}

static void
get_rpath_struct_data( void );

static std::string
append_ld_run_path_to_rpath( std::string rpath );

static void
write_cobi_deps( SCOREP_Config_LibraryDependencies& deps,
                 const std::deque<std::string>&     libs,
                 bool                               install );

static void
treat_linker_flags_for_nvcc( std::string& flags );

static void
treat_compiler_flags_for_nvcc( std::string& flags );

static inline void
clean_up()
{
    scorep_config_final_thread_systems();
    scorep_config_final_mpp_systems();
    scorep_config_final_adapters();
}

int
main( int    argc,
      char** argv )
{
    int i;
    /* set default mode to mpi */
    int  action  = 0;
    int  ret     = EXIT_SUCCESS;
    bool fortran = false;
    bool nvcc    = false;
    bool install = true;

    SCOREP_Config_LibraryDependencies                 deps;
    std::deque<SCOREP_Config_Adapter*>::iterator      adapter;
    std::deque<SCOREP_Config_MppSystem*>::iterator    mpp;
    std::deque<SCOREP_Config_ThreadSystem*>::iterator ts;

    scorep_config_init_adapters();
    scorep_config_init_mpp_systems();
    scorep_config_init_thread_systems();

    /* parsing the command line */
    for ( i = 1; i < argc; i++ )
    {
        if ( strcmp( argv[ i ], "--help" ) == 0 || strcmp( argv[ i ], "-h" ) == 0 )
        {
            print_help();
            clean_up();
            return EXIT_SUCCESS;
        }
        else if ( strcmp( argv[ i ], "--version" ) == 0 )
        {
            std::cout << PACKAGE_VERSION;
            std::cout.flush();
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--scorep-revision" ) == 0 )
        {
            std::cout << SCOREP_COMPONENT_REVISION << std::endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--common-revision" ) == 0 )
        {
            std::cout << SCOREP_COMMON_REVISION << std::endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--libs" ) == 0 )
        {
            action = ACTION_LIBS;
        }
        else if ( strcmp( argv[ i ], "--cflags" ) == 0 )
        {
            action = ACTION_CFLAGS;
        }
        else if ( strcmp( argv[ i ], "--ldflags" ) == 0 )
        {
            action = ACTION_LDFLAGS;
        }
        else if ( ( strcmp( argv[ i ], "--inc" ) == 0 ) |
                  ( strcmp( argv[ i ], "--cppflags" ) == 0 ) )
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
        else if ( strcmp( argv[ i ], "--mpicc" ) == 0 )
        {
            action = ACTION_MPICC;
        }
        else if ( strcmp( argv[ i ], "--mpicxx" ) == 0 )
        {
            action = ACTION_MPICXX;
        }
        else if ( strcmp( argv[ i ], "--mpifc" ) == 0 )
        {
            action = ACTION_MPIFC;
        }
        else if ( strcmp( argv[ i ], "--fortran" ) == 0 )
        {
            fortran = true;
        }
        else if ( strcmp( argv[ i ], "--nvcc" ) == 0 )
        {
            nvcc = true;
        }
        else if ( strcmp( argv[ i ], "--build-check" ) == 0 )
        {
            install = false;
        }
        else if ( strcmp( argv[ i ], "--cobi-deps" ) == 0 )
        {
            action = ACTION_COBI_DEPS;
        }
        else if ( strncmp( argv[ i ], "--thread=", 9 ) == 0 )
        {
            bool known_arg = false;
            for ( ts = scorep_thread_systems.begin();
                  ts != scorep_thread_systems.end(); ts++ )
            {
                known_arg = ( *ts )->checkArgument( &argv[ i ][ 9 ] );
                if ( known_arg )
                {
                    break;
                }
            }
            if ( !known_arg )
            {
                std::cerr << "\nUnknown threading system " << &argv[ i ][ 9 ]
                          << ". Abort.\n" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
        else if ( strncmp( argv[ i ], "--mpp=", 6 ) == 0 )
        {
            bool known_arg = false;
            for ( mpp = scorep_mpp_systems.begin();
                  mpp != scorep_mpp_systems.end(); mpp++ )
            {
                known_arg = ( *mpp )->checkArgument( &argv[ i ][ 6 ] );
                if ( known_arg )
                {
                    break;
                }
            }
            if ( !known_arg )
            {
                std::cerr << "\nUnknown multi-process paradigm " << &argv[ i ][ 6 ]
                          << ". Abort.\n" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
        else
        {
            bool known_arg = false;
            for ( adapter = scorep_adapters.begin();
                  adapter != scorep_adapters.end(); adapter++ )
            {
                known_arg = ( *adapter )->checkArgument( argv[ i ] );
                if ( known_arg )
                {
                    break;
                }
            }
            if ( !known_arg )
            {
                std::cerr << "\nUnknown option " << argv[ i ]
                          << ". Abort.\n" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
    }

    std::deque<std::string> libs;
    std::string             str;

    for ( adapter = scorep_adapters.begin(); adapter != scorep_adapters.end(); adapter++ )
    {
        ( *adapter )->addLibs( libs, deps );
    }
    SCOREP_Config_MppSystem::current->addLibs( libs, deps );
    SCOREP_Config_ThreadSystem::current->addLibs( libs, deps );

    switch ( action )
    {
        case ACTION_LDFLAGS:
            get_rpath_struct_data();
            std::cout << deque_to_string( deps.getLDFlags( libs, install ),
                                          " ", " ", "" );
            if ( USE_LIBDIR_FLAG )
            {
                str = deque_to_string( deps.getRpathFlags( libs, install ),
                                       m_rpath_head + m_rpath_delimiter,
                                       m_rpath_delimiter,
                                       m_rpath_tail );
                str = append_ld_run_path_to_rpath( str );
            }
            for ( adapter = scorep_adapters.begin();
                  adapter != scorep_adapters.end(); adapter++ )
            {
                ( *adapter )->addLdFlags( str );
            }

            if ( nvcc )
            {
                treat_linker_flags_for_nvcc( str );
            }
            std::cout << str;
            std::cout.flush();
            break;

        case ACTION_LIBS:
            std::cout << deque_to_string( deps.getLibraries( libs ),
                                          " ", " ", "" );
            std::cout.flush();
            break;

        case ACTION_CFLAGS:
            for ( adapter = scorep_adapters.begin();
                  adapter != scorep_adapters.end(); adapter++ )
            {
                ( *adapter )->addCFlags( str, !install, fortran, nvcc );
            }
            SCOREP_Config_ThreadSystem::current->addCFlags( str, !install, fortran, nvcc );

        // Append the include directories, too
        case ACTION_INCDIR:
            if ( install )
            {
                str += "-I" SCOREP_PREFIX "/include -I" SCOREP_PREFIX "/include/scorep ";
            }
            else
            {
                str += "-I" BUILD_SCOREP_PREFIX "/include -I" BUILD_SCOREP_PREFIX "/include/scorep ";
            }
            for ( adapter = scorep_adapters.begin();
                  adapter != scorep_adapters.end(); adapter++ )
            {
                ( *adapter )->addIncFlags( str, !install, nvcc );
            }
            SCOREP_Config_ThreadSystem::current->addIncFlags( str, !install, nvcc );

            if ( nvcc )
            {
                treat_compiler_flags_for_nvcc( str );
            }
            std::cout << str;
            std::cout.flush();
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

        case ACTION_MPICC:
            std::cout << SCOREP_MPICC;
            std::cout.flush();
            break;

        case ACTION_MPICXX:
            std::cout << SCOREP_MPICXX;
            std::cout.flush();
            break;

        case ACTION_MPIFC:
            std::cout << SCOREP_MPIFC;
            std::cout.flush();
            break;

        case ACTION_COBI_DEPS:
            if ( libs.empty() )
            {
                libs.push_back( "libscorep_measurement" );
            }
            write_cobi_deps( deps, libs, install );
            break;

        default:
            std::cout << SHORT_HELP << std::endl;
            break;
    }

    clean_up();
    return ret;
}


/** constructor and destructor */
void
get_rpath_struct_data( void )
{
    // Replace ${wl} by LIBDIR_FLAG_WL and erase everything from
    // $libdir on in order to create m_rpath_head and
    // m_rpath_delimiter. This will work for most and for the relevant
    // (as we know in 2012-07) values of LIBDIR_FLAG_CC. Possible
    // values are (see also ticket 530,
    // https://silc.zih.tu-dresden.de/trac-silc/ticket/530):
    // '+b $libdir'
    // '-L$libdir'
    // '-R$libdir'
    // '-rpath $libdir'
    // '${wl}-blibpath:$libdir:'"$aix_libpath"
    // '${wl}+b ${wl}$libdir'
    // '${wl}-R,$libdir'
    // '${wl}-R $libdir:/usr/lib:/lib'
    // '${wl}-rpath,$libdir'
    // '${wl}--rpath ${wl}$libdir'
    // '${wl}-rpath ${wl}$libdir'
    // '${wl}-R $wl$libdir'
    std::string rpath_flag = LIBDIR_FLAG_CC;
    size_t      index      = 0;
    while ( true )
    {
        index = rpath_flag.find( "${wl}", index );
        if ( index == std::string::npos )
        {
            break;
        }
        rpath_flag.replace( index, strlen( "${wl}" ), LIBDIR_FLAG_WL );
        ++index;
    }
    index = rpath_flag.find( "$libdir", 0 );
    rpath_flag.erase( index );

#if HAVE( PLATFORM_AIX )
    m_rpath_head      = " " + rpath_flag;
    m_rpath_delimiter = ":";
    m_rpath_tail      = ":" LIBDIR_AIX_LIBPATH;
#else
    m_rpath_head      = "";
    m_rpath_delimiter = " " + rpath_flag;
    m_rpath_tail      = "";
#endif
}

/**
 * Add content of the environment variable LD_RUN_PATH as -rpath argument
 */
static std::string
append_ld_run_path_to_rpath( std::string rpath )
{
    /* Get variable values */
    const char* ld_run_path = getenv( "LD_RUN_PATH" );
    if ( ld_run_path == NULL || *ld_run_path == '\0' )
    {
        return rpath;
    }

#if HAVE( PLATFORM_AIX )
    /* On AIX ist just a colon separated list, after a head */
    if ( rpath == "" )
    {
        return m_rpath_head + ld_run_path;
    }
    return rpath + m_rpath_delimiter + ld_run_path;
#else
    /* Otherwise replace all colons by the rpath flags */
    rpath += m_rpath_delimiter + replace_all( ":", m_rpath_delimiter, ld_run_path );
    return rpath;
#endif
}

/**
 * Checks whether a file with @a filename exists.
 * @param file The filename of the file,
 * @returns true if a file with @a filename exists.
 */
static bool
exists_file( const std::string& filename )
{
    std::ifstream ifile( filename.c_str() );
    return ifile;
}

static std::string
find_library( std::string                    library,
              const std::deque<std::string>& path_list,
              bool                           allow_static,
              bool                           allow_shared )
{
    std::string current_path;

    if ( library.substr( 0, 2 ) == "-l" )
    {
        library.replace( 0, 2, "lib" );
    }
    for ( std::deque<std::string>::const_iterator path = path_list.begin();
          path != path_list.end(); path++ )
    {
        current_path = *path + "/" + library;
        if ( allow_shared && exists_file( current_path + ".so" ) )
        {
            return current_path + ".so";
        }
        if ( allow_static && exists_file( current_path + ".a" ) )
        {
            return current_path + ".a";
        }
    }

    return "";
}

static std::deque<std::string>
get_full_library_names( const std::deque<std::string>& library_list,
                        const std::deque<std::string>& path_list,
                        bool                           allow_static,
                        bool                           allow_shared )
{
    std::deque<std::string> full_names;
    for ( std::deque<std::string>::const_iterator lib = library_list.begin();
          lib != library_list.end(); lib++ )
    {
        std::string name = find_library( *lib, path_list, allow_static, allow_shared );
        if ( name != "" )
        {
            full_names.push_back( name );
        }
    }
    return full_names;
}

static void
write_cobi_deps( SCOREP_Config_LibraryDependencies& deps,
                 const std::deque<std::string>&     libs,
                 bool                               install )
{
    std::deque<std::string> library_list = deps.getLibraries( libs );
    std::deque<std::string> path_list    = deps.getRpathFlags( libs, install );
    std::cout << deque_to_string( get_full_library_names( library_list,
                                                          path_list,
                                                          false, true ),
                                  "\t<library name=\"",
                                  "\" />\n\t<library name=\"",
                                  "\" />\n" );
}


static void
treat_linker_flags_for_nvcc( std::string& flags )
{
    std::string pattern1 = " ";
    std::string replace1 = ",";
    std::string pattern2 = LIBDIR_FLAG_WL;
    std::string replace2 = "";

    flags = remove_multiple_whitespaces( flags );
    /* Replace all white-spaces by comma */
    flags = replace_all( pattern1, replace1, flags );
    /* Replace flag for passing arguments to linker through compiler
     * (flags not needed because we use '-Xlinker' to specify linker
     * flags when using CUDA compiler */
    if ( pattern2.length() != 0 )
    {
        flags = replace_all( pattern2, replace2, flags );
    }

    flags = " -Xlinker " + flags;
}


static void
treat_compiler_flags_for_nvcc( std::string& flags )
{
    std::string pattern1 = " ";
    std::string replace1 = ",";

    flags = remove_multiple_whitespaces( flags );
    /* Replace all white-spaces by comma */
    flags = replace_all( pattern1, replace1, flags );

    flags = " -Xcompiler " + flags;
}
