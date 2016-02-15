/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2014
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file       SCOREP_Wrapgen.cpp
 * @ingroup    Wrapgen_module
 *
 * @brief Wrapper generator for automatic creation of MPI wrappers
 */

#include <config.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <sstream>
using std::ostringstream;
using std::istringstream;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <string>
using std::string;
using std::getline;
#include <cstring>
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <set>
using std::set;
#include <stack>
using std::stack;
#include <cctype>
using std::toupper;
#include <getopt.h>
using namespace std;
#include <regex.h>
using namespace std;
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SCOREP_Wrapgen_Help.h"
#include "SCOREP_Wrapgen.h"
#include "SCOREP_Wrapgen_Util.h"

#include "SCOREP_Wrapgen_Handlers.h"
#include "SCOREP_Wrapgen_HandlersMpi.h"

#include "SCOREP_Wrapgen_Funcparam.h"
#include "SCOREP_Wrapgen_Func.h"
#include "SCOREP_Wrapgen_MpiFunc.h"
using namespace SCOREP::Wrapgen;

static xmlChar* _prop = NULL;
#define XML_GET_STR_ATTR( node, id, var ) \
    _prop = xmlGetProp( node, ( const xmlChar* )id ); \
    if ( _prop ) { \
        var = string( ( const char* )_prop ); \
        xmlFree( _prop ); \
    }

#define XML_GET_CHAR_ATTR( node, id, var ) \
    _prop = xmlGetProp( node, ( const xmlChar* )id ); \
    if ( _prop ) { \
        var = _prop[ 0 ]; \
        xmlFree( _prop ); \
    } else { var = 'a'; }

#define OPEN_STREAM( var, filename ) \
    if ( filename ) { \
        var.open( filename ); \
        if ( !var.is_open() ) { \
            cerr << "ERROR: cannot open '" << filename << "' on line " \
                 << __LINE__ << " in file " << __FILE__ << "\n"; \
            exit( EXIT_FAILURE ); \
        } \
    }

/*
 * Helper Functions
 */

namespace
{
/** Command line options */
SCOREP::Wrapgen::option opts;

/** All MPI function prototypes */
map<string, MPIFunc> mpiFuncs;
} // End of anonymous namespace

/**
 * Process commandline options and set variables accordingly
 * @param argc Number of entries in argv
 * @param argv Command line options
 */
void
SCOREP::Wrapgen::process_cmd_line
(
    int&    argc,
    char**& argv
)
{
    int copt;
    opts.suppressbanner = false;
    opts.restriction    = "";
    opts.prototypes     = "";

    while ( ( copt = getopt( argc, argv, "bhp:r:" ) ) != EOF )
    {
        switch ( copt )
        {
            case 'b':
            {
                // if bannerflag is activated, there is no banner ouput on the console
                opts.suppressbanner = true;
                break;
            }
            case 'r':
            {
                opts.restriction = optarg;
                break;
            }
            case 'p':
            {
                opts.prototypes = optarg;
                break;
            }
            case 'h':
            {
                help( argv[ 0 ] );
                break;
            }
            default:
                break;
        }
    }
}

/**
 * Read MPI function prototypes from specification file
 * @param filename Filename of specification file
 * @param funcs Map of all MPI functions defined in the prototype specification
 */
void
SCOREP::Wrapgen::read_prototypes
(
    const char* filename,
    map<string, MPIFunc>& funcs
)
{
    ifstream proto;
    OPEN_STREAM( proto, opts.prototypes.c_str() );

    // read 1st line:  number of MPI function prototypes
    int numberOfPrototypes = 0;
    proto >> numberOfPrototypes;
    ignoreRestOfLine( proto );

    for ( int i = 0; i < numberOfPrototypes; ++i )
    {
        paramlist_t params;
        string      version, group;

        // read multi-line MPI function prototype spec
        // 1st line:
        string rettype;
        getline( proto, rettype );
        // 2st line:
        string funcname;
        getline( proto, funcname );

        // 3rd line:
        int numParam = 0;
        proto >> numParam;
        ignoreRestOfLine( proto );

        // 4th line: input output or both (type of arguments)
        string atype;
        getline( proto, atype );

        // initialize function parameter storage first, then read them
        for ( int a = 0; a < numParam; ++a )
        {
            string arg_modifier, arg_type, arg_name, arg_suffix;
            getline( proto, arg_modifier );
            getline( proto, arg_type );
            getline( proto, arg_name );
            getline( proto, arg_suffix );
            Funcparam arg = Funcparam( arg_modifier, arg_type, arg_name,
                                       arg_suffix, atype[ a ] );
            params.push_back( arg );
        }

        // last line: MPI version and function group
        proto >> version >> group;
        ignoreRestOfLine( proto );
        ignoreRestOfLine( proto );     // ignore (empty) line

        // create new MPI function prototype object and register it in map
        MPIFunc f = MPIFunc( rettype, funcname, group, "", version, params );
        mpiFuncs.insert( make_pair( funcname, f ) );
    }
}

