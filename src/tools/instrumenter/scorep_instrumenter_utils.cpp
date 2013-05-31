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
 * @status     alpha
 * @file       scorep_instrumenter_utils.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 */

#include <config.h>
#include "scorep_instrumenter_utils.hpp"
#include <UTILS_IO.h>
#include <UTILS_CStr.h>

#include <string>
#include <fstream>
#include <stdlib.h>

std::string
remove_multiple_whitespaces( std::string str )
{
    std::string            search = "  "; // this string contains 2 spaces
    std::string::size_type pos;

    /* Trim */
    pos = str.find_last_not_of( ' ' );
    if ( pos != std::string::npos )
    {
        str.erase( pos + 1 );
        pos = str.find_first_not_of( ' ' );
        if ( pos != std::string::npos )
        {
            str.erase( 0, pos );
        }
    }
    else
    {
        str.erase( str.begin(), str.end() );
    }

    /* Remove multiple white-spaces */
    while ( ( pos = str.find( search ) ) != std::string::npos )
    {
        /* remove 1 character from the string at index */
        str.erase( pos, 1 );
    }

    return str;
}

std::string
replace_all( const std::string& pattern,
             const std::string& replacement,
             std::string        original )
{
    std::string::size_type pos            = original.find( pattern, 0 );
    int                    pattern_length = pattern.length();

    while ( pos != std::string::npos )
    {
        original.replace( pos, pattern_length, replacement );
        pos = original.find( pattern, 0 );
    }

    return original;
}

/* ****************************************************************************
   Helper functions for file name manipulation and analysis
******************************************************************************/

std::string
simplify_path( const std::string& path )
{
    char* buffer = UTILS_CStr_dup( path.c_str() );
    UTILS_IO_SimplifyPath( buffer );
    std::string simple_path = buffer;
    free( buffer );
    return simple_path;
}

std::string
extract_path( const std::string& filename )
{
    size_t pos = filename.find_last_of( '/' );
    if ( pos == 0 )
    {
        return "/";
    }
    if ( pos != std::string::npos )
    {
        return filename.substr( 0, pos );
    }
    return ".";
}

std::string
remove_path( const std::string& full_path )
{
    size_t pos = full_path.rfind( "/" );
    if ( pos == std::string::npos )
    {
        return full_path;
    }
    else
    {
        return full_path.substr( pos + 1, std::string::npos );
    }
}

std::string
get_extension( const std::string& filename )
{
    int pos = filename.rfind( "." );
    if ( pos == std::string::npos )
    {
        return "";
    }
    return filename.substr( pos );
}

std::string
remove_extension( const std::string& filename )
{
    int pos = filename.rfind( "." );
    if ( pos == std::string::npos )
    {
        return filename;
    }
    return filename.substr( 0, pos );
}

bool
is_fortran_file( const std::string& filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    #define SCOREP_CHECK_EXT( ext ) if ( extension == ext ) return true
    SCOREP_CHECK_EXT( ".f" );
    SCOREP_CHECK_EXT( ".F" );
    SCOREP_CHECK_EXT( ".f90" );
    SCOREP_CHECK_EXT( ".F90" );
    SCOREP_CHECK_EXT( ".fpp" );
    SCOREP_CHECK_EXT( ".FPP" );
    SCOREP_CHECK_EXT( ".For" );
    SCOREP_CHECK_EXT( ".FOR" );
    SCOREP_CHECK_EXT( ".Ftn" );
    SCOREP_CHECK_EXT( ".FTN" );
    SCOREP_CHECK_EXT( ".f95" );
    SCOREP_CHECK_EXT( ".F95" );
    SCOREP_CHECK_EXT( ".f03" );
    SCOREP_CHECK_EXT( ".F03" );
    SCOREP_CHECK_EXT( ".f08" );
    SCOREP_CHECK_EXT( ".F08" );
    #undef SCOREP_CHECK_EXT
    return false;
}

bool
is_c_file( const std::string& filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    #define SCOREP_CHECK_EXT( ext ) if ( extension == ext ) return true
    SCOREP_CHECK_EXT( ".c" );
    SCOREP_CHECK_EXT( ".C" );
    #undef SCOREP_CHECK_EXT
    return false;
}

bool
is_cpp_file( const std::string& filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    #define SCOREP_CHECK_EXT( ext ) if ( extension == ext ) return true
    SCOREP_CHECK_EXT( ".cpp" );
    SCOREP_CHECK_EXT( ".CPP" );
    SCOREP_CHECK_EXT( ".cxx" );
    SCOREP_CHECK_EXT( ".CXX" );
    SCOREP_CHECK_EXT( ".cc" );
    SCOREP_CHECK_EXT( ".CC" );
    #undef SCOREP_CHECK_EXT
    return false;
}

bool
is_cuda_file( const std::string& filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    #define SCOREP_CHECK_EXT( ext ) if ( extension == ext ) return true
    SCOREP_CHECK_EXT( ".cu" );
    SCOREP_CHECK_EXT( ".CU" );
    #undef SCOREP_CHECK_EXT
    return false;
}

bool
is_source_file( const std::string& filename )
{
    return is_c_file( filename ) ||
           is_cpp_file( filename ) ||
           is_cuda_file( filename ) ||
           is_fortran_file( filename );
}

bool
is_object_file( const std::string& filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    if ( extension == ".o" )
    {
        return true;
    }
    return false;
}

bool
is_library( const std::string& filename )
{
    std::string extension = get_extension( filename );
    if ( extension == "" )
    {
        return false;
    }
    if ( extension == ".a" )
    {
        return true;
    }
    if ( extension == ".so" )
    {
        return true;
    }
    if ( extension.find( ".a." ) != std::string::npos )
    {
        return true;
    }
    if ( extension.find( ".so." ) != std::string::npos )
    {
        return true;
    }
    return false;
}

bool
exists_file( const std::string& filename )
{
    std::ifstream ifile( filename.c_str() );
    return ifile;
}

std::string
find_library( std::string        library,
              const std::string& path_list,
              const std::string& delimiter )
{
    size_t      cur_pos = 0;
    size_t      old_pos = 0;
    std::string current_path;

    if ( library.substr( 0, 2 ) == "-l" )
    {
        library.replace( 0, 2, "lib" );
    }
    else
    {
        return library;
    }
    while ( cur_pos != std::string::npos )
    {
        cur_pos = path_list.find( delimiter, old_pos );
        if ( old_pos < cur_pos ) // Discard trailing delimiter
        {
            current_path  = path_list.substr( old_pos, cur_pos - old_pos );
            current_path += "/" + library;
            if ( exists_file( current_path + ".so" ) )
            {
                return current_path + ".so";
            }
            if ( exists_file( current_path + ".a" ) )
            {
                return current_path + ".a";
            }
        }
        old_pos = cur_pos + 1;
    }
    return "";
}

std::string
scorep_tolower( std::string str )
{
    for ( size_t i = 0; i < str.length(); i++ )
    {
        str[ i ] = tolower( str[ i ] );
    }
    return str;
}

std::string
scorep_toupper( std::string str )
{
    for ( size_t i = 0; i < str.length(); i++ )
    {
        str[ i ] = toupper( str[ i ] );
    }
    return str;
}
