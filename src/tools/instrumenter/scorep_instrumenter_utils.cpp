/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file       scorep_instrumenter_utils.cpp
 */

#include <config.h>
#include "scorep_instrumenter_utils.hpp"
#include <scorep_config_tool_shmem.h>
#include <UTILS_IO.h>
#include <UTILS_CStr.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

std::string
undo_backslashing( std::string str )
{
    std::string::size_type pos = str.find( "\\" );
    while ( pos != std::string::npos )
    {
        str.erase( pos, 1 );
        pos = str.find( "\\", pos + 1 );
    }
    return str;
}

std::string
backslash_special_chars( std::string str )
{
    static const std::string char_list = "\\ ?\"<>|&;,`'$()\n\t#*";

    std::string::size_type pos = str.find_last_of( char_list );
    while ( pos != std::string::npos )
    {
        str.insert( pos, "\\" );
        if ( pos == 0 )
        {
            break;
        }
        pos = str.find_last_of( char_list, pos - 1 );
    }
    return str;
}

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
    std::string::size_type pos            = original.rfind( pattern, std::string::npos );
    int                    pattern_length = pattern.length();

    while ( pos != std::string::npos )
    {
        original.replace( pos, pattern_length, replacement );
        pos = original.rfind( pattern, pos );
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
    std::string::size_type pos = filename.find_last_of( '/' );
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
    std::string::size_type pos = full_path.rfind( "/" );
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
    std::string::size_type pos = filename.rfind( "." );
    if ( pos == std::string::npos )
    {
        return "";
    }
    return filename.substr( pos );
}

std::string
remove_extension( const std::string& filename )
{
    std::string::size_type pos = filename.rfind( "." );
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
    SCOREP_CHECK_EXT( ".for" );
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

/**
    Checks whether a file is a CUDA source file.
    @param filename A file name.
    @returns true if the file extension indicates CUDA source file.
 */
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
is_interposition_library( const std::string& library_name )
{
    return is_mpi_library( library_name ) || is_shmem_library( library_name );
}

bool
check_lib_name(  const std::string& library_name, const std::string& value )
{
    std::string value_with_dot        = value + ".";
    std::string value_with_underscore = value + "_";
    return ( ( library_name.length() == value.length() ) && ( library_name.substr( 0, value.length() ) == value ) ) ||
           ( ( library_name.length() > value.length() ) && ( library_name.substr( 0, value.length() + 1 ) == value_with_dot ) ) ||
           ( ( library_name.length() > value.length() ) && ( library_name.substr( 0, value.length() + 1 ) == value_with_underscore ) );
}

bool
is_mpi_library( const std::string& library_name )
{
    return check_lib_name( library_name, "mpi" ) ||
           check_lib_name( library_name, "mpich" ) ||
           check_lib_name( library_name, "mpigf" ) ||
           check_lib_name( library_name, "mpigi" ) ||
           check_lib_name( library_name, "mpifort" ) ||
           check_lib_name( library_name, "mpicxx" );
}

bool
is_opencl_library( const std::string& library_name )
{
    return check_lib_name( library_name, "OpenCL" );
}

bool
is_shmem_library( const std::string& library_name )
{
    /*
     * Check for libopenshmem (OpenSHMEM)
     *           liboshmem    (OpenMPI)
     *           libsma       (SGI MPT, Cray SHMEM)
     */

    if ( SCOREP_SHMEM_LIB_NAME != "" )
    {
        return check_lib_name( library_name, SCOREP_SHMEM_LIB_NAME );
    }

    return false;
}

bool
is_pthread_library( const std::string& library_name )
{
    return check_lib_name( library_name, std::string( "pthread" ) ) ||
           check_lib_name( library_name, std::string( "pthreads" ) ) ||
           check_lib_name( library_name, std::string( "lthread" ) );
}

bool
exists_file( const std::string& filename )
{
    std::ifstream ifile( filename.c_str() );
    return ( bool )ifile;
}

std::string
find_library( std::string                     library,
              const std::vector<std::string>& path_list,
              bool                            allow_dynamic,
              bool                            allow_static )
{
    if ( library.substr( 0, 2 ) == "-l" )
    {
        library.replace( 0, 2, "lib" );
    }
    else
    {
        return library;
    }
    for ( std::vector<std::string>::const_iterator current_libdir = path_list.begin();
          current_libdir != path_list.end();
          current_libdir++ )
    {
        std::string current_path = *current_libdir + "/" + library;
        if ( allow_dynamic && exists_file( current_path + ".so" ) )
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

std::string
scorep_tolower( std::string str )
{
    for ( std::string::size_type i = 0; i < str.length(); i++ )
    {
        str[ i ] = tolower( str[ i ] );
    }
    return str;
}

std::string
scorep_toupper( std::string str )
{
    for ( std::string::size_type i = 0; i < str.length(); i++ )
    {
        str[ i ] = toupper( str[ i ] );
    }
    return str;
}

std::string
scorep_vector_to_string( const std::vector<std::string>& list,
                         const std::string&              head,
                         const std::string&              tail,
                         const std::string&              delimiter )
{
    std::stringstream result;
    if ( list.size() == 0 )
    {
        return "";
    }
    result << head << list[ 0 ];
    for ( std::vector<std::string>::size_type i = 1; i < list.size(); i++ )
    {
        result << delimiter << list[ i ];
    }
    result << tail;
    return result.str();
}

std::string::size_type
find_string_in_list( const std::string& haystackList,
                     const std::string& needleString,
                     char               separator )
{
    std::string haystack = separator + haystackList + separator;
    std::string needle   = separator + needleString + separator;

    /* If we would have a match, we would matched also the separator, thus we
       would need to increase the pos by one to get the start of the
       needleString. But we also shifted the haystackList by one, thus we would
       need to decrease the pos again. Therefor pos already indicates the right
       position in haystackList, if it was a match at all. */
    return haystack.find( needle );
}

std::string
remove_string_from_list( const std::string& haystackList,
                         const std::string& needleString,
                         char               separator )
{
    std::string haystack = separator + haystackList + separator;
    std::string needle   = separator + needleString + separator;

    std::string result = replace_all( needle,
                                      std::string( 1, separator ),
                                      haystack );

    return result.substr( 1, result.length() - 2 );
}
