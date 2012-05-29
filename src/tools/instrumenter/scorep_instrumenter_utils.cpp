/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
#include <scorep_instrumenter_utils.hpp>
#include <string>


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
replace_all( std::string &pattern,
             std::string &replacement,
             std::string  original )
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
extract_path( std::string filename )
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
    return "";
}

std::string
remove_path( std::string full_path )
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
get_extension( std::string filename )
{
    int pos = filename.rfind( "." );
    if ( pos == std::string::npos )
    {
        return "";
    }
    return filename.substr( pos );
}

std::string
remove_extension( std::string filename )
{
    int pos = filename.rfind( "." );
    if ( pos == std::string::npos )
    {
        return filename;
    }
    return filename.substr( 0, pos );
}

bool
is_fortran_file( std::string filename )
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
is_c_file( std::string filename )
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
is_cpp_file( std::string filename )
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
is_cuda_file( std::string filename )
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
is_source_file( std::string filename )
{
    return is_c_file( filename ) ||
           is_cpp_file( filename ) ||
           is_cuda_file( filename ) ||
           is_fortran_file( filename );
}

bool
is_object_file( std::string filename )
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
is_library( std::string filename )
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