/**
 * Read MPI function prototypes from XML specification file
 * @param filename Filename of specification file
 * @param funcs Map of all MPI functions defined in the prototype specification
 */
void
SCOREP::Wrapgen::read_xml_prototypes
(
    const char* filename,
    map<string, MPIFunc>& funcs
)
{
    xmlDoc* proto_doc = NULL;

    // initialize and test libxml2
    LIBXML_TEST_VERSION

    if ( ( proto_doc = xmlReadFile( filename, NULL, 0 ) ) == NULL )
    {
        cerr << "ERROR: Could not read XML file: " << filename << endl;
    }
    else
    {
        xmlNode* root = NULL;
        xmlNode* cur  = NULL;

        // get root element and start building internal structures
        root = xmlDocGetRootElement( proto_doc );
        if ( xmlStrcmp( root->name, ( const xmlChar* )"prototypes" ) == 0 )
        {
            string family;

            // get prototype family
            XML_GET_STR_ATTR( root, "family", family );

            for ( cur = root->children; cur != NULL; cur = cur->next )
            {
                if ( cur->type == XML_ELEMENT_NODE )
                {
                    string              name, rtype, group;
                    string              version, send = "0", recv = "0";
                    map<string, string> decl, xblock, cleanup, attributes;
                    string              guard;
                    paramlist_t         params;

                    // 'name' is attribute of prototype
                    XML_GET_STR_ATTR( cur, "name", name );
                    // 'rtype' is attribute of prototype
                    XML_GET_STR_ATTR( cur, "rtype", rtype );
                    // 'group' is attribute of prototype
                    XML_GET_STR_ATTR( cur, "group", group );
                    // 'guard' is attribute of prototype
                    XML_GET_STR_ATTR( cur, "guard", guard );

                    // check children elements for params, countrules
                    // and version
                    for ( xmlNode* el = cur->children; el != NULL; el =
                              el->next )
                    {
                        if ( el->type == XML_ELEMENT_NODE )
                        {
                            if ( xmlStrcmp( el->name, ( const xmlChar* )
                                            "param" ) == 0 )
                            {
                                string         pname, type_modifier, type, suffix, special;
                                char           access;
                                vector<string> special_tags;

                                XML_GET_STR_ATTR( el, "type_modifier", type_modifier );
                                XML_GET_STR_ATTR( el, "type", type );
                                XML_GET_STR_ATTR( el, "name", pname );
                                XML_GET_CHAR_ATTR( el, "access", access );
                                XML_GET_STR_ATTR( el, "suffix", suffix );
                                XML_GET_STR_ATTR( el, "special", special );

                                params.push_back( Funcparam( type_modifier, type,
                                                             pname, suffix, access ) );
                                tokenize( special, ",", special_tags );
                                for ( vector<string>::const_iterator tag = special_tags.begin();
                                      tag != special_tags.end(); ++tag )
                                {
                                    params.back().add_special( *tag );
                                }
                            }
                            if ( xmlStrcmp( el->name, ( const xmlChar* )
                                            "decl" ) == 0 )
                            {
                                xmlChar* content = NULL;
                                content = xmlNodeGetContent( el );
                                if ( content )
                                {
                                    string id;
                                    XML_GET_STR_ATTR( el, "id", id );
                                    decl[ id ] = string( ( const char* )content );
                                    xmlFree( content );
                                }
                            }
                            if ( xmlStrcmp( el->name, ( const xmlChar* )
                                            "xblock" ) == 0 )
                            {
                                xmlChar* content = NULL;
                                content = xmlNodeGetContent( el );
                                if ( content )
                                {
                                    string id;
                                    XML_GET_STR_ATTR( el, "id", id );
                                    xblock[ id ] = string( ( const char* )content );
                                    xmlFree( content );
                                }
                            }
                            if ( xmlStrcmp( el->name, ( const xmlChar* )
                                            "cleanup" ) == 0 )
                            {
                                xmlChar* content = NULL;
                                content = xmlNodeGetContent( el );
                                if ( content )
                                {
                                    string id;
                                    XML_GET_STR_ATTR( el, "id", id );
                                    cleanup[ id ] = string( ( const char* )content );
                                    xmlFree( content );
                                }
                            }

                            if ( xmlStrcmp( el->name, ( const xmlChar* )
                                            "attribute" ) == 0 )
                            {
                                xmlChar* content = NULL;
                                content = xmlNodeGetContent( el );
                                if ( content )
                                {
                                    string id;
                                    XML_GET_STR_ATTR( el, "id", id );
                                    attributes[ id ] = string( ( const char* )content );
                                    xmlFree( content );
                                }
                            }

                            if ( family == "mpi" )
                            {
                                if ( xmlStrcmp( el->name, ( const xmlChar* )
                                                "version" ) == 0 )
                                {
                                    XML_GET_STR_ATTR( el, "id", version );
                                }
                                if ( xmlStrcmp( el->name, ( const xmlChar* )
                                                "countrules" ) == 0 )
                                {
                                    XML_GET_STR_ATTR( el, "send", send );
                                    //if (send.length() == 0) send = "0";
                                    XML_GET_STR_ATTR( el, "recv", recv );
                                    //if (recv.length() == 0) recv = "0";
                                }
                            }
                        }
                    }

                    if ( family == "mpi" )
                    {
                        // create new MPI function prototype object
                        // and register it in map
                        MPIFunc f = MPIFunc( rtype, name, group, guard,
                                             version, params );
                        for ( map<string, string>::const_iterator it = attributes.begin();
                              it != attributes.end();
                              ++it )
                        {
                            f.set_attribute( it->first, it->second );
                        }
                        for ( map<string, string>::const_iterator it = decl.begin();
                              it != decl.end();
                              ++it )
                        {
                            f.set_decl_block( it->first, it->second );
                        }
                        for ( map<string, string>::const_iterator it = xblock.begin();
                              it != xblock.end();
                              ++it )
                        {
                            f.set_expr_block( it->first, it->second );
                        }
                        for ( map<string, string>::const_iterator it = cleanup.begin();
                              it != cleanup.end();
                              ++it )
                        {
                            f.set_cleanup_block( it->first, it->second );
                        }
                        f.set_sendcount_rule( send );
                        f.set_recvcount_rule( recv );
                        mpiFuncs.insert( make_pair( name, f ) );
                    }
                }
            }
        }
    }

    // clean up
    xmlFreeDoc( proto_doc );
    xmlCleanupParser();
}

