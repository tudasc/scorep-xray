/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Score-P config tool.
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
#include <scorep_config_tool_shmem.h>
#include "SCOREP_Config_LibraryDependencies.hpp"
#include "scorep_config_adapter.hpp"
#include "scorep_config_mpp.hpp"
#include "scorep_config_thread.hpp"
#include "scorep_config_mutex.hpp"

#include <scorep_tools_utils.hpp>

enum
{
    ACTION_EVENT_LIBS = 1,
    ACTION_MGMT_LIBS  = 2,
    ACTION_LIBS       = ACTION_EVENT_LIBS | ACTION_MGMT_LIBS,
    ACTION_CFLAGS,
    ACTION_INCDIR,
    ACTION_LDFLAGS,
    ACTION_CC,
    ACTION_CXX,
    ACTION_FC,
    ACTION_MPICC,
    ACTION_MPICXX,
    ACTION_MPIFC,
    ACTION_SHMEMCC,
    ACTION_SHMEMCXX,
    ACTION_SHMEMFC,
    ACTION_PREFIX,
    ACTION_LIBTOOL,
    ACTION_MPILIBTOOL,
    ACTION_SHMEMLIBTOOL,
    ACTION_COBI_DEPS,
    ACTION_ADAPTER_INIT,
    ACTION_LIBWRAP_LINKTIME,
    ACTION_LIBWRAP_RUNTIME,
    ACTION_TARGETS,
    ACTION_CONSTRUCTOR
};

#define SHORT_HELP \
    "\nUsage:\nscorep-config <command> [<options>]\n\n" \
    "To print out more detailed help information on available parameters, type\n" \
    "scorep-config --help\n"


#define HELPTEXT \
    "\nUsage:\nscorep-config <command> [<options>]\n" \
    "  Commands:\n" \
    "   --prefix   Prints the canonical installation prefix of this Score-P\n" \
    "              installation.\n" \
    "   --cflags   Prints additional compiler flags for a C compiler.\n" \
    "              They already contain the include flags.\n" \
    "   --cxxflags Prints additional compiler flags for a C++ compiler.\n" \
    "              They already contain the include flags.\n" \
    "   --fflags   Prints additional compiler flags for a Fortran compiler.\n" \
    "              They already contain the include flags.\n" \
    "   --cppflags[=language]\n" \
    "              Prints the include flags. They are already contained in the\n" \
    "              output of the --cflags, --cxxflags, and --fflags commands.\n" \
    "              language may be one of c (default), c++, or fortran.\n" \
    "   --ldflags  Prints the library path flags for the linker.\n" \
    "   --libs     Prints the required libraries to link against\n" \
    "              (combines --event-libs and --mgmt-libs).\n" \
    "   --event-libs\n" \
    "              Prints only the required libraries to link against which\n" \
    "              includes event entry points into the measurement.\n" \
    "   --mgmt-libs\n" \
    "              Prints only the required libraries to link against which\n" \
    "              includes management code from the Score-P measurement and\n" \
    "              their dependencies.\n" \
    "   --preload-libs\n" \
    "              Prints only the required libraries which should be listed in\n" \
    "              LD_PRELOAD.\n" \
    "   --cc       Prints the C compiler name.\n" \
    "   --cxx      Prints the C++ compiler name.\n" \
    "   --fc       Prints the Fortran compiler name.\n" \
    "   --mpicc    Prints the MPI C compiler name.\n" \
    "   --mpicxx   Prints the MPI C++ compiler name.\n" \
    "   --mpifc    Prints the MPI Fortran compiler name.\n" \
    "   --shmemcc  Prints the SHMEM C compiler name.\n" \
    "   --shmemcxx Prints the SHMEM C++ compiler name.\n" \
    "   --shmemfc  Prints the SHMEM Fortran compiler name.\n" \
    "   --libtool  Prints the path to the libtool script used to build Score-P\n" \
    "              libraries.\n" \
    "   --mpilibtool\n" \
    "              Prints the path to the libtool script used to build Score-P\n" \
    "              MPI libraries.\n" \
    "   --shmemlibtool\n" \
    "              Prints the path to the libtool script used to build Score-P\n" \
    "              SHMEM libraries.\n" \
    "   --help     Prints this usage information.\n" \
    "   --version  Prints the version number of the Score-P package.\n" \
    "   --scorep-revision\n" \
    "              Prints the revision number of the Score-P package.\n" \
    "   --common-revision\n" \
    "              Prints the revision number of the common package.\n" \
    "   --remap-specfile\n" \
    "              Prints the path to the remapper specification file.\n" \
    "   --adapter-init\n" \
    "              Prints the code for adapter initialization.\n" \
    "   --libwrap-support=linktime\n" \
    "              Prints true if link-time library wrapping is supported.\n" \
    "   --libwrap-support=runtime\n" \
    "              Prints true if run-time library wrapping is supported.\n" \
    "  Options:\n" \
    "   --target   Get flags for specified target, e.g., mic, score.\n" \
    "   --nvcc     Convert flags to be suitable for the nvcc compiler.\n" \
    "   --static   Use only static Score-P libraries if possible.\n" \
    "   --dynamic  Use only dynamic Score-P libraries if possible.\n" \
    "   --online-access|--noonline-access\n" \
    "              Specifies whether online access (needed by Periscope) is\n" \
    "              enabled. On default it is enabled.\n"

