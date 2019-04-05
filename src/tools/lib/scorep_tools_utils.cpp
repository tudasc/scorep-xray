/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
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
 * @file
 */

#include <config.h>

#include "scorep_tools_utils.hpp"

#include <UTILS_IO.h>
#include <UTILS_CStr.h>

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

using std::string;
using std::vector;
using std::deque;
using std::ifstream;
using std::stringstream;

string
undo_backslashing( string str )
{
    string::size_type pos = str.find( "\\" );
    while ( pos != string::npos )
    {
        str.erase( pos, 1 );
        pos = str.find( "\\", pos + 1 );
    }
    return str;
}

string
backslash_special_chars( string str )
{
    static const string char_list = "\\ ?\"<>|&;,`'$()\n\t#*";

    string::size_type pos = str.find_last_of( char_list );
    while ( pos != string::npos )
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

string
make_string_literal( string str )
{
    static const string char_list    = "\"\\\a\b\f\n\r\t\v";
    static const string replace_list = "\"\\abfnrtv";

    string::size_type pos = 0;
    while ( pos < str.size() )
    {
        string::size_type special_char = char_list.find_first_of( str[ pos ] );
        if ( special_char != string::npos )
        {
            str[ pos ] = replace_list[ special_char ];
            str.insert( pos, "\\" );
            pos++;
        }
        pos++;
    }
    return '"' + str + '"';
}

string
remove_multiple_whitespaces( string str )
{
    string            search = "  "; // this string contains 2 spaces
    string::size_type pos;

    /* Trim */
    pos = str.find_last_not_of( ' ' );
    if ( pos != string::npos )
    {
        str.erase( pos + 1 );
        pos = str.find_first_not_of( ' ' );
        if ( pos != string::npos )
        {
            str.erase( 0, pos );
        }
    }
    else
    {
        str.erase( str.begin(), str.end() );
    }

    /* Remove multiple white-spaces */
    while ( ( pos = str.find( search ) ) != string::npos )
    {
        /* remove 1 character from the string at index */
        str.erase( pos, 1 );
    }

    return str;
}

string
replace_all( const string& pattern,
             const string& replacement,
             string        original )
{
    string::size_type pos            = original.rfind( pattern, string::npos );
    int               pattern_length = pattern.length();

    while ( pos != string::npos )
    {
        original.replace( pos, pattern_length, replacement );
        pos = original.rfind( pattern, pos );
    }

    return original;
}

/* ****************************************************************************
   Helper functions for file name manipulation and analysis
******************************************************************************/

string
simplify_path( const string& path )
{
    char* buffer = UTILS_CStr_dup( path.c_str() );
    UTILS_IO_SimplifyPath( buffer );
    string simple_path = buffer;
    free( buffer );
    return simple_path;
}

string
canonicalize_path( const string& path )
{
    static char working_directory[ 1024 ];
    if ( !working_directory[ 0 ] )
    {
        /* Get path of current working directory */
        if ( UTILS_IO_GetCwd( working_directory, sizeof( working_directory ) - 1 ) == NULL )
        {
            return "";
        }
    }

    char* buffer = UTILS_IO_JoinPath( 2, working_directory, path.c_str() );
    UTILS_IO_SimplifyPath( buffer );
    string result = buffer;
    free( buffer );
    return result;
}

string
join_path( const string& path0,
           const string& path1 )
{
    char*  buffer = UTILS_IO_JoinPath( 2, path0.c_str(), path1.c_str() );
    string result = buffer;
    free( buffer );
    return result;
}

string
extract_path( const string& filename )
{
    string::size_type pos = filename.find_last_of( '/' );
    if ( pos == 0 )
    {
        return "/";
    }
    if ( pos != string::npos )
    {
        return filename.substr( 0, pos );
    }
    return ".";
}

string
remove_path( const string& fullPath )
{
    string::size_type pos = fullPath.rfind( "/" );
    if ( pos == string::npos )
    {
        return fullPath;
    }
    else
    {
        return fullPath.substr( pos + 1 );
    }
}

string
get_extension( const string& filename )
{
    string::size_type slash = filename.rfind( "/" );
    if ( slash == string::npos )
    {
        slash = 0;
    }
    string::size_type dot = filename.rfind( "." );
    if ( dot == string::npos || dot < slash )
    {
        /* no "." , or not in the last path component */
        return "";
    }
    return filename.substr( dot );
}

string
remove_extension( const string& filename )
{
    string::size_type slash = filename.rfind( "/" );
    if ( slash == string::npos )
    {
        slash = 0;
    }
    string::size_type dot = filename.rfind( "." );
    if ( dot == string::npos || dot < slash )
    {
        /* no "." , or not in the last path component */
        return filename;
    }
    return filename.substr( 0, dot );
}

bool
exists_file( const string& filename )
{
    ifstream ifile( filename.c_str() );
    return ( bool )ifile;
}

string
find_library( const string&         library,
              const vector<string>& pathList,
              bool                  allowStatic,
              bool                  allowDynamic,
              bool                  onlyDashLArgs )
{
    string lib = library;
    if ( lib.substr( 0, 2 ) == "-l" )
    {
        lib.replace( 0, 2, "lib" );
    }
    else if ( onlyDashLArgs )
    {
        return lib;
    }
    for ( vector<string>::const_iterator current_libdir = pathList.begin();
          current_libdir != pathList.end();
          current_libdir++ )
    {
        string current_path = join_path( *current_libdir, lib );
        if ( allowDynamic && exists_file( current_path + ".so" ) )
        {
            return current_path + ".so";
        }
        if ( allowStatic && exists_file( current_path + ".a" ) )
        {
            return current_path + ".a";
        }
    }
    return "";
}

string
scorep_tolower( string str )
{
    for ( string::size_type i = 0; i < str.length(); i++ )
    {
        str[ i ] = tolower( str[ i ] );
    }
    return str;
}

string
scorep_toupper( string str )
{
    for ( string::size_type i = 0; i < str.length(); i++ )
    {
        str[ i ] = toupper( str[ i ] );
    }
    return str;
}

string
scorep_vector_to_string( const vector<string>& list,
                         const string&         head,
                         const string&         tail,
                         const string&         delimiter )
{
    return join_to_string( list.begin(), list.end(), head, tail, delimiter );
}

string::size_type
find_string_in_list( const string& haystackList,
                     const string& needleString,
                     char          separator )
{
    string haystack = separator + haystackList + separator;
    string needle   = separator + needleString + separator;

    /* If we would have a match, we would matched also the separator, thus we
       would need to increase the pos by one to get the start of the
       needleString. But we also shifted the haystackList by one, thus we would
       need to decrease the pos again. Therefor pos already indicates the right
       position in haystackList, if it was a match at all. */
    return haystack.find( needle );
}

string
remove_string_from_list( const string& haystackList,
                         const string& needleString,
                         char          separator )
{
    string haystack = separator + haystackList + separator;
    string needle   = separator + needleString + separator;

    string result = replace_all( needle,
                                 string( 1, separator ),
                                 haystack );

    return result.substr( 1, result.length() - 2 );
}

vector<string>
split_string( const string& input,
              const string& delimiter )
{
    string            token;
    string::size_type cur_pos = 0;
    string::size_type old_pos = 0;
    vector<string>    result;

    while ( cur_pos != string::npos )
    {
        cur_pos = input.find( delimiter, old_pos );
        token   = input.substr( old_pos, cur_pos - old_pos );
        if ( token != "" )
        {
            result.push_back( token );
        }
        old_pos = cur_pos + delimiter.length();
    }
    return result;
}

string
deque_to_string( const deque<string>& input,
                 const string&        head,
                 const string&        delimiter,
                 const string&        tail )
{
    if ( input.empty() )
    {
        return "";
    }
    string                        output = head;
    deque<string>::const_iterator i;
    for ( i = input.begin(); i != input.end(); i++ )
    {
        if ( i != input.begin() )
        {
            output += delimiter;
        }
        output += *i;
    }
    output += tail;
    return output;
}

deque<string>
string_to_deque( const string& input,
                 const string& delimiter )
{
    std::string             current_path = "";
    std::string::size_type  cur_pos      = 0;
    std::string::size_type  old_pos      = 0;
    std::deque<std::string> path_list;

    while ( cur_pos != std::string::npos )
    {
        cur_pos      = input.find( delimiter, old_pos );
        current_path = input.substr( old_pos, cur_pos - old_pos );
        if ( current_path != "" )
        {
            path_list.push_back( current_path );
        }
        old_pos = cur_pos + delimiter.length();
    }
    return path_list;
}

/**
 * Checks whether @a input contains @a item.
 * @param input A list of strings which is tested for @a item
 * @param item  A string that is searched in @a input.
 * @return true if @a input contains @a item.
 */
static bool
has_item( const deque<string>& input,
          const string&        item )
{
    deque<string>::const_iterator i;
    for ( i = input.begin(); i != input.end(); i++ )
    {
        if ( *i == item )
        {
            return true;
        }
    }
    return false;
}

deque<string>
remove_double_entries( const deque<string>& input )
{
    deque<string>                         output;
    deque<string>::const_reverse_iterator i;
    for ( i = input.rbegin(); i != input.rend(); i++ )
    {
        if ( !has_item( output, *i ) )
        {
            output.push_front( *i );
        }
    }

    return output;
}

string
wrap_lines( const string& message,
            size_t        indent,
            size_t        firstIndent,
            size_t        width )
{
    size_t            column_width = 0;
    size_t            reminder;
    int               nl   = 0;
    string            sep  = "";
    string::size_type curr = 0;

    if ( width > indent )
    {
        column_width = width - indent;
    }
    reminder = column_width;

    stringstream out;

    while ( true )
    {
        /* skip any whitespace */
        curr = message.find_first_not_of( " \t\n\r\v", curr );

        /* stop if we reached the end */
        if ( curr == string::npos )
        {
            break;
        }

        /* print separator to previous word */
        switch ( nl )
        {
            case 0:
                if ( firstIndent == 0 )
                {
                    out << "\n" << string( indent, ' ' );
                }
                else
                {
                    out << string( firstIndent, ' ' );
                }
                sep = "";
                break;

            case 3:
                /* a paragraph, empty line and fall thru to linebreak */
                out << "\n";

            case 2:
                out << "\n" << string( indent, ' ' );
                reminder = column_width;
                sep      = "";
                break;
        }

        /* get length of next word */
        string::size_type next = message.find_first_of( " \t\n\r\v", curr );
        if ( next == string::npos )
        {
            next = message.length();
        }
        string::size_type length = next - curr;

        nl = 1;
        if ( message[ next ] == '\n' )
        {
            nl = 2;
            if ( ( next + 1 ) < message.length() && message[ next + 1 ] == '\n' )
            {
                nl = 3;
            }
        }

        if ( length < reminder || reminder == column_width )
        {
            out << sep << message.substr( curr, length );
            curr     += length;
            reminder -= length + sep.length();
        }
        else
        {
            /* word does not fit anymore in this line, but it was not the first word */
            nl = 2;
        }
        sep = " ";
    }

    if ( nl > 1 )
    {
        out << "\n";
    }

    return out.str();
}
