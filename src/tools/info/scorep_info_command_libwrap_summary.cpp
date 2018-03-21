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
 */

#include <config.h>

#include "scorep_info_command_libwrap_summary.hpp"

#include <iostream>
#include <cstdlib>
#include <set>

#include <scorep_config_tool_backend.h>

#include <scorep_tools_utils.hpp>
#include <scorep_tools_dir.hpp>
#include <scorep_tools_config_parser.hpp>

#include "scorep_info.hpp"

enum
{
    EXIT_INVALID_WRAPPER = 2,
    EXIT_WRAPPER_NOT_FOUND
};

SCOREP_Info_Command_LibwrapSummary::SCOREP_Info_Command_LibwrapSummary()
    : SCOREP_Info_Command( "libwrap-summary",
                           "Shows known library wrappers available to this Score-P installation." )
{
}

SCOREP_Info_Command_LibwrapSummary::~SCOREP_Info_Command_LibwrapSummary()
{
}

void
SCOREP_Info_Command_LibwrapSummary::printHelp( bool full )
{
    SCOREP_Info_Command::printHelp( full );
    if ( full )
    {
        std::cout << "    Info command options:\n"
                  << "      --build       Shows detailed information about how the library wrapper\n"
                  << "                    was built.\n"
                  << std::endl;
    }
}

struct Wrapper
    : private SCOREP_Tools_ConfigParser
{
    Wrapper( const std::string& anchor )
    {
        m_share = extract_path( anchor );
        m_name  = remove_extension( remove_path( anchor ) );
        readConfigFile( anchor );
    }

    void
    set_value( const std::string& key,
               const std::string& value )
    {
        m_values.insert( std::make_pair( key, value ) );
    }

    std::string
    getFile( const std::string& extension ) const
    {
        return join_path( m_share, m_name + extension );
    }

    int
    display( const std::string& path,
             bool               withSummary ) const
    {
        std::cout << m_name << std::endl;

        if ( m_values.find( "LIBWRAP_NAME" ) == m_values.end() )
        {
            std::cerr << "ERROR: Cannot find entry LIBWRAP_NAME in anchor file of library wrapper " << m_name << "." << std::endl;
            std::cerr << "       The library wrapper is probably corrupt." << std::endl;
            return EXIT_INVALID_WRAPPER;
        }

        std::cout << "  Display name:\n    " << m_values.find( "LIBWRAP_NAME" )->second << std::endl;
        std::cout << "  Prefix:\n    " << path << std::endl;
        if ( withSummary )
        {
            std::string cat_command( "cat " + getFile( ".summary" ) + " 2>/dev/null | sed -e 's/^/  /'" );
            int         ret = system( cat_command.c_str() );
            if ( ret != 0 )
            {
                std::cerr << "ERROR: Cannot execute \"" << cat_command << "\"." << std::endl;
                std::cerr << "       The library wrapper is probably corrupt." << std::endl;
                return EXIT_INVALID_WRAPPER;
            }
        }
        std::cout << std::endl;

        return EXIT_SUCCESS;
    }

    std::string                        m_share;
    std::string                        m_name;
    std::map<std::string, std::string> m_values;
};

int
SCOREP_Info_Command_LibwrapSummary::run( const std::vector<std::string>& args )
{
    bool   with_build = false;
    size_t args_consumed;
    for ( args_consumed = 0; args_consumed < args.size(); ++args_consumed )
    {
        std::string arg( args[ args_consumed ] );
        if ( arg == "--build" )
        {
            with_build = true;
            continue;
        }

        if ( arg == "--" )
        {
            ++args_consumed;
            break;
        }

        if ( 0 == std::string( arg ).compare( 0, 2, "--" ) )
        {
            std::cerr << "ERROR: Invalid option for info command "
                      << "'" << m_command << "': '" << arg << "'" << std::endl;
            return EXIT_FAILURE;
        }

        break;
    }

    if ( args.size() - args_consumed > 1 )
    {
        std::cerr << "ERROR: Unexpected arguments for "
                  << "'" << m_command << "': "
                  << join_to_string( args.begin() + args_consumed + 1, args.end(),
                           "'", "'", "', '" )
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::string libwrap;
    if ( args.size() - args_consumed == 1 )
    {
        libwrap = *args.rbegin();
    }

    std::vector<std::string> libwrap_path;
    const char*              libwrap_path_env = getenv( "SCOREP_LIBWRAP_PATH" );
    if ( libwrap_path_env )
    {
        libwrap_path = split_string( libwrap_path_env, ":" );
    }

    /*
     * Add Score-P's own install path, do not use the DATADIR, as this may be
     * changed by the configuring of Score-P (--datadir flag) and thus does not
     * conform to the install layout of scorep-libwrap-init
     */
    libwrap_path.push_back( SCOREP_PREFIX );

    /* Print a message, if we have no library wrappers at all */
    bool have_wrappers = false;

    if ( !SCOREP_Tools_Dir::supported() && !libwrap.size() )
    {
        std::cout
            << "\
WARNING: Directory listing not supported by this installation of Score-P.\n\
INFO:    Please manually look for *.libwrap files in the following directories\n\
         to find out about installed wrappers:"
            << std::endl;
    }

    std::vector<std::string>::size_type i;
    std::set<std::string>               known_paths;
    for ( i = 0; i < libwrap_path.size(); ++i )
    {
        std::pair<std::set<std::string>::iterator, bool> result =
            known_paths.insert( simplify_path( libwrap_path[ i ] ) );
        if ( !result.second )
        {
            /* Ignore duplicate PATH entry */
            continue;
        }

        std::string path = simplify_path( join_path( join_path( libwrap_path[ i ], "share" ), "scorep" ) );
        if ( libwrap.size() )
        {
            std::string full = join_path( path, libwrap + ".libwrap" );
            if ( !exists_file( full ) )
            {
                continue;
            }
            const Wrapper wrapper( full );
            return wrapper.display( libwrap_path[ i ], with_build );
        }

        if ( !SCOREP_Tools_Dir::supported() )
        {
            std::cout << "         " << path << std::endl;
            continue;
        }

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

            /* We have at least one library wrapper in the path, remember */
            have_wrappers = true;

            const Wrapper wrapper( join_path( path, dirent.name ) );
            int           ret = wrapper.display( libwrap_path[ i ], with_build );
            if ( EXIT_SUCCESS != ret )
            {
                return ret;
            }
        }
    }

    if ( libwrap.size() )
    {
        std::cerr << "ERROR: Could not find library wrapper '" << libwrap << "'" << std::endl;
        /* Use not EXIT_FAILURE, as we do not want the usage printed. */
        return EXIT_WRAPPER_NOT_FOUND;
    }

    if ( SCOREP_Tools_Dir::supported() && !have_wrappers )
    {
        std::cerr << "WARNING: No library wrappers available. Please check 'SCOREP_LIBWRAP_PATH'." << std::endl;
        /* This is not an error */
    }

    return EXIT_SUCCESS;
}
