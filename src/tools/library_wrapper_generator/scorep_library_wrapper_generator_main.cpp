/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015-2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Wrapper generator for libraries
 */

#include <config.h>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <getopt.h>
#include <assert.h>
#include <sys/stat.h>

#include <SCOREP_Filter.h>
#include <UTILS_IO.h>

#include "scorep_library_wrapper_generator.hpp"

#include <scorep_tools_utils.hpp>

using namespace std;

/** @def WORK_DIR_SIZE length for buffer storing the current working directory */
#define WORK_DIR_SIZE 1024

/**
 * Contains the name of the tool for help output
 */
static const string tool_name = "scorep-library-wrapper-generator";

/**
 * Prints the common part of all help messages.
 */
static void
print_common_message_part()
{
    cout << endl
         << "This is the Score-P library wrapper generator tool. The usage is:" << endl
         << tool_name << " -n <name> -h <headerfile> [-l <libraries>] [-f <filterfile>] [-o <output>] [-x <language>] [-i <include-header>] [<options>]" << endl
         << endl;
}

/**
 * Prints a short usage message.
 */
static void
print_short_usage()
{
    print_common_message_part();
    cout << "To print out more detailed help information on available parameters, type" << endl
         << tool_name << " --help" << endl
         << endl;
}

/**
 * Prints detailed usage message.
 */
static void
print_help()
{
    print_common_message_part();
    cout << "Options are:" << endl
         << "--help            Show this message" << endl
         << "--verbose | -v    Increase verbose mode" << endl
         << "--quiet           Decrease verbose mode" << endl
         << "" << endl
         << "  language can be c or c++ optionally followed by the language standard (i.e., c++11)" << endl
         << endl;
}

enum generation_mode
{
    MODE_FUNCTION_LIST,
    MODE_ADAPTER_CODE,
    MODE_WRAP_FLAGS,
    MODE_NVCC_WRAP_FLAGS
};

static string
create_result_file_name( const string&   name,
                         const string&   language,
                         generation_mode mode );


static bool
ask_user( const string& filename );

