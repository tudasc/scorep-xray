/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Collects information about available adapters
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include "scorep_config_adapter.hpp"
#include "scorep_config_thread.hpp"
#include "scorep_config_utils.hpp"

#include <scorep_tools_utils.hpp>

#include <iostream>
#include <stdlib.h>

/* ****************************************************************************
   Compiler specific defines
******************************************************************************/

#if SCOREP_BACKEND_COMPILER_CRAY
#define SCOREP_COMPILER_TYPE "cray"

#elif SCOREP_BACKEND_COMPILER_GNU
#define SCOREP_COMPILER_TYPE "gnu"

#elif SCOREP_BACKEND_COMPILER_IBM
#define SCOREP_COMPILER_TYPE "ibm"

#elif SCOREP_BACKEND_COMPILER_INTEL
#define SCOREP_COMPILER_TYPE "intel"

#elif SCOREP_BACKEND_COMPILER_PGI
#define SCOREP_COMPILER_TYPE "pgi"

#elif SCOREP_BACKEND_COMPILER_STUDIO
#define SCOREP_COMPILER_TYPE "sun"

#elif SCOREP_BACKEND_COMPILER_FUJITSU
#define SCOREP_COMPILER_TYPE "fujitsu"

#elif SCOREP_BACKEND_COMPILER_CLANG
#define SCOREP_COMPILER_TYPE "gnu"

#else
#error "Missing compiler specific handling, extension required."
#endif

/* **************************************************************************************
 * class SCOREP_Config_Adapter
 * *************************************************************************************/

std::deque<SCOREP_Config_Adapter*> SCOREP_Config_Adapter::all;
void
SCOREP_Config_Adapter::init( void )
{
#if HAVE_BACKEND( COMPILER_INSTRUMENTATION )
    all.push_back( new SCOREP_Config_CompilerAdapter() );
#else
    all.push_back( new SCOREP_Config_MockupAdapter( "compiler" ) );
#endif
    all.push_back( new SCOREP_Config_UserAdapter() );
    all.push_back( new SCOREP_Config_Opari2Adapter() );
#if HAVE_BACKEND( CUDA_SUPPORT )
    all.push_back( new SCOREP_Config_CudaAdapter() );
#else
    all.push_back( new SCOREP_Config_MockupAdapter( "cuda" ) );
#endif
#if HAVE_BACKEND( OPENACC_SUPPORT )
    all.push_back( new SCOREP_Config_OpenaccAdapter() );
#else
    all.push_back( new SCOREP_Config_MockupAdapter( "openacc" ) );
#endif
#if HAVE_BACKEND( OPENCL_SUPPORT )
    all.push_back( new SCOREP_Config_OpenclAdapter() );
#else
    all.push_back( new SCOREP_Config_MockupAdapter( "opencl" ) );
#endif
    all.push_back( new SCOREP_Config_PreprocessAdapter() );
#if HAVE_BACKEND( MEMORY_SUPPORT )
    all.push_back( new SCOREP_Config_MemoryAdapter() );
#else
    all.push_back( new SCOREP_Config_MockupAdapter( "memory" ) );
#endif
    all.push_back( new SCOREP_Config_LibwrapAdapter() );
    all.push_back( new SCOREP_Config_IoAdapter() );
}

void
SCOREP_Config_Adapter::fini( void )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        delete ( *i );
    }
}

void
SCOREP_Config_Adapter::printAll( void )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        ( *i )->printHelp();
    }
}

bool
SCOREP_Config_Adapter::checkAll( const std::string& arg )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->checkArgument( arg ) )
        {
            return true;
        }
    }
    return false;
}

void
SCOREP_Config_Adapter::addLibsAll( std::deque<std::string>&           libs,
                                   SCOREP_Config_LibraryDependencies& deps )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->m_is_enabled )
        {
            ( *i )->addLibs( libs, deps );
        }
    }
}

void
SCOREP_Config_Adapter::addCFlagsAll( std::string&           cflags,
                                     bool                   build_check,
                                     SCOREP_Config_Language language,
                                     bool                   nvcc )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->m_is_enabled )
        {
            ( *i )->addCFlags( cflags, build_check, language, nvcc );
        }
    }
}

void
SCOREP_Config_Adapter::addIncFlagsAll( std::string&           incflags,
                                       bool                   build_check,
                                       SCOREP_Config_Language language,
                                       bool                   nvcc )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->m_is_enabled )
        {
            ( *i )->addIncFlags( incflags, build_check, language, nvcc );
        }
    }
}