void
SCOREP::Wrapgen::write_xml_prototypes
(
    const char* filename,
    map<string, MPIFunc>& funcs
)
{
    map<string, MPIFunc>::iterator it;
    ofstream                       pconf( filename );

    pconf << "<prototypes count=\"" << funcs.size() << "\" family=\"mpi\" >\n";

    for ( it = funcs.begin(); it != funcs.end(); ++it )
    {
        pconf << it->second.write_conf();
    }
    pconf << "</prototypes>" << endl;
    pconf.close();
}

/**
 * Process a textfile by passing it directly to stdout
 * @param filename Filename of the textfile
 */
void
SCOREP::Wrapgen::handle_textfile
(
    const char* filename
)
{
    ifstream textfile( filename );

    string text;
    while ( getline( textfile, text ) )
    {
        cout << text << "\n";
    }
}

/**
 * Process a wrapper template file by applying the template to all
 * @param filename Filename of the template file
 */
int
SCOREP::Wrapgen::handle_wrapper_template
(
    const char* filename
)
{
    // for all defined MPI functions
    for ( map<string, MPIFunc>::iterator it = mpiFuncs.begin();
          it != mpiFuncs.end(); ++it )
    {
        if ( generateOutput( it->second, filename,
                             opts.restriction ) )
        {
            return 1;
        }
    }
    return 1;
}

/**
 * Process a file template file
 */