std::string m_rpath_head      = "";
std::string m_rpath_delimiter = "";
std::string m_rpath_tail      = "";

enum
{
    TARGET_PLAIN = 0,
    TARGET_MIC   = 1,
    TARGET_SCORE = 2
};

static void
print_help( void )
{
    std::cout << HELPTEXT;
    SCOREP_Config_Adapter::printAll();
    SCOREP_Config_ThreadSystem::printAll();
    SCOREP_Config_Mutex::printAll();
    SCOREP_Config_MppSystem::printAll();
    std::cout << std::endl;
    std::cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">" << std::endl;
}

static void
get_rpath_struct_data( void );

static void
append_ld_run_path_to_rpath( std::deque<std::string>& rpath );

/*
   static void
   write_cobi_deps( const std::deque<std::string>&     libs,
                 SCOREP_Config_LibraryDependencies& deps,
                 bool                               install );
 */

static void
treat_linker_flags_for_nvcc( std::string& flags );

static void
treat_compiler_flags_for_nvcc( std::string& flags );

static std::deque<std::string>
remove_system_path( const std::deque<std::string>& path_list );

static std::deque<std::string>
get_full_library_names( const std::deque<std::string>& library_list,
                        const std::deque<std::string>& path_list,
                        bool                           allow_static,
                        bool                           allow_dynamic );

static void
print_adapter_init_source( void );

static void
delegate( int                argc,
          char**             argv,
          const std::string& target );

static inline void
clean_up()
{
    SCOREP_Config_Mutex::fini();
    SCOREP_Config_ThreadSystem::fini();
    SCOREP_Config_MppSystem::fini();
    SCOREP_Config_Adapter::fini();
}

std::string path_to_binary;
std::string path_to_src;