void
SCOREP_Config_Adapter::addLdFlagsAll( std::string& ldflags,
                                      bool         build_check,
                                      bool         nvcc )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->m_is_enabled )
        {
            ( *i )->addLdFlags( ldflags, build_check, nvcc );
        }
    }
}

void
SCOREP_Config_Adapter::getAllInitStructNames( std::deque<std::string>& init_structs )
{
    std::deque<SCOREP_Config_Adapter*>::iterator i;
    for ( i = all.begin(); i != all.end(); i++ )
    {
        if ( ( *i )->m_is_enabled )
        {
            ( *i )->appendInitStructName( init_structs );
        }
    }
}

SCOREP_Config_Adapter::SCOREP_Config_Adapter( const std::string& name,
                                              const std::string& library,
                                              bool               is_default )
    : m_is_enabled( is_default ),
    m_name( name ),
    m_library( library )
{
}

SCOREP_Config_Adapter::~SCOREP_Config_Adapter()
{
}

void
SCOREP_Config_Adapter::printHelp( void )
{
    std::cout << "   --" << m_name << "|--no" << m_name << "\n"
              << "              Specifies whether " << m_name << " instrumentation is used.\n"
              << "              On default " << m_name << " instrumentation is " \
              << ( m_is_enabled ? "enabled" : "disabled" ) << ".\n";
}

bool
SCOREP_Config_Adapter::checkArgument( const std::string& arg )
{
    if ( arg == "--" + m_name )
    {
        m_is_enabled = true;
        return true;
    }
    else if ( arg == "--no" + m_name )
    {
        m_is_enabled = false;
        return true;
    }
    return false;
}

void
SCOREP_Config_Adapter::addLibs( std::deque<std::string>&           libs,
                                SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "lib" + m_library + "_event" );
    deps.addDependency( "libscorep_measurement", "lib" + m_library + "_mgmt" );
}

void
SCOREP_Config_Adapter::addCFlags( std::string&           cflags,
                                  bool                   build_check,
                                  SCOREP_Config_Language language,
                                  bool                   nvcc )
{
}

void
SCOREP_Config_Adapter::addLdFlags( std::string& /* ldflags */,
                                   bool /* build_check */,
                                   bool /* nvcc */ )
{
}

void
SCOREP_Config_Adapter::addIncFlags( std::string&           incflags,
                                    bool                   build_check,
                                    SCOREP_Config_Language language,
                                    bool                   nvcc )
{
}

void
SCOREP_Config_Adapter::appendInitStructName( std::deque<std::string>& init_structs )
{
    std::string name = m_name;
    name[ 0 ] = toupper( name[ 0 ] );
    init_structs.push_back( "SCOREP_Subsystem_" + name + "Adapter" );
}

/* **************************************************************************************
 * Mockup adapter
 * *************************************************************************************/

SCOREP_Config_MockupAdapter::SCOREP_Config_MockupAdapter( const std::string& name )
    : SCOREP_Config_Adapter( name, "", false )
{
}

void
SCOREP_Config_MockupAdapter::printHelp( void )
{
}

bool
SCOREP_Config_MockupAdapter::checkArgument( const std::string& arg )
{
    if ( arg == "--" + m_name )
    {
        std::cerr << "[Score-P] ERROR: Unsupported feature '" << m_name << "' cannot be enabled by '" << arg << "'" << std::endl;
        exit( EXIT_FAILURE );
        return false;
    }

    if ( arg == "--no" + m_name )
    {
        m_is_enabled = false;
        return true;
    }

    return false;
}

void
SCOREP_Config_MockupAdapter::addLibs( std::deque<std::string>& /* libs */,
                                      SCOREP_Config_LibraryDependencies& /* deps */ )
{
}

void
SCOREP_Config_MockupAdapter::appendInitStructName( std::deque<std::string>& init_structs )
{
}

/* **************************************************************************************
 * Compiler adapter
 * *************************************************************************************/

SCOREP_Config_CompilerAdapter::SCOREP_Config_CompilerAdapter()
    : SCOREP_Config_Adapter( "compiler", "scorep_adapter_compiler", true )
{
}

