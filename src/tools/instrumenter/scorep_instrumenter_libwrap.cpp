/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Implements the class for library wrapping.
 */

#include <config.h>
#include <scorep_config_tool_backend.h>
#include "scorep_instrumenter_libwrap.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"

#include <scorep_tools_utils.hpp>
#include <scorep_tools_dir.hpp>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_LibwrapAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_LibwrapAdapter::SCOREP_Instrumenter_LibwrapAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_LIBWRAP, "libwrap" )
{
    const char* libwrap_path_env = getenv( "SCOREP_LIBWRAP_PATH" );
    if ( libwrap_path_env )
    {
        m_path = split_string( libwrap_path_env, ":" );
    }

    /*
     * Add Score-P's own install path, do not use the DATADIR, as this may be
     * changed by the configuring of Score-P (--datadir flag) and thus does not
     * conform to the install layout of scorep-libwrap-init
     */
    m_path.push_back( SCOREP_PREFIX );
}


void
SCOREP_Instrumenter_LibwrapAdapter::printHelp( void )
{
    /** @todo Add a note to 'scorep-libwrap-init', how to generate a wrapper */
    std::cout
        << "\
  --libwrap=[<wrap-mode>:](<wrapper-name>...|<path-to-wrapper>)\n\
                  Enables user library wrapping for specified libraries.\n\
                  <wrap-mode> may be 'linktime' or 'runtime'.\n\
                  The default is the first supported mode in the above order.\n\
                  <wrapper-name>... is a comma-separated list of library wrappers\n\
                  which will be looked up in the paths of the colon-separated\n\
                  'SCOREP_LIBWRAP_PATH' environment variable and in the installation\n\
                  directory of Score-P. <path-to-wrapper> is a full path to the\n\
                  .libwrap anchor file.\n\
                  Use 'scorep-info libwrap-summary' for a more detailed listing\n\
                  of available library wrappers."
        << std::endl;

    if ( !SCOREP_Tools_Dir::supported() )
    {
        std::cout
            << "\
                  Directory listing not supported by this installation of\n\
                  Score-P. Please manually look for *.libwrap files in the\n\
                  following directories to find out about installed wrappers:"
            << std::endl;
    }

    /* Print a message, if we have no library wrappers at all */
    bool have_wrappers = false;

    std::vector<std::string>::size_type i;
    std::set<std::string>               known_paths;
    for ( i = 0; i < m_path.size(); ++i )
    {
        std::pair<std::set<std::string>::iterator, bool> result =
            known_paths.insert( simplify_path( m_path[ i ] ) );
        if ( !result.second )
        {
            /* Ignore duplicate PATH entry */
            continue;
        }

        std::string path = join_path( join_path( m_path[ i ], "share" ), "scorep" );

        if ( !SCOREP_Tools_Dir::supported() )
        {
            std::cout << "                   " << path << std::endl;
            continue;
        }

        bool             empty = true;
        SCOREP_Tools_Dir dir( path, SCOREP_Tools_Dir::NO_DOT_ENTRIES | SCOREP_Tools_Dir::NO_DIRECTORIES );
        while ( dir.good() )
        {
            SCOREP_Tools_Dirent dirent;
            dir >> dirent;

            std::string::size_type pos = dirent.name.rfind( ".libwrap" );
            if ( std::string::npos == pos )
            {
                // extension not found
                continue;
            }
            if ( pos != ( dirent.name.size() - 8 ) )
            {
                // not as an extension
                continue;
            }

            if ( !have_wrappers )
            {
                std::cout
                    << "\
                  Available library wrappers:"
                    << std::endl;
            }
            /* We have at least one library wrapper in the path, remember */
            have_wrappers = true;

            if ( empty )
            {
                std::cout << "                   " << m_path[ i ] << ":" << std::endl;
                empty = false;
            }

            Wrapper wrapper( join_path( path, dirent.name ) );
            std::cout << "                    " << wrapper.m_name;
            if ( 1 == wrapper.m_values.count( "LIBWRAP_NAME" ) && wrapper.m_values[ "LIBWRAP_NAME" ] != wrapper.m_name )
            {
                std::cout << " (" << wrapper.m_values[ "LIBWRAP_NAME" ] << ")";
            }
            std::cout << std::endl;
        }
    }

    if ( SCOREP_Tools_Dir::supported() && !have_wrappers )
    {
        std::cout
            << "\
                  No library wrappers available. Please check 'SCOREP_LIBWRAP_PATH'."
            << std::endl;
    }
}

void
SCOREP_Instrumenter_LibwrapAdapter::add_libwrap( const std::string& wrapmode,
                                                 const std::string& arg,
                                                 const std::string& anchor )
{
    Wrapper wrapper( anchor );
    if ( wrapmode == "linktime" )
    {
        m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING );
    }
    if ( wrapmode == "runtime" && wrapper.m_dlopen_libs.empty() )
    {
        std::cerr << "[Score-P] ERROR: Library wrapper '" << arg << "' does not support runtime wrapping. Use linktime wrapping instead." << std::endl;
        exit( EXIT_FAILURE );
    }
    m_wrappers.push_back( std::make_pair( wrapmode, wrapper ) );
}