int
main( int    argc,
      char** argv )
{
    int i;
    /* set default mode to mpi */
    int                    action        = 0;
    int                    ret           = EXIT_SUCCESS;
    SCOREP_Config_Language language      = SCOREP_CONFIG_LANGUAGE_C;
    bool                   nvcc          = false;
    bool                   install       = true;
    bool                   allow_dynamic = true;
    bool                   allow_static  = true;
    bool                   online_access = true;
#if defined( SCOREP_SHARED_BUILD )
    bool preload_libs = false;
#endif

    /* set default target to plain */
    int         target      = TARGET_PLAIN;
    const char* target_name = 0;

    SCOREP_Config_Adapter::init();
    SCOREP_Config_MppSystem::init();
    SCOREP_Config_ThreadSystem::init();
    SCOREP_Config_Mutex::init();

    std::string            binary( argv[ 0 ] );
    std::string::size_type last_slash = binary.find_last_of( "/" );
    if ( last_slash != std::string::npos )
    {
        path_to_binary = binary.substr( 0, last_slash + 1 );
        char* path_c_str = UTILS_IO_JoinPath( 2, path_to_binary.c_str(), AFS_PACKAGE_SRCDIR );
        path_to_src = path_c_str;
        free( path_c_str );
    }

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
        else if ( strcmp( argv[ i ], "--remap-specfile" ) == 0 )
        {
            std::cout << SCOREP_DATADIR << "/scorep.spec" << std::endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--libs" ) == 0 )
        {
            action = ACTION_LIBS;
        }
        else if ( strcmp( argv[ i ], "--event-libs" ) == 0 )
        {
            action = ACTION_EVENT_LIBS;
        }
        else if ( strcmp( argv[ i ], "--mgmt-libs" ) == 0 )
        {
            action = ACTION_MGMT_LIBS;
        }
        else if ( strcmp( argv[ i ], "--preload-libs" ) == 0 )
        {
#if defined( SCOREP_SHARED_BUILD )
            allow_dynamic = true;
            allow_static  = false;
            preload_libs  = true;
            action        = ACTION_EVENT_LIBS;
#else
            std::cerr << "[Score-P] ERROR: Unsupported option: '" << argv[ i ] << "'.\n"
                      << "                 This installation contains no shared Score-P libraries." << std::endl;
            clean_up();
            exit( EXIT_FAILURE );
#endif
        }
        else if ( strcmp( argv[ i ], "--cflags" ) == 0 )
        {
            action   = ACTION_CFLAGS;
            language = SCOREP_CONFIG_LANGUAGE_C;
        }
        else if ( strcmp( argv[ i ], "--cxxflags" ) == 0 )
        {
            action   = ACTION_CFLAGS;
            language = SCOREP_CONFIG_LANGUAGE_CXX;
        }
        else if ( strcmp( argv[ i ], "--fflags" ) == 0 )
        {
            action   = ACTION_CFLAGS;
            language = SCOREP_CONFIG_LANGUAGE_FORTRAN;
        }
        else if ( strcmp( argv[ i ], "--ldflags" ) == 0 )
        {
            action = ACTION_LDFLAGS;
        }
        else if ( ( strcmp( argv[ i ], "--inc" ) == 0 ) |
                  ( strcmp( argv[ i ], "--cppflags" ) == 0 ) )
        {
            action = ACTION_INCDIR;
            /* Default languange is C, which is already set. */
        }
        else if ( ( strncmp( argv[ i ], "--cppflags=", 11 ) == 0 ) )
        {
            action = ACTION_INCDIR;
            std::string language_arg( argv[ i ] + 11 );
            if ( language_arg == "c" )
            {
                language = SCOREP_CONFIG_LANGUAGE_C;
            }
            else if ( language_arg == "c++" )
            {
                language = SCOREP_CONFIG_LANGUAGE_CXX;
            }
            else if ( language_arg == "fortran" )
            {
                language = SCOREP_CONFIG_LANGUAGE_FORTRAN;
            }
            else
            {
                std::cerr << "[Score-P] ERROR: Invalid language for '--cppflags=': " << language_arg << std::endl;
                exit( EXIT_FAILURE );
            }
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
        else if ( strcmp( argv[ i ], "--shmemcc" ) == 0 )
        {
            action = ACTION_SHMEMCC;
        }
        else if ( strcmp( argv[ i ], "--shmemcxx" ) == 0 )
        {
            action = ACTION_SHMEMCXX;
        }
        else if ( strcmp( argv[ i ], "--shmemfc" ) == 0 )
        {
            action = ACTION_SHMEMFC;
        }
        else if ( strcmp( argv[ i ], "--nvcc" ) == 0 )
        {
            nvcc = true;
        }
        else if ( strcmp( argv[ i ], "--prefix" ) == 0 )
        {
            action = ACTION_PREFIX;
        }
        else if ( strcmp( argv[ i ], "--libtool" ) == 0 )
        {
            action = ACTION_LIBTOOL;
        }
        else if ( strcmp( argv[ i ], "--mpilibtool" ) == 0 )
        {
            action = ACTION_MPILIBTOOL;
        }
        else if ( strcmp( argv[ i ], "--shmemlibtool" ) == 0 )
        {
            action = ACTION_SHMEMLIBTOOL;
        }
        else if ( strcmp( argv[ i ], "--build-check" ) == 0 )
        {
            if ( path_to_binary == "" )
            {
                std::cerr << "[Score-P] ERROR: Using '--build-check' requires calling scorep-config not via $PATH" << std::endl;
                exit( EXIT_FAILURE );
            }
            install = false;
        }
        /*
           else if ( strcmp( argv[ i ], "--cobi-deps" ) == 0 )
           {
            action = ACTION_COBI_DEPS;
           }
         */
        else if ( strcmp( argv[ i ], "--constructor" ) == 0 )
        {
            action = ACTION_CONSTRUCTOR;
        }
        else if ( strcmp( argv[ i ], "--dynamic" ) == 0 )
        {
            allow_static = false;
        }
        else if ( strcmp( argv[ i ], "--static" ) == 0 )
        {
            allow_dynamic = false;
        }
        else if ( strcmp( argv[ i ], "--online-access" ) == 0 )
        {
            online_access = true;
        }
        else if ( strcmp( argv[ i ], "--noonline-access" ) == 0 )
        {
            online_access = false;
        }
        else if ( strcmp( argv[ i ], "--adapter-init" ) == 0 )
        {
            action = ACTION_ADAPTER_INIT;
        }
        else if ( strcmp( argv[ i ], "--libwrap-support=linktime" ) == 0 )
        {
            action = ACTION_LIBWRAP_LINKTIME;
        }
        else if ( strcmp( argv[ i ], "--libwrap-support=runtime" ) == 0 )
        {
            action = ACTION_LIBWRAP_RUNTIME;
        }
        else if ( strncmp( argv[ i ], "--thread=", 9 ) == 0 )
        {
            std::string arg( &argv[ i ][ 9 ] );
            bool        known_arg = SCOREP_Config_ThreadSystem::checkAll( arg );
            if ( !known_arg )
            {
                std::cerr << "[Score-P] ERROR: Unknown threading system: '" << arg << "'" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
        else if ( strncmp( argv[ i ], "--mpp=", 6 ) == 0 )
        {
            std::string arg( &argv[ i ][ 6 ] );
            bool        known_arg = SCOREP_Config_MppSystem::checkAll( arg );
            if ( !known_arg )
            {
                std::cerr << "[Score-P] ERROR: Unknown multi-process paradigm: '" << arg << "'" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
        else if ( strncmp( argv[ i ], "--mutex=", 8 ) == 0 )
        {
            std::string arg( &argv[ i ][ 8 ] );
            bool        known_arg = SCOREP_Config_Mutex::checkAll( arg );
            if ( !known_arg )
            {
                std::cerr << "[Score-P] ERROR: Unknown locking system: '" << arg << "'" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
        else if ( strncmp( argv[ i ], "--target", 8 ) == 0 )
        {
            if ( argv[ i ][ 8 ] == '=' )
            {
                target_name = argv[ i ] + 9;
            }
            else
            {
                target_name = argv[ i + 1 ];
                i++;
            }
            if ( !target_name )
            {
                std::cerr << "[Score-P] ERROR: Missing argument for '--target'" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }

            if ( !strcmp( target_name, "mic" ) )
            {
                target = TARGET_MIC;
            }
            else if ( !strcmp( target_name, "score" ) )
            {
                target = TARGET_SCORE;
            }
            else
            {
                std::cerr << "[Score-P] ERROR: Unknown target: '" << target_name << "'" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
        else
        {
            std::string arg( argv[ i ] );
            bool        known_arg = SCOREP_Config_Adapter::checkAll( arg );
            if ( !known_arg )
            {
                std::cerr << "[Score-P] ERROR: Unknown option: '" << arg << "'" << std::endl;
                clean_up();
                exit( EXIT_FAILURE );
            }
        }
    }

    if ( target == TARGET_MIC )
    {
#if HAVE( MIC_SUPPORT )
        delegate( argc, argv, "mic" );
#else
#if !HAVE( PLATFORM_MIC )
        std::cerr << "[Score-P] ERROR: Unsupported target: '" << target_name << "'" << std::endl;
        clean_up();
        exit( EXIT_FAILURE );
#endif  /* !HAVE( PLATFORM_MIC ) */
#endif  /* HAVE( MIC_SUPPORT ) */
    }

    if ( target == TARGET_SCORE )
    {
        std::deque<std::string>           libs;
        SCOREP_Config_LibraryDependencies deps;
        std::string                       str;

        libs.push_back( "libscorep_estimator" );
        switch ( action )
        {
            case ACTION_LDFLAGS:
                get_rpath_struct_data();
                std::cout << deque_to_string( deps.getLDFlags( libs, install ),
                                              " ", " ", "" );
                if ( USE_LIBDIR_FLAG )
                {
                    std::deque<std::string> rpath = deps.getRpathFlags( libs, install );
                    append_ld_run_path_to_rpath( rpath );
                    rpath = remove_system_path( rpath );
                    str   = deque_to_string( rpath,
                                             m_rpath_head + m_rpath_delimiter,
                                             m_rpath_delimiter,
                                             m_rpath_tail );
                }
                std::cout << str;
                std::cout.flush();
                break;

            case ACTION_LIBS:
            {
                if ( !allow_dynamic || !allow_static )
                {
                    std::deque<std::string> rpath = deps.getRpathFlags( libs, install );
                    libs = get_full_library_names( deps.getLibraries( libs ),
                                                   rpath,
                                                   allow_static,
                                                   allow_dynamic );
                }
                else
                {
                    libs = deps.getLibraries( libs );
                }
                std::cout << deque_to_string( libs, " ", " ", "" );
                std::cout.flush();
            }
            break;

            case ACTION_CFLAGS:
            case ACTION_INCDIR:
                if ( install )
                {
                    str += "-I" SCOREP_INCLUDEDIR;
                }
                else
                {
                    str += "-I" + path_to_src + "include ";
                }
                std::cout << str;
                std::cout.flush();
                break;

            default:
                std::cout << SHORT_HELP << std::endl;
                break;
        }

        clean_up();
        exit( EXIT_SUCCESS );
    }

    std::deque<std::string>           libs;
    SCOREP_Config_LibraryDependencies deps;
    std::string                       str;

    SCOREP_Config_Adapter::addLibsAll( libs, deps );
    SCOREP_Config_MppSystem::current->addLibs( libs, deps, online_access );
    SCOREP_Config_MutexId newMutexId = SCOREP_Config_ThreadSystem::current->validateDependencies();
    SCOREP_Config_ThreadSystem::current->addLibs( libs, deps );
    SCOREP_Config_Mutex::select( newMutexId );
    SCOREP_Config_Mutex::current->addLibs( libs, deps );

#if defined( SCOREP_SHARED_BUILD )
    if ( preload_libs )
    {
        /* libscorep_measurement.so must be in the event libs */
        libs.push_back( "libscorep_measurement" );

#if HAVE_BACKEND( COMPILER_CONSTRUCTOR_SUPPORT )
        /* --preload-libs only outputs the `libs` list, not any of the dependencies */
        libs.push_back( "libscorep_constructor" );
#endif
    }
#endif

    switch ( action )
    {
        case ACTION_LDFLAGS:
            get_rpath_struct_data();
            std::cout << deque_to_string( deps.getLDFlags( libs, install ),
                                          " ", " ", "" );
            if ( USE_LIBDIR_FLAG )
            {
                std::deque<std::string> rpath = deps.getRpathFlags( libs, install );
                append_ld_run_path_to_rpath( rpath );
                rpath = remove_system_path( rpath );
                str   = deque_to_string( rpath,
                                         m_rpath_head + m_rpath_delimiter,
                                         m_rpath_delimiter,
                                         m_rpath_tail );
            }
            if ( SCOREP_Config_Adapter::isActive() )
            {
                str += " " SCOREP_INSTRUMENTATION_LDFLAGS;
            }
            SCOREP_Config_Adapter::addLdFlagsAll( str, !install, nvcc );
            SCOREP_Config_MppSystem::current->addLdFlags( str, !install, nvcc );
            SCOREP_Config_ThreadSystem::current->addLdFlags( str, nvcc );

            if ( nvcc )
            {
                treat_linker_flags_for_nvcc( str );
            }
            std::cout << str;
            std::cout.flush();
            break;

        case ACTION_EVENT_LIBS:
        case ACTION_MGMT_LIBS:
        case ACTION_LIBS:
        {
            bool honor_libs = !!( action & ACTION_EVENT_LIBS );
            bool honor_deps = !!( action & ACTION_MGMT_LIBS );
            if ( !allow_dynamic || !allow_static )
            {
                std::deque<std::string> rpath = deps.getRpathFlags( libs, install, honor_libs, honor_deps );
                libs = get_full_library_names( deps.getLibraries( libs, honor_libs, honor_deps ),
                                               rpath,
                                               allow_static,
                                               allow_dynamic );
            }
            else
            {
                libs = deps.getLibraries( libs, honor_libs, honor_deps );
            }
            std::cout << deque_to_string( libs, " ", " ", "" );
            std::cout.flush();
        }
        break;

        case ACTION_CFLAGS:
            if ( SCOREP_Config_Adapter::isActive() )
            {
                str += " " SCOREP_INSTRUMENTATION_CPPFLAGS;
            }
            SCOREP_Config_Adapter::addCFlagsAll( str, !install, language, nvcc );
            SCOREP_Config_ThreadSystem::current->addCFlags( str, !install, language, nvcc );

        // Append the include directories, too
        case ACTION_INCDIR:
            if ( install )
            {
                str += "-I" SCOREP_INCLUDEDIR " -I" SCOREP_INCLUDEDIR "/scorep ";
            }
            else
            {
                str += "-I" + path_to_src + "include " +
                       "-I" + path_to_src + "include/scorep ";
            }
            SCOREP_Config_Adapter::addIncFlagsAll( str, !install, language, nvcc );
            SCOREP_Config_ThreadSystem::current->addIncFlags( str, !install, language, nvcc );

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

        case ACTION_SHMEMCC:
            std::cout << SCOREP_SHMEMCC;
            std::cout.flush();
            break;

        case ACTION_SHMEMCXX:
            std::cout << SCOREP_SHMEMCXX;
            std::cout.flush();
            break;

        case ACTION_SHMEMFC:
            std::cout << SCOREP_SHMEMFC;
            std::cout.flush();
            break;

        case ACTION_PREFIX:
            std::cout << SCOREP_PREFIX;
            std::cout.flush();
            break;

        case ACTION_LIBTOOL:
            std::cout << SCOREP_BACKEND_PKGLIBEXECDIR << "/libtool";
            std::cout.flush();
            break;

        case ACTION_MPILIBTOOL:
#if HAVE_BACKEND( MPI_SUPPORT )
            std::cout << SCOREP_BACKEND_PKGLIBEXECDIR << "/mpi/libtool";
#endif
            std::cout.flush();
            break;

        case ACTION_SHMEMLIBTOOL:
#if HAVE_BACKEND( SHMEM_SUPPORT )
            std::cout << SCOREP_BACKEND_PKGLIBEXECDIR << "/shmem/libtool";
#endif
            std::cout.flush();
            break;

        /*
           case ACTION_COBI_DEPS:
           if ( libs.empty() )
           {
            libs.push_back( "libscorep_measurement" );
           }
           write_cobi_deps( libs, deps, install );
           break;
         */
        case ACTION_ADAPTER_INIT:
            print_adapter_init_source();
            break;

        case ACTION_CONSTRUCTOR:
            /* ignore this request if no support is avaible */
#if HAVE_BACKEND( COMPILER_CONSTRUCTOR_SUPPORT )
            /* only add the constructor object if we would also add libs */
            if ( !libs.empty() )
            {
                if ( install )
                {
                    std::cout << SCOREP_BACKEND_PKGLIBDIR "/scorep_constructor." OBJEXT << std::endl;
                }
                else
                {
                    std::cout << path_to_binary + "../build-backend/scorep_constructor." OBJEXT << std::endl;
                }
            }
#endif
            break;

        case ACTION_LIBWRAP_LINKTIME:
#if HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
            std::cout << "true" << std::endl;
#else
            std::cout << "false" << std::endl;
#endif
            break;

        case ACTION_LIBWRAP_RUNTIME:
#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )
            std::cout << "true" << std::endl;
#else
            std::cout << "false" << std::endl;
#endif
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
    // Replace $wl by LIBDIR_FLAG_WL and erase everything from
    // $libdir on in order to create m_rpath_head and
    // m_rpath_delimiter. This will work for most and for the relevant
    // (as we know in 2012-07) values of LIBDIR_FLAG_CC. Some possible
    // values are (see also ticket 530,
    // https://silc.zih.tu-dresden.de/trac-silc/ticket/530):
    // '+b $libdir'
    // '-L$libdir'
    // '-R$libdir'
    // '-rpath $libdir'
    // '$wl-blibpath:$libdir:'"$aix_libpath"
    // '$wl+b $wl$libdir'
    // '$wl-R,$libdir'
    // '$wl-R $libdir:/usr/lib:/lib'
    // '$wl-rpath,$libdir'
    // '$wl--rpath $wl$libdir'
    // '$wl-rpath $wl$libdir'
    // '$wl-R $wl$libdir'
    // For a complete list, check the currently used libtool.m4.
    std::string            rpath_flag = LIBDIR_FLAG_CC;
    std::string::size_type index      = 0;
    while ( true )
    {
        index = rpath_flag.find( "$wl", index );
        if ( index == std::string::npos )
        {
            break;
        }
        rpath_flag.replace( index, strlen( "$wl" ), LIBDIR_FLAG_WL );
        ++index;
    }
    index = rpath_flag.find( "$libdir", 0 );
    if ( index != std::string::npos )
    {
        rpath_flag.erase( index );
    }

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
 * Add content of the environment variable LD_RUN_PATH to rpath
 */
static void
append_ld_run_path_to_rpath( std::deque<std::string>& rpath )
{
    /* Get variable values */
    const char* ld_run_path = getenv( "LD_RUN_PATH" );
    if ( ld_run_path == NULL || *ld_run_path == '\0' )
    {
        return;
    }

    std::deque<std::string> run_path = string_to_deque( ld_run_path, ":" );
    rpath.insert( rpath.end(), run_path.begin(), run_path.end() );
}

static bool
is_scorep_lib( const std::string& name )
{
    if ( name.length() < 6 )
    {
        return false;
    }
    if ( name.substr( 0, 6 ) == "-lotf2" ||
         name.substr( 0, 6 ) == "-lcube" )
    {
        return true;
    }
    if ( name.length() >= 8 &&
         name.substr( 0, 8 ) == "-lscorep" )
    {
        return true;
    }
    return false;
}

static std::deque<std::string>
get_full_library_names( const std::deque<std::string>& library_list,
                        const std::deque<std::string>& path_list,
                        bool                           allow_static,
                        bool                           allow_dynamic )
{
    std::vector<std::string> pathlist( path_list.begin(), path_list.end() );
    std::deque<std::string>  full_names;
    for ( std::deque<std::string>::const_iterator lib = library_list.begin();
          lib != library_list.end(); lib++ )
    {
        if ( is_scorep_lib( *lib ) )
        {
            std::string name = find_library( *lib, pathlist, allow_static, allow_dynamic );
            if ( name != "" )
            {
                full_names.push_back( name );
            }
            else
            {
                full_names.push_back( *lib );
            }
        }
        else
        {
            full_names.push_back( *lib );
        }
    }
    return full_names;
}

/*
   static void
   write_cobi_deps( const std::deque<std::string>&     libs,
                 SCOREP_Config_LibraryDependencies& deps,
                 bool                               install )
   {
    std::deque<std::string> library_list = deps.getLibraries( libs );
    std::deque<std::string> path_list    = deps.getRpathFlags( libs, install );
    std::cout << deque_to_string( get_full_library_names( library_list,
                                                          path_list,
                                                          false, true, true ),
                                  "\t<library name=\"",
                                  "\" />\n\t<library name=\"",
                                  "\" />\n" );
   }
 */

static void
treat_linker_flags_for_nvcc( std::string& flags )
{
    std::string pattern1 = " ";
    std::string replace1 = ",";
    std::string pattern2 = LIBDIR_FLAG_WL;
    std::string replace2 = "";

    flags = remove_multiple_whitespaces( flags );
    if ( flags.length() == 0 )
    {
        /* empty flags don't need the -Xlinker prefix */
        return;
    }
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
    if ( flags.length() == 0 )
    {
        /* empty flags don't need the -Xcompiler prefix */
        return;
    }
    /* Replace all white-spaces by comma */
    flags = replace_all( pattern1, replace1, flags );

    flags = " -Xcompiler " + flags;
}


static std::deque<std::string>
remove_system_path( const std::deque<std::string>& path_list )
{
    std::string             dlsearch_path = SCOREP_BACKEND_SYS_LIB_DLSEARCH_PATH;
    std::deque<std::string> system_paths  = string_to_deque( dlsearch_path, " " );
    std::deque<std::string> result_paths;

    std::deque<std::string>::iterator       sys_path;
    std::deque<std::string>::const_iterator app_path;

    for ( app_path = path_list.begin(); app_path != path_list.end(); app_path++ )
    {
        bool is_sys_path = false;
        for ( sys_path = system_paths.begin();
              sys_path != system_paths.end(); sys_path++ )
        {
            if ( *app_path == *sys_path )
            {
                is_sys_path = true;
            }
        }
        if ( !is_sys_path )
        {
            result_paths.push_back( *app_path );
        }
    }
    return result_paths;
}

static void
print_adapter_init_source( void )
{
    std::cout << "#include <stddef.h>\n\n";

    std::deque<std::string> init_structs;
    SCOREP_Config_Adapter::getAllInitStructNames( init_structs );
    SCOREP_Config_MppSystem::current->getInitStructName( init_structs );
    SCOREP_Config_ThreadSystem::current->getInitStructName( init_structs );
    if ( !init_structs.empty() )
    {
        init_structs.push_front( "SCOREP_Subsystem_PlatformTopology" );
        init_structs.push_front( "SCOREP_Subsystem_Topologies" );
#if HAVE_BACKEND( SAMPLING_SUPPORT )
        init_structs.push_front( "SCOREP_Subsystem_SamplingService" );
#endif
#if HAVE_BACKEND( UNWINDING_SUPPORT )
        init_structs.push_front( "SCOREP_Subsystem_UnwindingService" );
#endif
        init_structs.push_front( "SCOREP_Subsystem_MetricService" );
        init_structs.push_front( "SCOREP_Subsystem_TaskStack" );
        init_structs.push_front( "SCOREP_Subsystem_Substrates" );

        init_structs = remove_double_entries( init_structs );

        std::cout << deque_to_string( init_structs,
                                      "extern const struct SCOREP_Subsystem ",
                                      ";\nextern const struct SCOREP_Subsystem ",
                                      ";\n" );

        std::cout << "\nconst struct SCOREP_Subsystem* scorep_subsystems[] = {\n";

        std::cout << deque_to_string( init_structs,
                                      "    &",
                                      ",\n    &",
                                      "\n" );
        std::cout << "};\n\n";

        std::cout << "const size_t scorep_number_of_subsystems = sizeof( scorep_subsystems ) /\n"
                  << "                                           sizeof( scorep_subsystems[ 0 ] );"
                  << std::endl;
    }
    else
    {
        std::cout << "\nconst struct SCOREP_Subsystem** scorep_subsystems;\n\n"
                  << "const size_t scorep_number_of_subsystems = 0;"
                  << std::endl;
    }
}

void
delegate( int                argc,
          char**             argv,
          const std::string& target )
{
    // Construct command string with all original arguments
    std::string command = PKGLIBEXECDIR "/scorep-config-" + target;
    for ( int i = 1; i < argc; ++i )
    {
        command += " " + std::string( argv[ i ] );
    }

    // Delegate to scorep-config command of target build
    exit( system( command.c_str() ) );
}