bool
SCOREP_Config_CompilerAdapter::checkArgument( const std::string& arg )
{
#if HAVE_BACKEND( COMPILER_INSTRUMENTATION )
    if ( SCOREP_Config_Adapter::checkArgument( arg ) )
    {
        return true;
    }

#if HAVE_BACKEND( GCC_PLUGIN_SUPPORT )
    /* Catch any compiler plug-in args */
    if ( arg.substr( 0, 22 ) == "--compiler-plugin-arg=" )
    {
        m_cflags += "-fplugin-arg-scorep_instrument_function-" + arg.substr( 22 ) + " ";
        return true;
    }
#endif /*HAVE_BACKEND( GCC_PLUGIN_SUPPORT )*/
#endif /*HAVE_BACKEND( COMPILER_INSTRUMENTATION )*/

    return false;
}

void
SCOREP_Config_CompilerAdapter::addCFlags( std::string&           cflags,
                                          bool                   build_check,
                                          SCOREP_Config_Language language,
                                          bool /* nvcc */ )
{
    cflags += SCOREP_COMPILER_INSTRUMENTATION_CFLAGS " ";

#if HAVE_BACKEND( GCC_PLUGIN_SUPPORT )
    if ( build_check )
    {
        extern std::string path_to_binary;
        cflags += "-fplugin=" + path_to_binary + "../build-gcc-plugin/" LT_OBJDIR "scorep_instrument_function.so ";
    }
    else
    {
        cflags += "-fplugin=" SCOREP_PKGLIBDIR "/scorep_instrument_function.so ";
    }
    cflags += m_cflags;
#endif
}

void
SCOREP_Config_CompilerAdapter::addLdFlags( std::string& ldflags,
                                           bool /* build_check */,
                                           bool         nvcc )
{
    if ( nvcc )
    {
        ldflags += " " SCOREP_COMPILER_INSTRUMENTATION_LDFLAGS;
    }
    else
    {
        /*
         * Add compiler instrumentation cflags, because we encountered a
         * case on JUQUEEN with the XLC compiler, that performed
         * rebuilding of code during linking on high optimization
         * levels. If the link command does not contain compiler
         * instrumentation flags, the code was not instrumented.
         *
         * However, we got errors with PGI on Todi, because compiler instrumentation
         * functions were defined twice. Thus do not add the
         * compiler instrumentation for PGI.
         *
         * See also ticket #855.
         */
#if !SCOREP_BACKEND_COMPILER_PGI
        ldflags += " " SCOREP_COMPILER_INSTRUMENTATION_CFLAGS;
#endif
        ldflags += " " SCOREP_COMPILER_INSTRUMENTATION_LDFLAGS;
    }
}

/* **************************************************************************************
 * User adapter
 * *************************************************************************************/
SCOREP_Config_UserAdapter::SCOREP_Config_UserAdapter()
    : SCOREP_Config_Adapter( "user", "scorep_adapter_user", false )
{
}

void
SCOREP_Config_UserAdapter::addIncFlags( std::string&           cflags,
                                        bool                   build_check,
                                        SCOREP_Config_Language language,
                                        bool /* nvcc */ )
{
    if ( language == SCOREP_CONFIG_LANGUAGE_FORTRAN )
    {
#if SCOREP_BACKEND_COMPILER_IBM
        cflags += "-WF,";
#endif      // SCOREP_BACKEND_COMPILER_IBM
        cflags += "-DSCOREP_USER_ENABLE ";
    }
    else
    {
        cflags += "-DSCOREP_USER_ENABLE ";
    }
}

/* **************************************************************************************
 * Cuda adapter
 * *************************************************************************************/
SCOREP_Config_CudaAdapter::SCOREP_Config_CudaAdapter()
    : SCOREP_Config_Adapter( "cuda", "scorep_adapter_cuda", true )
{
}

void
SCOREP_Config_CudaAdapter::addLibs( std::deque<std::string>&           libs,
                                    SCOREP_Config_LibraryDependencies& deps )
{
    /* there is no libscorep_adapter_cuda_event.la, thus in case this is the
       only adapter, we need to add libscorep_measurement.la to the needed libs. */
    libs.push_back( "libscorep_measurement" );
    deps.addDependency( "libscorep_measurement", "lib" + m_library + "_mgmt" );
}

/* **************************************************************************************
 * OpenACC adapter
 * *************************************************************************************/
SCOREP_Config_OpenaccAdapter::SCOREP_Config_OpenaccAdapter()
    : SCOREP_Config_Adapter( "openacc", "scorep_adapter_openacc", true )
{
}