int
main( int   argc,
      char* argv[] )
{
    generator_configuration config;
    config.verbose                           = 0;
    config.create_internal_wrapper_code_file = false;
    config.overwrite                         = false;
    config.filter                            = SCOREP_Filter_New();

    /* current argument */
    string arg = "";
    /* next argument */
    string next_arg = "";
    for ( int i = 1; i < argc; i++ )
    {
        arg      = argv[ i ];
        next_arg = ( i + 1 < argc ? argv[ i + 1 ] : "" );

        if ( arg == "-l" )
        {
            vector<string> args = split_string( next_arg, " " );
            config.library_names.insert(
                config.library_names.end(),
                args.begin(),
                args.end() );
            i++;
        }
        else if ( arg == "-h" )
        {
            config.header_file = next_arg;
            i++;
            if ( !exists_file( config.header_file ) )
            {
                cerr << "ERROR: given header file '"
                     << config.header_file
                     << "' does not exist." << endl;
                return EXIT_FAILURE;
            }
        }
        else if ( arg == "-f" )
        {
            config.filter_file_name = next_arg;
            i++;
            if ( !exists_file( config.filter_file_name ) )
            {
                cerr << "ERROR: given filter file '"
                     << config.header_file
                     << "' does not exist." << endl;
                return EXIT_FAILURE;
            }
        }
        else if ( arg == "-x" )
        {
            config.language = next_arg;
            if ( next_arg.substr( 0, 3 ) == "c++" )
            {
                if ( next_arg.size() == 5 )
                {
                    config.language_standard = next_arg;
                    config.language          = "c++";
                }
            }
            else if ( next_arg.substr( 0, 1 ) == "c" )
            {
                if ( next_arg.size() == 3 )
                {
                    config.language_standard = next_arg;
                    config.language          = "c";
                }
            }
            i++;
        }
        else if ( arg == "-n" )
        {
            config.wrapper_name = next_arg;
            i++;
        }
        else if ( arg == "-d" )
        {
            config.display_name = next_arg;
            i++;
        }
        else if ( arg == "-o" )
        {
            config.output_directory = next_arg;
            i++;
        }
        else if ( arg == "-i" )
        {
            config.include_statement = next_arg;
            i++;
        }
        else if ( arg == "--variadic-is-void" )
        {
            vector<string> args = split_string( next_arg, " " );
            config.variadic_is_void.insert( args.begin(), args.end() );
            i++;
        }
        else if ( arg == "--ellipsis-mapping" )
        {
            vector<string> args = split_string( next_arg, " " );
            for ( vector<string>::const_iterator it = args.begin(); it != args.end(); ++it )
            {
                string::size_type colon = ( *it ).find( ":" );
                if ( colon == string::npos )
                {
                    cerr << "ERROR: invalid value for '--ellipsis-mapping': '" << *it << "'" << endl;
                    return EXIT_FAILURE;
                }
                config.ellipsis_mapping.insert(
                    make_pair( ( *it ).substr( 0, colon ),
                               ( *it ).substr( colon + 1 ) ) );
            }
            i++;
        }
        else if ( arg == "--help" )
        {
            print_help();
            return EXIT_SUCCESS;
        }
        else if ( arg == "--internal" )
        {
            config.create_internal_wrapper_code_file = true;
        }
        else if ( arg == "--overwrite" )
        {
            config.overwrite = true;
        }
        else if ( arg == "-v" || arg == "--verbose" )
        {
            config.verbose++;
        }
        else if ( arg == "--quiet" )
        {
            config.verbose--;
        }
        else
        {
            print_short_usage();
            return EXIT_FAILURE;
        }
    }

    /* Check whether obligatory parameters are given */
    if ( config.header_file.empty() )
    {
        cerr << "ERROR: no header file given" << endl;
        print_help();
        return EXIT_FAILURE;
    }

    if ( config.wrapper_name.empty() )
    {
        cerr << "ERROR: no name given" << endl;
        print_help();
        return EXIT_FAILURE;
    }

    if ( config.library_names.empty() )
    {
        // This is not an error. It just means that we do not do runtime linking.
        // Not specifying the library_names and linking at runtime (dlopen) would be an error.
        // The decision between runtime and link time wrapping is used before and after calling this tool.
        // We do not know and care here.
        //
        //cerr << "ERROR: no library name given" << endl;
        //print_help();
        //return EXIT_FAILURE;
    }

    /* Set default parameter */
    if ( config.language == "" )
    {
        config.language = "c";
    }

    if ( config.output_directory == "" )
    {
        config.output_directory = canonicalize_path( "." );
    }

    /* Check and apply filter file if given */
    if ( !config.filter_file_name.empty() )
    {
        SCOREP_ErrorCode err = SCOREP_Filter_ParseFile( config.filter, config.filter_file_name.c_str() );
        if ( err != SCOREP_SUCCESS )
        {
            cerr << "ERROR: Parsing filter file '" << config.filter_file_name
                 << "' failed." << endl;
            return EXIT_FAILURE;
        }
    }

    /* Create output file names */
    config.function_list_file_name = join_path(
        config.output_directory,
        create_result_file_name( config.wrapper_name,
                                 config.language,
                                 MODE_FUNCTION_LIST ) );
    if ( exists_file( config.function_list_file_name )
         && !config.overwrite
         && !ask_user( config.function_list_file_name ) )
    {
        cerr << "ERROR: Output file already exists: '" << config.function_list_file_name << "'" << endl;
        return EXIT_FAILURE;
    }

    config.wrap_flags_file_name = join_path(
        config.output_directory,
        create_result_file_name( config.wrapper_name,
                                 config.language,
                                 MODE_WRAP_FLAGS ) );
    if ( exists_file( config.wrap_flags_file_name )
         && !config.overwrite
         && !ask_user( config.wrap_flags_file_name ) )
    {
        cerr << "ERROR: Output file already exists: '" << config.wrap_flags_file_name << "'" << endl;
        return EXIT_FAILURE;
    }

    config.nvcc_wrap_flags_file_name = join_path(
        config.output_directory,
        create_result_file_name( config.wrapper_name,
                                 config.language,
                                 MODE_NVCC_WRAP_FLAGS ) );
    if ( exists_file( config.nvcc_wrap_flags_file_name )
         && !config.overwrite
         && !ask_user( config.nvcc_wrap_flags_file_name ) )
    {
        cerr << "ERROR: Output file already exists: '" << config.nvcc_wrap_flags_file_name << "'" << endl;
        return EXIT_FAILURE;
    }

    config.wrap_file_name = join_path(
        config.output_directory,
        create_result_file_name( config.wrapper_name,
                                 config.language,
                                 MODE_ADAPTER_CODE ) );
    if ( exists_file( config.wrap_file_name )
         && !config.overwrite
         && !ask_user( config.wrap_file_name ) )
    {
        cerr << "ERROR: Output file already exists: '" << config.wrap_file_name << "'" << endl;
        return EXIT_FAILURE;
    }

    /* Print setup information  */
    if ( config.verbose >= 1 )
    {
        cout << endl
             << "== Setup ==" << endl
             << "Input:" << endl
             << "  Header file                " << config.header_file << endl
             << "  Libraries                  ";
        for ( vector<string>::const_iterator i = config.library_names.begin(); i != config.library_names.end(); ++i )
        {
            cout << *i << ", ";
        }
        cout << endl
             << "  Filter file                " << ( config.filter_file_name.length() == 0 ? "none" : config.filter_file_name ) << endl
             << endl
             << "Output:" << endl
             << "  Function list file         " << config.function_list_file_name << endl
             << "  Wrapper code skeleton      " << config.wrap_file_name << endl;
    }

    SCOREP_Libwrap_Generator generator( config );
    int                      ret = generator.generate_source_code_files();
    if ( ret == EXIT_FAILURE )
    {
        cerr << "ERROR: could not generate source code files" << endl;
    }
    else if ( generator.noSymbolsWrapped() )
    {
        cerr << "ERROR: Cannot find any function to wrap. Make sure to add include statements to libwrap.h, and verify that the filter does not filter out everything." << endl;
        ret = EXIT_FAILURE;
    }

    if ( ret != EXIT_SUCCESS )
    {
        if ( exists_file( config.function_list_file_name ) )
        {
            remove( config.function_list_file_name.c_str() );
        }
        if ( exists_file( config.wrap_flags_file_name ) )
        {
            remove( config.wrap_flags_file_name.c_str() );
        }
        if ( exists_file( config.nvcc_wrap_flags_file_name ) )
        {
            remove( config.nvcc_wrap_flags_file_name.c_str() );
        }
        if ( exists_file( config.wrap_file_name ) )
        {
            remove( config.wrap_file_name.c_str() );
        }
    }

    SCOREP_Filter_Delete( config.filter );

    return ret;
}

