/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Wrapgen_Handlers.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Handler for processing a variable expression in a function wrapper.
 */


#include <iostream>
using std::cerr;
using std::endl;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <stack>
using std::stack;

#include "SILC_Wrapgen_Util.h"
#include "SILC_Wrapgen_Handlers.h"
using namespace SILC::Wrapgen;
using namespace SILC::Wrapgen::handler;

/** Map of all registered func handlers */
SILC::Wrapgen::handler::func_handlers_t SILC::Wrapgen::handler::func_handlers;

string
SILC::Wrapgen::handler::apply_modifiers
(
    const string&  instr,
    stack<string>& modifier_stack
)
{
    string
    str
    (
        instr
    );

    // apply any modifiers parsed earlier
    while ( !modifier_stack.empty() )
    {
        string modifier = modifier_stack.top();
        modifier_stack.pop();

        if ( modifier.find( "uppercase" ) != string::npos )
        {
            toupper( str );
        }
        else if ( modifier.find( "lowercase" ) != string::npos )
        {
            tolower( str );
        }
        else if ( modifier.find( "padding left" ) != string::npos )
        {
            size_t pos    = modifier.find_first_of( "0123456789" );
            size_t endpos = modifier.find_first_not_of( "0123456789", pos );

            str = padding( str, ' ',
                           string2int( modifier.substr( pos, endpos - pos - 1 ) ),
                           'l' );
        }
        else if ( modifier.find( "padding right" ) != string::npos )
        {
            size_t pos    = modifier.find_first_of( "0123456789" );
            size_t endpos = modifier.find_first_not_of( "0123456789", pos );

            str = padding( str, ' ',
                           string2int( modifier.substr( pos, endpos - pos - 1 ) ),
                           'r' );
        }
    }

    return str;
}

/**
 * Dispatch output handler referenced by 'key'
 * @param key Key for output handler
 * @param func Function object for handler internal use
 */
string
SILC::Wrapgen::handler::dispatch
(
    const string& key,
    const Func&   func
)
{
    string str;
    string
           internal_key
    (
        key
    );

    stack<string> modifier_stack;
    size_t        pos = string::npos;

    // check if general modifier is attached to key
    while ( ( pos = internal_key.find_last_of( "|" ) ) != string::npos )
    {
        modifier_stack.push( internal_key.substr( pos + 1 ) );
        internal_key = internal_key.substr( 0, pos );
    }

    handler::func_handlers_t::const_iterator it =
        func_handlers.find( internal_key );
    if ( it != func_handlers.end() )
    {
        str = ( it->second )( func );
    }

    str = apply_modifiers( str, modifier_stack );

    return str;
}