void
SCOREP_Config_OpenaccAdapter::addLibs( std::deque<std::string>&           libs,
                                       SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "lib" + m_library + "_event" );
    deps.addDependency( "libscorep_measurement", "lib" + m_library + "_mgmt" );
    deps.addDependency( "lib" + m_library + "_mgmt", "libscorep_alloc_metric" );
}


/* **************************************************************************************
 * OpenCL adapter
 * *************************************************************************************/
SCOREP_Config_OpenclAdapter::SCOREP_Config_OpenclAdapter()
    : SCOREP_Config_Adapter( "opencl", "scorep_adapter_opencl", true )
    , m_wrapmode( SCOREP_LIBWRAP_DEFAULT_MODE )
{
}

void
SCOREP_Config_OpenclAdapter::printHelp( void )
{
    std::cout << "   --opencl[:<wrapping mode>]|--noopencl\n"
              << "              Specifies whether opencl instrumentation is used.\n"
              << "              On default opencl instrumentation is enabled.\n"
              << "              Available values for <wrap-mode> are:\n"
#if HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
        << "               linktime (default)\n"
#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )
        << "               runtime\n"
#endif
#elif HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )
        << "               runtime (default)\n"
#endif
    ;
}

bool
SCOREP_Config_OpenclAdapter::checkArgument( const std::string& arg )
{
    if ( arg == "--opencl" )
    {
        m_is_enabled = true;
        return true;
    }

    if ( arg.substr( 0, 9 ) == ( "--opencl:" ) )
    {
        m_is_enabled = true;
        m_wrapmode   = arg.substr( 9 );

#if HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
        if ( m_wrapmode == "linktime" )
        {
            return true;
        }
#endif
#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )
        if ( m_wrapmode == "runtime" )
        {
            return true;
        }
#endif

        std::cerr << "[Score-P] ERROR: Invalid or unsupported wrapping mode for OpenCL: " << m_wrapmode << std::endl;
        exit( EXIT_FAILURE );

        return true;
    }

    if ( arg == "--noopencl" )
    {
        m_is_enabled = false;
        return true;
    }
    return false;
}

void
SCOREP_Config_OpenclAdapter::addLibs( std::deque<std::string>&           libs,
                                      SCOREP_Config_LibraryDependencies& deps )
{
    libs.push_back( "lib" + m_library + "_event_" + m_wrapmode );
    deps.addDependency( "libscorep_measurement", "lib" + m_library + "_mgmt_" + m_wrapmode );
}

void
SCOREP_Config_OpenclAdapter::addLdFlags( std::string& ldflags,
                                         bool         buildCheck,
                                         bool         nvcc )
{
    if ( m_wrapmode == "linktime" )
    {
        ldflags += get_ld_wrap_flag( "opencl", buildCheck, nvcc );
    }
}

/* **************************************************************************************
 * Preprocess adapter
 * *************************************************************************************/
SCOREP_Config_PreprocessAdapter::SCOREP_Config_PreprocessAdapter()
    : SCOREP_Config_Adapter( "preprocess", "", false )
{
}

void
SCOREP_Config_PreprocessAdapter::addCFlags( std::string&           cflags,
                                            bool                   build_check,
                                            SCOREP_Config_Language language,
                                            bool                   nvcc )
{
    if ( language == SCOREP_CONFIG_LANGUAGE_C )
    {
        cflags += SCOREP_C_NO_PREINCLUDE_FLAG " ";
    }
    if ( language == SCOREP_CONFIG_LANGUAGE_CXX )
    {
        cflags += SCOREP_CXX_NO_PREINCLUDE_FLAG " ";
    }
}

void
SCOREP_Config_PreprocessAdapter::addLibs( std::deque<std::string>&           libs,
                                          SCOREP_Config_LibraryDependencies& deps )
{
}

void
SCOREP_Config_PreprocessAdapter::appendInitStructName( std::deque<std::string>& init_structs )
{
    // Do nothing here as PreprocessAdapter is no SCOREP_Subsystem.
}


/* **************************************************************************************
 * Opari2 adapter
 * *************************************************************************************/
SCOREP_Config_Opari2Adapter::SCOREP_Config_Opari2Adapter()
    : SCOREP_Config_Adapter( "pomp", "scorep_adapter_opari2_user", false )
{
}

