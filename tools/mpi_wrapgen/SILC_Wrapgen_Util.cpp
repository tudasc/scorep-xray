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
 * @file       SILC_Wrapgen_Util.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief String processing functions.
 */


#include <config.h>

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <cctype>
using std::toupper;
using std::tolower;
#include <sstream>
using std::ostringstream;
using std::istringstream;

#include "SILC_Wrapgen_Util.h"
using namespace SILC::Wrapgen;

/**
 * @internal
 * Definition of whitespace for trimming strings
 */
static const char* const whitespace = " \t\r\n\v\f";

/**
 * Ignore the rest of the line in the input file stream, and move to the
 * next line.
 * @param istr Input file stream
 */
void
SILC::Wrapgen::ignoreRestOfLine
(
    ifstream& istr
)
{
    istr.ignore( 99999999, '\n' );
}

/**
 * Convert an integer to a string.
 * @param i Integer to be converted
 * @return String variable representing the decimal notation of the
 *         input
 */
string
SILC::Wrapgen::int2string
(
    int i
)
{
    ostringstream num;
    num << i;
    return num.str();
}

/**
 * Convert a string to an integer.
 * @param s String to be converted
 * @result Integer value of input string or -1 if input string could not
 * be converted.
 */
int
SILC::Wrapgen::string2int
(
    const string& s
)
{
    istringstream
    str
    (
        s
    );

    int i;
    if ( str >> i )
    {
        return i;
    }
    else
    {
        return -1;
    }
}

/**
 * Convert all characters to uppercase.
 * @param s String variable that is to be converted to uppercase.
 */
void
SILC::Wrapgen::toupper
(
    string& s
)
{
    int length = s.length();
    for ( int i = 0; i < length; i++ )
    {
        s[ i ] = std::toupper( s[ i ] );
    }
}

/**
 * Convert all characters to lowercase.
 * @param s String variable that is to be converted to lowercase.
 */
void
SILC::Wrapgen::tolower
(
    string& s
)
{
    int length = s.length();
    for ( int i = 0; i < length; i++ )
    {
        s[ i ] = std::tolower( s[ i ] );
    }
}

/**
 * Convert first character to uppercase
 */
string
SILC::Wrapgen::start_caps
(
    const string& s
)
{
    string
    str
    (
        s
    );

    std::toupper( str[ 0 ] );
    return str;
}

/**
 * Trim string of leading and training spaces
 */
string
SILC::Wrapgen::trim
(
    const string& s
)
{
    string str;

    size_t startpos = s.find_first_not_of( whitespace );
    size_t endpos   = s.find_last_not_of( whitespace );

    if ( ( string::npos == startpos ) ||
         ( string::npos == endpos ) )
    {
        return str;
    }
    else
    {
        str = s.substr( startpos, endpos - startpos + 1 );
        return str;
    }
}

/**
 * Tokenize a string according to a list of delimiting characters
 * @param str        Input string to be tokenized
 * @param delimiters Delimiting characters
 * @param tokens     List of tokens found
 */
void
SILC::Wrapgen::tokenize
(
    const string&   str,
    const string&   delimiters,
    vector<string>& tokens
)
{
    string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    string::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( string::npos != pos || string::npos != lastPos )
    {
        tokens.push_back( str.substr( lastPos, pos - lastPos ) );
        lastPos = str.find_first_not_of( delimiters, pos );
        pos     = str.find_first_of( delimiters, lastPos );
    }
}

/**
 * Add padding to left or right of string
 * @param pad_char Character used for padding
 * @param str string to be padded
 * @param width padding width
 * @param dir direction of padding
 * @return padded string
 */
string
SILC::Wrapgen::padding
(
    const string& str,
    char          pad_char,
    int           width,
    char          dir
)
{
    string
    padstr
    (
        str
    );

    switch ( dir )
    {
        case 'r':
            for ( int i = padstr.length(); i < width; ++i )
            {
                padstr += pad_char;
            }
            break;
        case 'l':
        {
            string tmp;
            for ( int i = padstr.length(); i < width; ++i )
            {
                tmp += pad_char;
            }
            padstr = tmp + padstr;
            break;
        }
    }

    return padstr;
}