int
SCOREP::Wrapgen::handle_file_template
(
    const char* filename
)
{
    string   src_line;
    ifstream file_tmpl;
    int      lineno = -1;
    OPEN_STREAM( file_tmpl, filename );

    while ( getline( file_tmpl, src_line ) )
    {
        lineno++;
        if ( string::size_type pos = src_line.find( "#pragma wrapgen" ) != string::npos )
        {
            string        scope, cmd, wrapper_tmpl;
            istringstream line_stream( src_line.substr( pos + 15 ) );

            line_stream >> scope >> cmd >> wrapper_tmpl;

            if ( scope == "single" )
            {
                map<string, MPIFunc>::iterator it = mpiFuncs.find( cmd );
                if ( it == mpiFuncs.end() )
                {
                    cerr << "ERROR: unknown function '" << cmd << "' in '"
                         << filename << "'\n";
                }
                else
                {
                    if ( generateOutput( it->second, wrapper_tmpl.c_str(), "" ) )
                    {
                        return 1;
                    }
                }
            }
            else if ( scope == "multiple" )
            {
                string            op;    // operator for choosing multiple functions
                string            rule;  // parameter of operator
                string::size_type ppos;  // offset of opening bracket
                if ( ( ppos = cmd.find( '(' ) ) != string::npos )
                {
                    op   = cmd.substr( 0, ppos );
                    rule = cmd.substr( ppos + 1, cmd.length() - ppos - 2 );
                }
                if ( op.compare( "restrict" ) == 0 )
                {
                    map<string, MPIFunc>::iterator it;
                    for ( it = mpiFuncs.begin(); it != mpiFuncs.end(); ++it )
                    {
                        string rule( cmd.substr( pos + 8, cmd.length() - pos - 9 ) );

                        generateOutput( it->second, wrapper_tmpl.c_str(),
                                        rule );
                    }
                }
                else if ( op.compare( "regex" ) == 0 )
                {
                    regex_t    pattern;
                    regmatch_t match;
                    int        ret;

                    if ( ( ret = regcomp( &pattern, rule.c_str(),
                                          REG_EXTENDED + REG_ICASE + REG_NOSUB ) ) == 0 )
                    {
                        map<string, MPIFunc>::iterator it;
                        for ( it = mpiFuncs.begin(); it != mpiFuncs.end(); ++it )
                        {
                            if ( ( ret = regexec( &pattern, it->first.c_str(),
                                                  rule.length(), &match, 0 ) ) == 0 )
                            {
                                generateOutput( it->second,
                                                wrapper_tmpl.c_str(), "" );
                            }
                        }
                    }
                    else
                    {
                        cerr << "Failed to compile regular expression:\n"
                             << filename << ":" << lineno << ": " << rule << endl;
                        exit( EXIT_FAILURE );
                    }
                }
            }
            else if ( scope == "include" )
            {
                ifstream include_file;
                string   line;
                OPEN_STREAM( include_file, cmd.c_str() );

                while ( getline( include_file, line ) )
                {
                    cout << line << endl;
                }
            }
        }
        else
        {
            cout << src_line << "\n";
        }
    }
    return 1;
}

/**
 * Search for next template variable. If a template variable is found,
 * the parameter 'key' is overwritten with the variable name found. If
 * no template variable is found, 'key' is left unchanged.
 * @param str string searched for
 * @param key key of template variable found
 * @return position of template variable in string
 */
string::size_type
find_variable
(
    const string& str,
    size_t        p,
    string&       key
)
{
    string::size_type pos = str.rfind( "$", p );

    if ( pos != string::npos )
    {
        if ( str[ pos + 1 ] == '{' )
        {
            // parse long variable name
            string::size_type endpos = string::npos;

            if ( ( endpos = str.find_first_of( '}', pos + 2 ) ) != string::npos )
            {
                key = str.substr( pos + 2, endpos - pos - 2 );
            }
            else
            {
                cerr << "ERROR: Missing closing bracket while parsing template variable" << endl;
            }
        }
        else
        {
            // one character variable name
            key = str[ pos + 1 ];
        }
    }

    return pos;
}

/**
 * Check whether a given function is to be included in the output set
 * @param[in] func Reference to Func-object to be queried
 * @param[in] restriction String representing the restriction criteria
 * @return true is selected, false otherwise
 */