void
SCOREP_Config_Opari2Adapter::addIncFlags( std::string&           incflags,
                                          bool                   build_check,
                                          SCOREP_Config_Language language,
                                          bool                   nvcc )
{
    printOpariCFlags( build_check,
                      false,
                      language,
                      nvcc );
}

void
SCOREP_Config_Opari2Adapter::addCFlags( std::string&           cflags,
                                        bool                   build_check,
                                        SCOREP_Config_Language language,
                                        bool                   nvcc )
{
    printOpariCFlags( build_check,
                      true,
                      language,
                      nvcc );
}


void
SCOREP_Config_Opari2Adapter::appendInitStructName( std::deque<std::string>& init_structs )
{
    init_structs.push_back( "SCOREP_Subsystem_Opari2UserAdapter" );
}


void
SCOREP_Config_Opari2Adapter::printOpariCFlags( bool                   build_check,
                                               bool                   with_cflags,
                                               SCOREP_Config_Language language,
                                               bool                   nvcc )
{
    static bool printed_once = false;
    if ( !printed_once )
    {
        printed_once = true;

#if !HAVE( EXTERNAL_OPARI2 )
        if ( build_check )
        {
            extern std::string path_to_src;
            std::cout << "-I" + path_to_src + "vendor/opari2/include ";
            std::cout.flush();
            return;
        }
#endif

        std::string opari_config = "`" OPARI_CONFIG " --cflags";
#if ( !defined HAVE_BACKEND_OPARI2_REVISION ) || ( HAVE_BACKEND_OPARI2_REVISION < 1068 )
        if ( with_cflags )
        {
            opari_config += "=" SCOREP_COMPILER_TYPE;

            if ( language == SCOREP_CONFIG_LANGUAGE_FORTRAN )
            {
                opari_config += " --fortran";
            }
        }
#endif
        opari_config += "` ";

        if ( nvcc )
        {
            opari_config = "printf -- \"-Xcompiler %s \" " + opari_config;
        }
        else
        {
            opari_config = "printf -- \"%s \" " + opari_config;
        }

        int return_value = system( opari_config.c_str() );
        if ( return_value != 0 )
        {
            std::cerr << "[Score-P] ERROR: Execution failed: " << opari_config.c_str() << std::endl;
            exit( EXIT_FAILURE );
        }
        std::cout << " ";
        std::cout.flush();
    }
}

/* **************************************************************************************
 * Memory adapter
 * *************************************************************************************/
SCOREP_Config_MemoryAdapter::SCOREP_Config_MemoryAdapter()
    : SCOREP_Config_Adapter( "memory", "scorep_adapter_memory", false )
{
}

void
SCOREP_Config_MemoryAdapter::printHelp( void )
{
    std::cout << "   --" << m_name << "=<api list>|--no" << m_name << "\n"
              << "              Specifies whether memory usage recording is used.\n"
              << "              On default memory usage recording is " \
              << ( m_is_enabled ? "enabled" : "disabled" ) << ".\n"
              << "              The following memory interfaces may be recorded:\n"
              << "               libc:\n"
              << "                malloc,realloc,calloc,free,memalign,posix_memalign,valloc\n"
              << "               libc11:\n"
              << "                aligned_alloc\n"
              << "               c++L32|c++L64:\n"
              << "                new,new[],delete,delete[] (IA-64 C++ ABI)\n"
              << "               pgCCL32|pgCCL64:\n"
              << "                new,new[],delete,delete[] (old PGI/EDG C++ ABI)\n"
              << "               hbwmalloc:\n"
              << "                hbw_malloc,hbw_realloc,hbw_calloc,hbw_free,hbw_posix_memalign,\n"
              << "                hbw_posix_memalign_psize (Intel KNL MCDRAM API)\n";
}

bool
SCOREP_Config_MemoryAdapter::checkArgument( const std::string& arg )
{
    if ( arg == "--" + m_name )
    {
        m_is_enabled = true;
        return true;
    }

    if ( arg.substr( 0, 9 ) == ( "--" + m_name + "=" ) )
    {
        m_is_enabled = true;
        std::deque<std::string> categories = string_to_deque( arg.substr( 9 ), "," );
        m_categories.insert( categories.begin(), categories.end() );
        return true;
    }

    if ( arg == "--no" + m_name )
    {
        m_is_enabled = false;
        return true;
    }
    return false;
}