string
create_result_file_name( const string&   name,
                         const string&   language,
                         generation_mode mode )
{
    /* Determine prefix */
    string tmp;
    switch ( mode )
    {
        case MODE_ADAPTER_CODE:
        case MODE_FUNCTION_LIST:
            tmp = "scorep_libwrap_";
            break;
        case MODE_WRAP_FLAGS:
        case MODE_NVCC_WRAP_FLAGS:
            break;
    }

    tmp.append( name );

    /* append new endings */
    if ( mode == MODE_WRAP_FLAGS )
    {
        tmp.append( ".wrap" );
    }
    else if ( mode == MODE_NVCC_WRAP_FLAGS )
    {
        tmp.append( ".nvcc.wrap" );
    }
    else if ( mode == MODE_FUNCTION_LIST )
    {
        tmp.append( ".inc.c" );
    }
    else if ( language == "c++" )
    {
        tmp.append( ".cc" );
    }
    else
    {
        tmp.append( ".c" );
    }

    return tmp;
}

/**
 * Prints a dialog which warns about already existing files and asks
 * how to proceed: overwrite file or enter a new file name.
 *
 * @param filename          Current file name
 * @return true if the user wants to overwrite this file
 */
bool
ask_user( const string& filename )
{
    // evtl. return a new string instead of changing the given one
    char decision;

    /* Show warning */
    cerr << "Warning: file " << filename
         << " already exists. Do you really want to override? [y/n]"
         << endl;

    /* Get input from the user */
    cin.get( decision );
    cin.ignore( numeric_limits<streamsize>::max(), '\n' );

    /* Process input */
    if ( !( decision == 'y' || decision == 'Y' ) )
    {
        return false;
    }

    return true;
}