bool
SCOREP::Wrapgen::is_selected
(
    const Func&   func,
    const string& restriction
)
{
    bool           ret_val = true;
    vector<string> token;
    tokenize( restriction, "+", token );

    for ( vector<string>::iterator tok = token.begin();
          tok != token.end(); ++tok )
    {
        bool invert_selection = false;
        int  scope_offset     = 0;

        if ( ( *tok ).at( scope_offset ) == '!' )
        {
            invert_selection = true;
            scope_offset++;
        }
        // first character indicates scope of the rule token
        char   rule_scope = ( *tok ).at( scope_offset );
        string rule_body  = ( *tok ).substr( scope_offset + 1 );

        switch ( rule_scope )
        {
            case  'g':  //group
                ret_val = ret_val &&
                          ( ( func.get_group() == rule_body ) ^ invert_selection );
                break;
            case  'i':  //ID
                ret_val = ret_val &&
                          ( ( func.get_id() == rule_body ) ^ invert_selection );
                break;
            case  'n':  //name
                ret_val = ret_val &&
                          ( ( func.get_name().find( rule_body ) != string::npos ) ^
                            invert_selection );
                break;
            case  't':  //return  type
                ret_val = ret_val &&
                          ( ( func.get_rtype() == rule_body ) ^ invert_selection );
                break;
            case  'v':  //MPI version
            {
                const MPIFunc& mpifunc = dynamic_cast<const MPIFunc&>( func );
                ret_val = ret_val &&
                          ( ( mpifunc.get_version() == string2int( rule_body ) ) ^
                            invert_selection );
                break;
            }
            default:
                break;
        } //end  switch
    }

    return ret_val;
}

/**
 * Generate output based on template files
 * @param func MPI function to be processed
 * @param templatefile Filename of the wrapper tamplate
 * @param rstring Restriction filter
 * @return Returns always false. Some calls to this functions invoke error handling
 *         if the return would be true.
 */
bool
SCOREP::Wrapgen::generateOutput
(
    MPIFunc&    func,
    const char* templatefile,
    string      rstring
)
{
    /* Check whether the specified wrapper template can be opened. */
    ifstream wrap_template;
    OPEN_STREAM( wrap_template, templatefile );

    /*
     * Only proceed, if function is selected for output
     */
    if ( !is_selected( func, rstring ) )
    {
        return false;
    }

    //  read  wrapper  template  and  copy  to  output
    //  and  replace  template  variables  (marked  by  ${...})
    string line;

    while ( getline( wrap_template, line ) )
    {
        string::size_type p = string::npos;
        string            key;

        //string::size_type last_p = string::npos;
        bool output_line = true;

        // replace all template variables with corresponding output
        while ( ( p = find_variable( line, p, key ) ) != string::npos )
        {
            // replacement string is output of dispatched handler
            string repl = handler::dispatch( key, func );

            // replace template variable with return of dispatcher
            line.replace( p, key.length() + 3, repl );

            /*
             * only ouput, if replacement did not created an empty line
             * NOTE: This is just a "beautification" of the generated
             *       code, to make short wrappers more compact
             */
            if ( trim( line ).length() == 0 )
            {
                output_line = false;
            }
        }
        //  output  potentially  modifed  line
        if ( output_line )
        {
            cout << line << "\n";
        }
    }

    return false;
}

/**
 * Initialize internal data structures
 */
void
SCOREP::Wrapgen::init
    ()
{
    /* initialize handlers for MPI templates */
    handler::mpi::_initialize();
}

/**
 * Main program
 */
int
main
(
    int    argc,
    char** argv
)
{
    // no arguments: print usage message
    if ( argc == 1 )
    {
        help( argv[ 0 ] );
    }

    // initialize internal data structures
    SCOREP::Wrapgen::init();

    // set global options
    process_cmd_line( argc, argv );

    // read MPI function prototype description file
    if ( opts.prototypes.rfind( ".xml" ) == opts.prototypes.length() - 4 )
    {
        read_xml_prototypes( opts.prototypes.c_str(), mpiFuncs );
    }
    else
    {
        read_prototypes( opts.prototypes.c_str(), mpiFuncs );
    }

    // read MPI wrapper generation specification files
    if ( !opts.suppressbanner )
    {
        cout << "/*-----------------------------------------------------------------------*/\n";
        cout << "/*-- Automatically generated by wrapgen. Do not change. -----------------*/\n";
        cout << "/*-----------------------------------------------------------------------*/\n\n";
    }

    for ( int n = 1; optind < argc; optind++ )
    {
        n = optind;
        string aname( argv[ n ] );

        if ( aname.rfind( ".txt" ) == ( aname.size() - 4 ) )
        {
            // arguments end in ".txt"
            // considered to be comments or additional code
            // output of the textfiles in the specified order
            handle_textfile( argv[ n ] );
        }
        else if ( aname.rfind( ".w" ) == ( aname.size() - 2 ) )
        {
            // argument ends in ".w"
            // considered to be wrapper template file
            handle_wrapper_template( argv[ n ] );
        }
        else if ( aname.rfind( ".tmpl" ) == ( aname.size() - 5 ) )
        {
            handle_file_template( argv[ n ] );
        }
    }

    return EXIT_SUCCESS;
}