bool
SCOREP_Instrumenter_LibwrapAdapter::checkOption( const std::string& arg )
{
    if ( arg.size() > 10 && arg.substr( 0, 10 ) == "--libwrap=" )
    {
        std::string libwrap = arg.substr( 10 );

        if ( libwrap == "help" )
        {
            printHelp();
            std::cout << std::endl;
            std::cout << "Type 'scorep --help' to get the full list of possible options" << std::endl;
            exit( EXIT_SUCCESS );
        }

        std::string wrapmode = SCOREP_LIBWRAP_DEFAULT_MODE;
        std::string anchor;
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

        if ( libwrap.find( "/" ) != std::string::npos )
        {
            if ( !exists_file( libwrap ) )
            {
                std::cerr << "[Score-P] ERROR: Library wrapper does not exists: '" << libwrap << "'" << std::endl;
                exit( EXIT_FAILURE );
            }
            add_libwrap( wrapmode, libwrap, canonicalize_path( libwrap ) );
        }
        else
        {
            /*
             * Library wrapper names are not allowed to have ',' in it, thus
             * we can support a comma-separated list here
             */
            std::vector<std::string> libwraps = split_string( libwrap, "," );

            std::vector<std::string>::size_type i;
            for ( i = 0; i < libwraps.size(); ++i )
            {
                libwrap = libwraps[ i ];

                /* search path */
                std::vector<std::string>::size_type j;
                for ( j = 0; j < m_path.size(); ++j )
                {
                    std::string full_path = join_path( join_path( join_path( m_path[ i ], "share" ), "scorep" ), libwrap + ".libwrap" );
                    if ( exists_file( full_path ) )
                    {
                        add_libwrap( wrapmode, libwrap, full_path );
                        break;
                    }
                }
                if ( j == m_path.size() )
                {
                    std::cerr << "[Score-P] ERROR: Library wrapper not found in SCOREP_LIBWRAP_PATH or in Score-P installation: '" << libwrap << "'" << std::endl;
                    exit( EXIT_FAILURE );
                }
            }
        }

        return true;
    }

    if ( arg == "--libwrap" || arg == "--libwrap=" )
    {
        std::cerr << "[Score-P] ERROR: Missing argument to '--libwrap'" << std::endl;
        exit( EXIT_FAILURE );
    }

    return false;
}

std::string
SCOREP_Instrumenter_LibwrapAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    std::string result;
    for ( std::vector<int>::size_type i = 0; i < m_wrappers.size(); ++i )
    {
        result += " --libwrap=" + m_wrappers[ i ].first + ":" + m_wrappers[ i ].second.getAnchor();
    }
    return result;
}

bool
SCOREP_Instrumenter_LibwrapAdapter::isInterpositionLibrary( const std::string& libraryName )
{
    for ( std::vector<int>::size_type i = 0; i < m_wrappers.size(); ++i )
    {
        const Wrapper& wrapper = m_wrappers[ i ].second;
        if ( wrapper.m_libs.count( libraryName ) != 0 )
        {
            return true;
        }
    }
    return false;
}

SCOREP_Instrumenter_LibwrapAdapter::Wrapper::Wrapper( const std::string& anchor )
{
    /* we point to <prefix>/share/scorep/<name>.libwrap */
    m_prefix = extract_path( extract_path( extract_path( anchor ) ) );
    m_name   = remove_extension( remove_path( anchor ) );
    readConfigFile( anchor );
}

std::string
SCOREP_Instrumenter_LibwrapAdapter::Wrapper::getAnchor( void )
{
    return join_path( join_path( join_path( m_prefix, "share" ), "scorep" ), m_name + ".libwrap" );
}

void
SCOREP_Instrumenter_LibwrapAdapter::Wrapper::set_value( const std::string& key,
                                                        const std::string& value )
{
    m_values.insert( std::make_pair( key, value ) );
    if ( key == "LIBWRAP_LIBS" )
    {
        std::vector<std::string> libs = split_string( value, " " );
        for ( std::vector<std::string>::size_type i = 0; i < libs.size(); ++i )
        {
            const std::string& lib = libs[ i ];
            if ( lib.substr( 0, 2 ) == "-l" )
            {
                m_libs.insert( lib.substr( 2 ) );
            }
            else if ( is_library( lib ) )
            {
                m_libs.insert( get_library_name( lib ) );
            }
        }
    }
    else if ( key == "LIBWRAP_DLOPEN_LIBS" )
    {
        m_dlopen_libs = split_string( value, " " );
    }
}
