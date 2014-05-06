/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file scorep_config_utils.cpp
 *
 * Utility functions for the config tool.
 */

#include <config.h>
#include "scorep_config_utils.hpp"

using namespace std;

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

/**
 * Removes leading whitespaces and returns the stripped string.
 * @param The string from which leading whitespaces are removed.
 */
static inline string
strip_leading_whitespace( const string& input )
{
    const char* pos = input.c_str();
    while ( ( *pos != '\0' ) && ( ( *pos == ' ' ) || ( *pos == '\t' ) ) )
    {
        pos++;
    }
    return pos;
}

/**
 * Strips the head and leading delimiter from a input string. Ignores leading whitespaces.
 */
string
strip_head( const string& input,
            const string& head_orig,
            const string& delimiter_orig )
{
    string output    = strip_leading_whitespace( input );
    string head      = strip_leading_whitespace( head_orig );
    string delimiter = strip_leading_whitespace( delimiter_orig );

    if ( output.compare( 0, head.length(), head ) == 0 )
    {
        output.erase( 0, head.length() );
    }

    output = strip_leading_whitespace( input );
    if ( output.compare( 0, delimiter.length(), delimiter ) == 0 )
    {
        output.erase( 0, delimiter.length() );
    }
    return output;
}

bool
has_item( const deque<string>& input, const string& item )
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
string_to_deque( const string& input, const string& delimiter )
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