void
SCOREP_Config_MemoryAdapter::addLibs( std::deque<std::string>&           libs,
                                      SCOREP_Config_LibraryDependencies& deps )
{
    if ( m_categories.count( "libc" ) )
    {
        libs.push_back( "lib" + m_library + "_event_libc" );
    }

    if ( m_categories.count( "libc11" ) )
    {
        libs.push_back( "lib" + m_library + "_event_libc11" );
    }

    if ( m_categories.count( "c++L32" ) || m_categories.count( "c++L64" ) )
    {
        libs.push_back( "lib" + m_library + "_event_cxx" );
    }

    if ( m_categories.count( "c++L32" ) )
    {
        libs.push_back( "lib" + m_library + "_event_cxx_L32" );
    }

    if ( m_categories.count( "c++L64" ) )
    {
        libs.push_back( "lib" + m_library + "_event_cxx_L64" );
    }

    if ( m_categories.count( "c++14L32" ) )
    {
        libs.push_back( "lib" + m_library + "_event_cxx14_L32" );
    }

    if ( m_categories.count( "c++14L64" ) )
    {
        libs.push_back( "lib" + m_library + "_event_cxx14_L64" );
    }

    if ( m_categories.count( "pgCCL32" ) || m_categories.count( "pgCCL64" ) )
    {
        libs.push_back( "lib" + m_library + "_event_pgCC" );
    }

    if ( m_categories.count( "pgCCL32" ) )
    {
        libs.push_back( "lib" + m_library + "_event_pgCC_L32" );
    }

    if ( m_categories.count( "pgCCL64" ) )
    {
        libs.push_back( "lib" + m_library + "_event_pgCC_L64" );
    }

    if ( m_categories.count( "hbwmalloc" ) )
    {
        libs.push_back( "lib" + m_library + "_event_hbwmalloc" );
    }

    deps.addDependency( "libscorep_measurement", "lib" + m_library + "_mgmt" );
    deps.addDependency( "lib" + m_library + "_mgmt", "libscorep_alloc_metric" );
}

