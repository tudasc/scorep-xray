/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
 * @file SILC_Wrapgen.cpp
 *
 * Wrapper generator for automatic creation of MPI wrappers
 */
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

#include "SILC_Wrapgen_Help.h"
#include "SILC_Wrapgen.h"
#include "SILC_Wrapgen_Util.h"

#include "SILC_Wrapgen_Handlers.h"
#include "SILC_Wrapgen_HandlersMpi.h"

#include "SILC_Wrapgen_Funcparam.h"
#include "SILC_Wrapgen_Func.h"
#include "SILC_Wrapgen_MpiFunc.h"
using namespace SILC::Wrapgen;

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

namespace {
SILC::Wrapgen::option opts;

/** Partial predifinitions of function wrappers */
map<string, string> partial_predef;

/** Complete predifinitions of function wrappers */
map<string, string> complete_predef;

/** All MPI function prototypes */
map<string, MPIFunc> mpiFuncs;
} // End of anonymous namespace

/**
 * Read predefined wrapper core parts from a stream
 */
void
SILC::Wrapgen::read_predefined
(
    const char* filename,
    map<string, string>& predef
)
{
    string   predefline;
    string   predefbody;
    string   predefname;

    ifstream pdstream;
    OPEN_STREAM( pdstream, filename );

    predefline = "";
    while ( getline( pdstream, predefline ) &&
            ( predefline.find( "#END" ) != string::npos ) )
    {
        if ( predefbody == "" )
        {
            predefname = predefline;
        }
        else
        {
            if ( predefline.find( "#NEXT" ) != string::npos )
            {
                /* store predefined wrapper and reinitialize */
                predef[ predefname ] = predefbody;
                predefbody           = "";
            }
            else
            {
                /* append line to predefined wrapper body */
                predefbody += predefline + "\n";
            }
        }
    }
}

/**
 * Process commandline options and set variables accordingly
 * @param argc Number of entries in argv
 * @param argv Command line options
 */
