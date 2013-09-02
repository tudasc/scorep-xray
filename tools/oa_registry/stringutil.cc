/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file stringutil.cc
 *
 * This file contains implementation of strings utility functions
 */
#include <config.h>


#include <string>
#include <iostream>
#include <algorithm>


#include "strutil.h"

using std::string;
using std::cout;
using std::endl;
using std::pair;


size_t
strskip_ws( std::string str, int pos )
{
    return str.find_first_not_of( "\t ", pos );
}

size_t
get_token( std::string str, int pos, std::string delim, std::string& tok )
{
    std::string::size_type p1, p2;

    p1 = str.find_first_not_of( delim, pos );

    if ( p1 == std::string::npos )
    {
        tok = "";
        return std::string::npos;
    }

    p2 = str.find_first_of( delim, p1 );

    if ( p2 == std::string::npos )
    {
        tok = str.substr( p1, str.size() - p1 );
        return std::string::npos;
    }

    tok = str.substr( p1, p2 - p1 );

    return p2;
}


//   list< pair<std::string, std::string> >& kvpairs
size_t
get_key_value_pair( std::string str, int pos,
                    std::pair< std::string, std::string >& kvpair )
{
    std::string            key, value;
    std::string::size_type p1;

    // get the key
    p1 = get_token( str, pos, " =", key );
    if ( p1 == std::string::npos )
    {
        kvpair.first  = "";
        kvpair.second = "";
        return std::string::npos;
    }

    // expect '=' next
    p1 = strskip_ws( str, p1 );
    if ( p1 == std::string::npos ||
         str[ p1 ] != '=' )
    {
        kvpair.first  = "";
        kvpair.second = "";
        return std::string::npos;
    }

    p1 = strskip_ws( str, p1 + 1 );
    if ( p1 == std::string::npos )
    {
        kvpair.first  = "";
        kvpair.second = "";
        return std::string::npos;
    }

    // value could be enclosed in double quotes
    if ( str[ p1 ] == '"' )
    {
        p1 = get_token( str, p1, "\"", value );
    }
    else
    {
        p1 = get_token( str, p1, " ", value );
    }
    if ( p1 != std::string::npos )
    {
        p1++;
    }

    kvpair.first  = key;
    kvpair.second = value;

    return p1;
}