void
SCOREP_Config_MemoryAdapter::addLdFlags( std::string& ldflags,
                                         bool         build_check,
                                         bool         nvcc )
{
#if SCOREP_BACKEND_COMPILER_CRAY
    ldflags += " -h system_alloc";
#endif

    if ( m_categories.count( "libc" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap_malloc,"
                   "-wrap,malloc,"
                   "-wrap,realloc,"
                   "-wrap,calloc,"
                   "-wrap,free,"
                   "-wrap,memalign,"
                   "-wrap,posix_memalign,"
                   "-wrap,valloc";
    }

    if ( m_categories.count( "libc11" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap_aligned_alloc,"
                   "-wrap,aligned_alloc";
    }

    if ( m_categories.count( "c++L32" ) || m_categories.count( "c++L64" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap__ZdlPv,"
                   "-wrap,_ZdlPv,"
                   "-wrap,_ZdaPv";
    }

    if ( m_categories.count( "c++L32" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap__Znwj,"
                   "-wrap,_Znwj,"
                   "-wrap,_Znaj";
    }

    if ( m_categories.count( "c++L64" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap__Znwm,"
                   "-wrap,_Znwm,"
                   "-wrap,_Znam";
    }

    if ( m_categories.count( "c++14L32" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap__ZdlPvj,"
                   "-wrap,_ZdlPvj,"
                   "-wrap,_ZdaPvj";
    }

    if ( m_categories.count( "c++14L64" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap__ZdlPvm,"
                   "-wrap,_ZdlPvm,"
                   "-wrap,_ZdaPvm";
    }

    if ( m_categories.count( "pgCCL32" ) || m_categories.count( "pgCCL64" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap___dl__FPv,"
                   "-wrap,__dl__FPv,"
                   "-wrap,__dla__FPv";
    }

    if ( m_categories.count( "pgCCL32" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap___nw__FUi,"
                   "-wrap,__nw__FUi,"
                   "-wrap,__nwa__FUi";
    }

    if ( m_categories.count( "pgCCL64" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap___nw__FUl,"
                   "-wrap,__nw__FUl,"
                   "-wrap,__nwa__FUl";
    }

    if ( m_categories.count( "hbwmalloc" ) )
    {
        ldflags += " -Wl,"
                   "--undefined,__wrap_hbw_malloc,"
                   "-wrap,hbw_malloc,"
                   "-wrap,hbw_realloc,"
                   "-wrap,hbw_calloc,"
                   "-wrap,hbw_free,"
                   "-wrap,hbw_posix_memalign,"
                   "-wrap,hbw_posix_memalign_psize";
    }
}

/* **************************************************************************************
 * Libwrap adapter
 * *************************************************************************************/
SCOREP_Config_LibwrapAdapter::SCOREP_Config_LibwrapAdapter()
    : SCOREP_Config_Adapter( "libwrap", "", true )
{
}

void
SCOREP_Config_LibwrapAdapter::printHelp( void )
{
    std::cout << "   --libwrap=<wrap-mode>:<libwrap-anchor-file>\n"
              << "              Uses the specified library wrapper.\n";
}

bool
SCOREP_Config_LibwrapAdapter::checkArgument( const std::string& arg )
{
    if ( arg.substr( 0, 10 ) == "--libwrap=" )
    {
        std::string libwrap = arg.substr( 10 );

        std::string wrapmode;
        if ( libwrap.compare( 0, 9, "linktime:" ) == 0 )
        {
            wrapmode = "linktime";
            libwrap.erase( 0, 9 );
        }
        else if ( libwrap.compare( 0, 8, "runtime:" ) == 0 )
        {
            wrapmode = "runtime";
            libwrap.erase( 0, 8 );
        }
        else
        {
            std::cerr << "[Score-P] ERROR: Missing libwrap mode in: '" << arg << "'" << std::endl;
            exit( EXIT_FAILURE );
        }

        m_wrappers.insert( std::make_pair( wrapmode, libwrap ) );
        m_is_enabled = true;
        return true;
    }
    return false;
}

void
SCOREP_Config_LibwrapAdapter::addLibs( std::deque<std::string>&           libs,
                                       SCOREP_Config_LibraryDependencies& deps )
{
    for ( std::set<std::pair<std::string, std::string> >::const_iterator it = m_wrappers.begin(); it != m_wrappers.end(); ++it )
    {
        const std::string& wrapmode = it->first;
        const std::string& libwrap  = it->second;
        /* we point to <prefix>/share/scorep/<name>.libwrap */
        std::string libdir = join_path( extract_path( extract_path( extract_path( libwrap ) ) ), "lib" SCOREP_BACKEND_SUFFIX );
        std::string name   = remove_extension( remove_path( libwrap ) );

        std::deque<std::string> empty;
        std::deque<std::string> dependency_las;
        dependency_las.push_back( "libscorep_measurement" );

#if HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
        if ( exists_file( join_path( libdir, "libscorep_libwrap_" + name + "_linktime.la" ) ) )
        {
            deps.insert( "libscorep_libwrap_" + name + "_linktime",
                         "",
                         libdir,
                         empty, empty, empty,
                         dependency_las );
        }
        else
#endif
        if ( wrapmode == "linktime" )
        {
            std::cerr << "[Score-P] ERROR: Library wrapping mode 'linktime' not supported by this installation." << std::endl;
            exit( EXIT_FAILURE );
        }

#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )
        if ( exists_file( join_path( libdir, "libscorep_libwrap_" + name + "_runtime.la" ) ) )
        {
            deps.insert( "libscorep_libwrap_" + name + "_runtime",
                         "",
                         libdir,
                         empty, empty, empty,
                         dependency_las );
        }
        else
#endif
        if ( wrapmode == "runtime" )
        {
            std::cerr << "[Score-P] ERROR: Library wrapping mode 'runtime' not supported by this installation." << std::endl;
            exit( EXIT_FAILURE );
        }

        libs.push_back( "libscorep_libwrap_" + name + "_" + wrapmode );
    }
}

void
SCOREP_Config_LibwrapAdapter::addLdFlags( std::string& ldflags,
                                          bool /* buildCheck */,
                                          bool         nvcc )
{
    for ( std::set<std::pair<std::string, std::string> >::const_iterator it = m_wrappers.begin(); it != m_wrappers.end(); ++it )
    {
        const std::string& wrapmode = it->first;
        const std::string& libwrap  = it->second;

        if ( wrapmode == "linktime" )
        {
            /* libwrap points to the .libwrap file */
            ldflags += get_ld_wrap_flag( remove_extension( libwrap ), nvcc );
        }
    }
}

void
SCOREP_Config_LibwrapAdapter::appendInitStructName( std::deque<std::string>& init_structs )
{
}
/* **************************************************************************************
 * I/O wrapping adapter
 * *************************************************************************************/
SCOREP_Config_IoAdapter::SCOREP_Config_IoAdapter()
    : SCOREP_Config_Adapter( "io", "", false )
{
#if HAVE_BACKEND( POSIX_IO_SUPPORT )
    m_supported_ios.insert( SCOREP_Config_SupportedIosV(
                                "posix",
                                SCOREP_Config_SupportedIo( "Posix", "posix_io", "posix_io" ) ) );
#endif
}

void
SCOREP_Config_IoAdapter::printHelp( void )
{
    std::cout << "   --io=[<wrap-mode>:]<paradigm,...>|--noio\n"
              << "              Specifies whether I/O recording is used.\n"
              << "              On default I/O recording is disabled.\n"
              << "              The following I/O paradigms may be recorded:\n"
              << "               none\n";
    for ( SCOREP_Config_SupportedIosCIT it = m_supported_ios.begin(); it != m_supported_ios.end(); ++it )
    {
        std::cout << "               " << it->first << "\n";
    }
}

bool
SCOREP_Config_IoAdapter::checkArgument( const std::string& arg )
{
    if ( arg.substr( 0, 5 ) == ( "--io=" ) )
    {
        std::string io = arg.substr( 5 );

        std::string wrapmode = "linktime";
        if ( io.compare( 0, 9, "linktime:" ) == 0 )
        {
            wrapmode = "linktime";
            io.erase( 0, 9 );
        }
        else if ( io.compare( 0, 8, "runtime:" ) == 0 )
        {
            wrapmode = "runtime";
            io.erase( 0, 8 );
        }

        std::deque<std::string> ios = string_to_deque( io, "," );

        for ( std::deque<std::string>::const_iterator it = ios.begin(); it != ios.end(); ++it )
        {
            if ( *it == "none" )
            {
                m_selected_ios.clear();
                continue;
            }

            if ( !m_supported_ios.count( *it ) )
            {
                std::cerr << "ERROR: I/O paradigm " << *it << " not supported by this Score-P installation." << std::endl;
                exit( EXIT_FAILURE );
            }
            m_selected_ios[ *it ] = wrapmode;
        }

        m_is_enabled = m_selected_ios.size() != 0;

        return true;
    }
    if ( arg == "--noio" )
    {
        m_is_enabled = false;
        return true;
    }
    return false;
}

void
SCOREP_Config_IoAdapter::addLibs( std::deque<std::string>&           libs,
                                  SCOREP_Config_LibraryDependencies& deps )
{
    if ( !m_is_enabled )
    {
        return;
    }

    for ( SCOREP_Config_SupportedIosCIT it = m_supported_ios.begin(); it != m_supported_ios.end(); ++it )
    {
        if ( !m_selected_ios.count( it->first ) )
        {
            continue;
        }

        libs.push_back( "libscorep_adapter_" + it->second.m_lib_name + "_event_" + m_selected_ios[ it->first ] );
        deps.addDependency( "libscorep_measurement", "libscorep_adapter_" + it->second.m_lib_name + "_mgmt_" + m_selected_ios[ it->first ] );
    }
}

void
SCOREP_Config_IoAdapter::addLdFlags( std::string& ldflags,
                                     bool         buildCheck,
                                     bool         nvcc )
{
    if ( !m_is_enabled )
    {
        return;
    }

    for ( SCOREP_Config_SupportedIosCIT it = m_supported_ios.begin(); it != m_supported_ios.end(); ++it )
    {
        if ( m_selected_ios.count( it->first ) && m_selected_ios[ it->first ] == "linktime" )
        {
            for ( std::vector<std::string>::const_iterator iter = it->second.m_wrap_names.begin();
                  iter != it->second.m_wrap_names.end();
                  ++iter )
            {
                ldflags += get_ld_wrap_flag( *iter, buildCheck, nvcc );
            }
        }
    }
}

void
SCOREP_Config_IoAdapter::appendInitStructName( std::deque<std::string>& initStructs )
{
    if ( !m_is_enabled )
    {
        return;
    }

    initStructs.push_back( "SCOREP_Subsystem_IoManagement" );

    for ( SCOREP_Config_SupportedIosCIT it = m_supported_ios.begin(); it != m_supported_ios.end(); ++it )
    {
        if ( m_selected_ios.count( it->first ) )
        {
            initStructs.push_back( "SCOREP_Subsystem_" + it->second.m_subsystem_name + "IoAdapter" );
        }
    }
}