void
SILC::Wrapgen::process_cmd_line
(
    int&    argc,
    char**& argv
)
{
    int copt;
    opts.suppressbanner  = false;
    opts.restriction     = "";
    opts.partial_predef  = "";
    opts.complete_predef = "";
    opts.prototypes      = "";
    opts.counts          = "";
    opts.guard           = "";
    opts.banner          = "";

    while ( ( copt = getopt( argc, argv, "bB:c:C:D:hp:P:r:" ) ) != EOF )
    {
        switch ( copt )
        {
            case 'b':
            {
                // if bannerflag is activated, there is no banner ouput on the console
                opts.suppressbanner = true;
                break;
            }
            case 'B':
            {
                opts.banner = optarg;
                break;
            }
            case 'c':
            {
                opts.counts = optarg;
                break;
            }
            case 'C':
            {
                opts.complete_predef = optarg;
                break;
            }
            case 'D':
            {
                opts.guard = optarg;
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
            case 'P':
            {
                opts.partial_predef = optarg;
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
SILC::Wrapgen::read_prototypes
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
            string arg_type, arg_name, arg_suffix;
            getline( proto, arg_type );
            getline( proto, arg_name );
            getline( proto, arg_suffix );
            Funcparam arg = Funcparam( arg_type, arg_name, arg_suffix,
                                       atype[ a ] );
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
SILC::Wrapgen::read_xml_prototypes
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
                    string      name, rtype, group;
                    string      version, send = "0", recv = "0";
                    string      decl, xblock;
                    string      guard;
                    paramlist_t params;

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
                                string pname, type, suffix;
                                char   access;

                                XML_GET_STR_ATTR( el, "type", type );
                                XML_GET_STR_ATTR( el, "name", pname );
                                XML_GET_CHAR_ATTR( el, "access", access );
                                XML_GET_STR_ATTR( el, "suffix", suffix );

                                params.push_back( Funcparam( type, pname,
                                                             suffix, access ) );
                            }
                            if ( xmlStrcmp( el->name, ( const xmlChar* )
                                            "decl" ) == 0 )
                            {
                                xmlChar* content = NULL;
                                content = xmlNodeGetContent( el );
                                if ( content )
                                {
                                    decl = string( ( const char* )content );
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
                                    xblock = string( ( const char* )content );
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
                        f.set_decl_block( decl );
                        f.set_expr_block( xblock );
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
SILC::Wrapgen::write_xml_prototypes
(
    const char* filename,
    map<string, MPIFunc>& funcs
)
{
    map<string, MPIFunc>::iterator it;
    ofstream
    pconf
    (
        filename
    );

    pconf << "<prototypes count=\"" << funcs.size() << "\" family=\"mpi\" >\n";

    for ( it = funcs.begin(); it != funcs.end(); ++it )
    {
        pconf << it->second.write_conf();
    }
    pconf << "</prototypes>" << endl;
    pconf.close();
}

void
SILC::Wrapgen::read_count_spec
(
    const char* filename,
    map<string, MPIFunc>& funcs
)
{
    string line;

    // read MPI function transferred counts description file
    ifstream cnt;
    OPEN_STREAM( cnt, opts.counts.c_str() );

    // Format: sequence of
    //   MPIFunc
    //   SendAmountExpression
    //   RecvAmountExpression
    while ( getline( cnt, line ) )
    {
        map<string, MPIFunc>::iterator it = mpiFuncs.find( line );
        if ( it == mpiFuncs.end() )
        {
            cerr << "ERROR: unknown function '" << line
                 << "' in '" << opts.counts << "'\n";
            ignoreRestOfLine( cnt );
            ignoreRestOfLine( cnt );
        }
        else
        {
            MPIFunc& func = it->second;
            string   rule;
            getline( cnt, rule );
            func.set_sendcount_rule( rule );
            getline( cnt, rule );
            func.set_recvcount_rule( rule );
        }
    }
}

void
SILC::Wrapgen::print_banner
(
    const string& str
)
{
    cout << "/*\n"
         << " *-----------------------------------------------------------------------------\n"
         << " * \n * " << str << "\n *\n"
         << " *-----------------------------------------------------------------------------\n"
         << " */\n" << endl;
}

/**
 * Process a textfile by passing it directly to stdout
 * @param filename Filename of the textfile
 */
void
SILC::Wrapgen::handle_textfile
(
    const char* filename
)
{
    ifstream
    textfile
    (
        filename
    );

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
SILC::Wrapgen::handle_wrapper_template
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
SILC::Wrapgen::handle_file_template
(
    const char* filename
)
{
    string   src_line;
    ifstream file_tmpl;
    OPEN_STREAM( file_tmpl, filename );

    while ( getline( file_tmpl, src_line ) )
    {
        if ( int pos = src_line.find( "#pragma wrapgen" ) != string::npos )
        {
            string scope, cmd, wrapper_tmpl;
            istringstream
            line_stream
            (
                src_line.substr( pos + 15 )
            );

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
                string op;      // operator for choosing multiple functions
                string rule;    // parameter of operator
                size_t ppos;    // offset of opening bracket
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
                        string
                        rule
                        (
                            cmd.substr( pos + 8, cmd.length() - pos - 9 )
                        );

                        generateOutput( it->second, wrapper_tmpl.c_str(),
                                        rule );
                    }
                }
                else if ( op.compare( "regex" ) == 0 )
                {
                    regex_t    pattern;
                    regmatch_t match;
                    int        ret;

                    // TODO evaluate return value of patter search
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
 * Process a specification file, which contains a list of
 * functionname/templatename pairs
 * @param filename Filename of the specification file
 */

int
SILC::Wrapgen::handle_spec_file
(
    const char* filename
)
{
    // Format: sequence of "MPIFunc WrapperToUse.w"
    ifstream spec;
    OPEN_STREAM( spec, filename );

    string name;
    string wrappertype;
    string newline;
    while ( getline( spec, newline ) )
    {
        if ( newline[ 0 ] == '#' )
        {
            // print_banner(newline.substr(1));
        }
        else
        {
            string wrapperfile;
            istringstream
            oss
            (
                newline
            );

            oss >> name >> wrappertype;
            wrapperfile.append( wrappertype );
            map<string, MPIFunc>::iterator it = mpiFuncs.find( name );
            if ( it == mpiFuncs.end() )
            {
                cerr << "ERROR: unknown function '" << name << "' in '"
                     << filename << "'\n";
            }
            else
            {
                if ( generateOutput( it->second, wrapperfile.c_str(),
                                     opts.restriction ) )
                {
                    return 1;
                }
            }
        }
    }

    return 1;
}

/**
 * Search for next template variable. If a template variable is found,
 * the parameter 'key' is overwritten with the variable name found. If
 * no template variable is found, 'keý' is left unchanged.
 * @param str string searched for
 * @param key key of template variable found
 * @return position of template variable in string
 */
size_t
find_variable
(
    const string& str,
    size_t        p,
    string&       key
)
{
    size_t pos = str.rfind( "$", p );

    if ( pos != string::npos )
    {
        if ( str[ pos + 1 ] == '{' )
        {
            // parse long variable name
            size_t endpos = string::npos;

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
SILC::Wrapgen::is_selected
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
        string rule_body  = ( *tok ).substr( scope_offset + 1, string::npos );

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
 * @return TODO
 */
bool
SILC::Wrapgen::generateOutput
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
SILC::Wrapgen::init
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
    SILC::Wrapgen::init();

    // set global options
    process_cmd_line( argc, argv );

    //read predefined from predef.in
    if ( !opts.partial_predef.empty() )
    {
        read_predefined( opts.partial_predef.c_str(), partial_predef );
    }

    //read predefined from predeff2c.in
    if ( !opts.complete_predef.empty() )
    {
        read_predefined( opts.complete_predef.c_str(), complete_predef );
    }

    // read MPI function prototype description file
    if ( opts.prototypes.rfind( ".xml" ) == opts.prototypes.length() - 4 )
    {
        read_xml_prototypes( opts.prototypes.c_str(), mpiFuncs );
    }
    else
    {
        read_prototypes( opts.prototypes.c_str(), mpiFuncs );
    }

    // read Count specification and update function information
    if ( !opts.counts.empty() )
    {
        read_count_spec( opts.counts.c_str(), mpiFuncs );
    }

    // read MPI wrapper generation specification files
    if ( ( argc > 1 ) && !opts.suppressbanner )
    {
        cout << "/*-----------------------------------------------------------------------*/\n";
        cout << "/*-- Automatically generated by wrapgen. Do not change. -----------------*/\n";
        cout << "/*-----------------------------------------------------------------------*/\n\n";
    }

    for ( int n = 1; optind < argc; optind++ )
    {
        n = optind;
        string
        aname
        (
            argv[ n ]
        );

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
        else
        {
            if ( opts.banner != "" )
            {
                cout << "\n/* " << opts.banner << " */\n";
                if ( opts.guard == "" )
                {
                    cout << "\n";
                }
            }
            if ( opts.guard != "" )
            {
                cout << "\n#ifdef " << opts.guard << "\n\n";
            }
            // argument considered to be specification file
            handle_spec_file( argv[ n ] );
            if ( opts.guard != "" )
            {
                cout << "\n#endif" "\n\n";
            }
        }
    }

    //write_xml_prototypes("skel/prototypes", mpiFuncs);
    /*
       set<string> wrap_groups;
       for (map<string,MPIFunc>::const_iterator it=mpiFuncs.begin();
            it != mpiFuncs.end(); ++it)
       {
        wrap_groups.insert(it->second.get_group());
       }
       for (set<string>::const_iterator it=wrap_groups.begin();
            it != wrap_groups.end(); ++it)
       {
        cerr << *it << endl;
       }
     */

    return EXIT_SUCCESS;
}
