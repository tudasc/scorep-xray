/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file       SCOREP_Wrapgen_HeaderParser.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    Wrapgen_module
 *
 * @brief Generates a prototype xml file from a header file.
 */

#include <config.h>

#include "SCOREP_Wrapgen_Func.h"

#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

// Maxmimum number of bytes read at once from a file.
#define SCOREP_WRAPGEN_MAXLINELENGTH 4096

/** Contains the current group name */
string group = "";

namespace SCOREP
{
namespace Wrapgen
{
/** Reads from a file unil file and or a ';' is reached. Removes all linebreaks and
    substitutes all tabs for single whitespaces.
    @param hfile Pointer to the input stream.
    @returns The read text.
 */
string
readLineFromHeader( ifstream* hfile )
{
    char   cline[ SCOREP_WRAPGEN_MAXLINELENGTH ];
    string line;
    size_t pos;

    // Read until next ';'
    hfile->getline( cline, SCOREP_WRAPGEN_MAXLINELENGTH, ';' );
    line = cline;

    // Remove linebreaks
    do
    {
        pos = line.find( '\n' );
        if ( pos == string::npos )
        {
            break;
        }
        line = line.substr( 0, pos ).append( line.substr( pos + 1 ) );
    }
    while ( true );

    // Substritute tabs with withespace
    do
    {
        pos = line.find( '\t' );
        if ( pos == string::npos )
        {
            break;
        }
        line.replace( pos, 1, 1, ' ' );
    }
    while ( true );

    return line;
}

/** Removes leading and trailing blanks.
    @param line The string hich is trimmed.
    @returns the trimmed string.
 */
string
trim( string line )
{
    int beg, end;
    beg = line.find_first_not_of( " \t" );
    end = line.find_last_not_of( " \t" );
    return line.substr( beg, end - beg + 1 );
}

/** Process a parameter list recursively and adds all paramaters to @a func.
    @param func  Pointer to a @ref Func instance to which the parameter information is
                 added.
    @param plist A string containing the parameter list.
 */
void
processParams( Func*  func,
               string plist )
{
    string param, name, type, suffix;
    int    pos;

    // Separate last paramter and work recusrively on the beginning
    pos = plist.rfind( ',' );
    if ( pos != string::npos )
    {
        processParams( func, plist.substr( 0, pos ) );
        param = trim( plist.substr( pos + 1 ) );
    }
    else
    {
        param = plist;
    }

    // Get suffix
    pos = param.find_last_of( "[" );
    if ( pos == string::npos )
    {
        suffix = "";
    }
    else
    {
        suffix = trim( param.substr( pos ) );
        param  = trim( param.substr( 0, pos ) );
    }

    // Separate name and type
    pos = param.find_last_of( " *&" );
    if ( pos == string::npos )
    {
        name = param;
        printf( "Can not separate type and name\n" );
    }
    type = trim( param.substr( 0, pos + 1 ) );
    name = trim( param.substr( pos + 1 ) );

    // Add to function
    func->add_param( type, name, suffix );
}

/** Evaluates one function prototype
    Interprets the line a complete prototype of a function froma header file.
    @param line  The text from the header file.
    @returns A @ref Func instance containing the extracted information
 */
Func*
evalHeaderLine( string line )
{
    string params, name, type;
    int    bracOpen, bracClose, pos;

    // Separate function name+type from parameters
    bracOpen  = line.find( '(' );
    bracClose = line.rfind( ')' );
    if ( bracOpen == string::npos || bracClose == string::npos )
    {
        printf( "Can not identify parameter section\n" );
        return NULL;
    }
    name   = trim( line.substr( 0, bracOpen ) );
    params = trim( line.substr( bracOpen + 1, bracClose - bracOpen - 1 ) );

    // Remove starting 'extern'
    if ( name.substr( 0, 7 ) == "extern " )
    {
        name = name.substr( 7 );
        name = trim( name );
    }

    // Separate function name and type
    pos = name.find_last_of( " \t*&" );
    if ( pos == string::npos )
    {
        printf( "Can not separate type and name\n" );
        return NULL;
    }
    type = trim( name.substr( 0, pos + 1 ) );
    name = trim( name.substr( pos + 1 ) );

    // Create function object
    Func* newFunc = new Func( type, name, group, group );

    // Process parameter
    if ( params != "void" )
    {
        processParams( newFunc, params );
    }

    return newFunc;
}
}   // namespace Wrapgen
}   // namespace SCOREP

/** Prints usage information.
    @param name  The name of the program (e.g. ergv[0])
 */
void
printUsage( string name )
{
    cout << name << " generates a Prototype XML file from a stripped header file." << endl
         << "Usage:" << endl
         << "       " << name << " <headerfile>\n" << endl
         << "Param: <headerfile>  The filename of the stripped header file from which" << endl
         << "       the prototypes are generated.\n" << endl
         << "It works on stripped header files, meaning that the headers contain" << endl
         << "nothing except the functin prototypes. The file must not contain any" << endl
         << "typedefinitions or comments." << endl
         << "The generated prototypes are a skeleton which need manual adjustments." << endl
         << "The access type of parameters is always set to i for input.\n" << endl
         << "The headerfile can contain directeves for the prototype generator." << endl
         << "Curetnly, the only supported directive is:" << endl
         << "#pragma proto group <groupname>   It sets the group and guard of the" << endl
         << "                                  following functions to <groupname>" << endl
         << "                                  until it is overwritten by the next" << endl
         << "                                  group directive." << endl
         << "NOTE: All #pragma directives must be terminated with a semi-colon!" << endl;
    abort();
}


int
main( int   argc,
      char* argv[] )
{
    // Process command line
    if ( argc < 2 )
    {
        printUsage( argv[ 0 ] );
    }

    // Open file
    ifstream
    file( argv[ 1 ] );
    if ( !file.is_open() )
    {
        cout << "Could not open " << argv[ 1 ] << endl;
        abort();
    }

    // Process file
    string line;
    do
    {
        line = SCOREP::Wrapgen::readLineFromHeader( &file );
        // File end?
        if ( line.empty() )
        {
            break;
        }
        // Group directive?
        if ( line.substr( 0, 20 ) == "#pragma proto group " )
        {
            group =  SCOREP::Wrapgen::trim( line.substr( 20 ) );
        }
        // Assume function prototype
        else
        {
            SCOREP::Wrapgen::Func* func = SCOREP::Wrapgen::evalHeaderLine( line );
            cout << func->write_conf();
            delete func;
        }
    }
    while ( true );
}
