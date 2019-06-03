/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015-2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_library_wrapper_generator_parser.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/* clang_Cursor_isFunctionInlined comes with clang 3.9
 * we use as an fallback clang_isCursorDefinition, but this does not work
 * if we have CXTranslationUnit_SkipFunctionBodies set
 */
#if CINDEX_VERSION < CINDEX_VERSION_ENCODE( 0, 34 )
#define clang_Cursor_isFunctionInlined( cursor ) clang_isCursorDefinition( cursor )
#endif


SCOREP_Libwrap_Parser::SCOREP_Libwrap_Parser( int verbose,
                                              const string& filename,
                                              const string& language,
                                              const string& languageStandard,
                                              const set<string>& variadicIsVoid,
                                              const map<string, string>& ellipsisMapping,
                                              const SCOREP_Filter* filter )
    : m_verbose( verbose )
    , m_variadic_is_void( variadicIsVoid )
    , m_ellipsis_mapping( ellipsisMapping )
    , m_filter( filter )
{
    vector<string> string_vector;
    string_vector.push_back( "-ferror-limit=0" );
    string_vector.push_back( "-x" );
    string_vector.push_back( language );
    if ( languageStandard.size() )
    {
        string_vector.push_back( "-std=" + languageStandard );
    }

    /* Convert vector<string> to char*[] */
    unsigned int num_arguments = string_vector.size();
    const char** arguments     = new const char*[ num_arguments ];
    for ( unsigned int i = 0; i < num_arguments; i++ )
    {
        arguments[ i ] = string_vector[ i ].c_str();
    }

    m_index = clang_createIndex( 0,                /* int excludeDeclarationsFromPCH: 0 - false, 1 - true */
                                 m_verbose >= 1 ); /* int displayDiagnostics: 0 - false, 1 - true */

    m_tu = clang_parseTranslationUnit( m_index,
                                       filename.c_str(),
                                       arguments,
                                       num_arguments,
                                       0, 0,
                                       CXTranslationUnit_DetailedPreprocessingRecord
                                       | CXTranslationUnit_SkipFunctionBodies );
    m_start_cursor = clang_getTranslationUnitCursor( m_tu );

    m_canonical_cursors = clang_createCXCursorSet();

    m_toplevel_namespace        = m_current_namespace = new name_space;
    m_toplevel_namespace->empty = true;

    /* Clean up */
    delete[] arguments;
}

SCOREP_Libwrap_Parser::~SCOREP_Libwrap_Parser()
{
    clang_disposeTranslationUnit( m_tu );
    clang_disposeIndex( m_index );
    clang_disposeCXCursorSet( m_canonical_cursors );
}

static enum CXChildVisitResult
visit( CXCursor     cursor,
       CXCursor     parent,
       CXClientData clientData )
{
    SCOREP_Libwrap_Parser* p_this = static_cast<SCOREP_Libwrap_Parser*>( clientData );
    return p_this->Visit( cursor, parent );
}

bool
SCOREP_Libwrap_Parser::ParseHeader()
{
    if ( clang_Cursor_isNull( m_start_cursor ) )
    {
        return false;
    }

    clang_visitChildren( m_start_cursor, ::visit, this );

    return true;
}

static inline string
from_cx( CXString s )
{
    string result( clang_getCString( s ) );
    clang_disposeString( s );
    return result;
}

bool
is_in_class_template( CXCursor cursor )
{
    CXCursorKind kind = clang_getCursorKind( cursor );

    if ( clang_Cursor_isNull( cursor ) || kind == clang_isTranslationUnit( kind ) )
    {
        return false;
    }
    else if ( kind == CXCursor_ClassTemplate || kind == CXCursor_ClassTemplatePartialSpecialization )
    {
        return true;
    }
    else
    {
        return is_in_class_template( clang_getCursorSemanticParent( cursor ) );
    }
}

/**
 * This visitor function will be invoked for each cursor found by
 * clang_visitCursorChildren().
 *
 * @cursor          Cursor being visited
 * @parent          Parent visitor for that cursor
 * @clientData      User defined data
 */
enum CXChildVisitResult
SCOREP_Libwrap_Parser::Visit( CXCursor cursor,
                              CXCursor parent )
{
    string       cursor_name( from_cx( clang_getCursorSpelling( cursor ) ) );
    CXCursorKind cursor_kind = clang_getCursorKind( cursor );

    CXString cx_filename;
    unsigned line, column;
    clang_getPresumedLocation( clang_getCursorLocation( cursor ),
                               &cx_filename,
                               &line,
                               &column );
    string filename( from_cx( cx_filename ) );

    if ( cursor_kind == CXCursor_Namespace )
    {
        name_space* sub_namespace = 0;
        for ( size_t i = 0; i < m_current_namespace->sub_namespaces.size(); ++i )
        {
            name_space* sub_namespace = m_current_namespace->sub_namespaces[ i ];
            if ( sub_namespace->name == cursor_name )
            {
                break;
            }
            sub_namespace = 0;
        }
        if ( sub_namespace == 0 )
        {
            sub_namespace         = new name_space;
            sub_namespace->name   = cursor_name;
            sub_namespace->parent = m_current_namespace;
            sub_namespace->empty  = true;
            m_current_namespace->sub_namespaces.push_back( sub_namespace );
        }

        m_current_namespace = sub_namespace;
        clang_visitChildren( cursor, ::visit, this );

        if ( !m_current_namespace->empty )
        {
            m_current_namespace->parent->empty = false;
        }
        m_current_namespace = m_current_namespace->parent;

        return CXChildVisit_Continue;
    }

    if ( cursor_kind != CXCursor_FunctionDecl
         && cursor_kind != CXCursor_CXXMethod
         && cursor_kind != CXCursor_Constructor
         && cursor_kind != CXCursor_Destructor )
    {
        return CXChildVisit_Recurse;
    }

    int filtered = false;
    if ( !filename.empty()
         && filename != "libwrap.i"
         && SCOREP_Filter_MatchFile( m_filter, filename.c_str(), &filtered ) == SCOREP_SUCCESS
         && filtered )
    {
        return CXChildVisit_Recurse;
    }

    bool duplicate_declaration = false;
    if ( clang_isDeclaration( cursor_kind ) )
    {
        /* returns false, if the cursor is already in the set */
        duplicate_declaration = !clang_CXCursorSet_insert( m_canonical_cursors, clang_getCanonicalCursor( cursor ) );
    }

    /* just parse function declarations */
    if ( !duplicate_declaration
         && cursor_kind == CXCursor_FunctionDecl
         && clang_getCursorLinkage( cursor ) == CXLinkage_External
         && !clang_Cursor_isFunctionInlined( cursor ) )
    {
        string name( from_cx( clang_Cursor_getMangling( cursor ) ) );

        /* We get the empty string if the symbol wont be mangled,
         * but only since clang 3.8
         * workaround to get the correct function name for extern "C" functions
         * if it starts with "_Z" and ends in cursor_name, than it looks like
         * a C function was mangled.
         */
        stringstream mangled;
        mangled << "_Z" << cursor_name.size() << cursor_name;
        if ( name == "" || name.compare( mangled.str() ) == 0 )
        {
            name = cursor_name;
        }

        AddFunctionDecl( cursor, name, filename, line, false );
    }
    else if ( !duplicate_declaration
              && cursor_kind == CXCursor_CXXMethod
              && ( clang_getCursorLinkage( cursor ) == CXLinkage_External
                   || clang_getCursorLinkage( cursor ) == CXLinkage_UniqueExternal )
              && clang_getCXXAccessSpecifier( cursor ) == CX_CXXPublic
              && !clang_Cursor_isFunctionInlined( cursor )
              && !is_in_class_template( cursor ) )
    {
        string name( from_cx( clang_Cursor_getMangling( cursor ) ) );
        AddFunctionDecl( cursor, name, filename, line,
                         clang_CXXMethod_isStatic( cursor ) ? false : true );
    }
    else if ( !duplicate_declaration
              && ( cursor_kind == CXCursor_Constructor
                   || cursor_kind == CXCursor_Destructor )
              && clang_getCXXAccessSpecifier( cursor ) == CX_CXXPublic
              && !clang_Cursor_isFunctionInlined( cursor )
              && !is_in_class_template( cursor ) )
    {
#if CINDEX_VERSION >= CINDEX_VERSION_ENCODE( 0, 32 )
        CXStringSet* manglings = clang_Cursor_getCXXManglings( cursor );
        for ( unsigned i = 0; i < manglings->Count; i++ )
        {
            string name = clang_getCString( manglings->Strings[ i ] );
            AddFunctionDecl( cursor, name, filename, line, true );
        }
        clang_disposeStringSet( manglings );
#else
        string name( from_cx( clang_Cursor_getMangling( cursor ) ) );
        AddFunctionDecl( cursor, name, filename, line, true );
#endif
    }

    return CXChildVisit_Continue;
}

static string
fully_qualified_name( CXCursor cursor )
{
    if ( clang_Cursor_isNull( cursor ) || clang_isTranslationUnit( clang_getCursorKind( cursor ) ) )
    {
        return "";
    }

    string parentName = fully_qualified_name( clang_getCursorSemanticParent( cursor ) );

    if ( parentName.empty() == false )
    {
        parentName += "::";
    }

    return parentName + from_cx( clang_getCursorDisplayName( cursor ) );
}

static bool
contains_function_proto_or_const_array( CXType t )
{
    do
    {
        if ( t.kind == CXType_FunctionProto || t.kind == CXType_ConstantArray )
        {
            return true;
        }
        else if ( t.kind == CXType_Pointer )
        {
            t = clang_getPointeeType( t );
        }
        else
        {
            return false;
        }
    }
    while ( true );
}

static string
put_spaces_back_into_arg_type( string arg_type, char parenthesis )
{
    if ( parenthesis != '(' && parenthesis != '[' )
    {
        cerr << "[Score-P] Internal error: Wrong use of function put_spaces_back_into_arg_type with \'" << parenthesis << "\'." << endl
             << "          Report this to support@scorep.org. Ignoring." << endl;
        return arg_type;
    }

    size_t fpos = 0;
    do
    {
        fpos = arg_type.find( parenthesis, fpos );
        if ( fpos == string::npos )
        {
            break;
        }
        else if ( fpos > 0 && ( arg_type[ fpos - 1 ] == ' ' || arg_type[ fpos - 1 ] == '*' ||
                                ( parenthesis == '[' && arg_type[ fpos - 1 ] == ']' ) ||
                                ( parenthesis == '[' && arg_type[ fpos - 1 ] == ')' ) ||
                                ( parenthesis == '(' && arg_type[ fpos - 1 ] == ')' ) ) )
        {
        }
        else if ( fpos > 0 && ( isalnum( arg_type[ fpos - 1 ] ) || arg_type[ fpos - 1 ] == '_' ) )
        {
            arg_type.insert( fpos, " " );
        }
        else if ( fpos > 0 )
        {
            cerr << "[Score-P] Warning: Unexpected character \'" << arg_type[ fpos - 1 ] << "\' in type in front of \'" << parenthesis << "\'. "
                 << " Report this to support@scorep.org" << endl;
        }
        fpos += 1;
    }
    while ( fpos != string::npos && fpos < arg_type.size() );

    return arg_type;
}

void
SCOREP_Libwrap_Parser::AddFunctionDecl( CXCursor      cursor,
                                        const string& symbolName,
                                        const string& filename,
                                        unsigned      line,
                                        bool          memberMethod )
{
    string full_name = fully_qualified_name( cursor );

    if ( symbolName.empty() )
    {
        if ( m_verbose >= 0 )
        {
            cerr << "[Score-P] " << filename << ":" << line << ": Warning:"
                 << " Ignoring function with empty symbol name: '" << full_name << "'." << endl;
        }
        return;
    }

    macro_information function_decl;
    function_decl.filename     = filename;
    function_decl.functionname = full_name;
    function_decl.symbolname   = symbolName;
    function_decl.linenr       = line;

    /* Do never wrap __builtin_* functions. */
    string prefix_builtin( "__builtin_" );
    if ( 0 == function_decl.symbolname.compare( 0, prefix_builtin.size(), prefix_builtin ) )
    {
        return;
    }

    /* Check the filter first, so that the user is not confused by the
     * ignore message from the variadic check.
     * we already checked the filename earlier on.
     */
    int filtered = false;
    if ( SCOREP_Filter_MatchFunction( m_filter,
                                      function_decl.functionname.c_str(),
                                      function_decl.symbolname.c_str(),
                                      &filtered ) == SCOREP_SUCCESS
         && filtered )
    {
        return;
    }

    if ( m_all_wrapped_symbols.count( function_decl.symbolname ) != 0 )
    {
        /* duplciate, print warning again */
        return;
    }
    m_all_wrapped_symbols.insert( function_decl.symbolname );

    CXType type = clang_getCursorType( cursor );
    if ( clang_isFunctionTypeVariadic( type ) )
    {
        if ( clang_Cursor_getNumArguments( cursor ) == 0 )
        {
            if ( m_variadic_is_void.count( function_decl.symbolname ) == 0 )
            {
                if ( m_verbose >= 0 )
                {
                    cerr << "[Score-P] " << filename << ":" << line << ": Warning:"
                         << " Ignoring function with unknown argument list: '" << function_decl.functionname << "'."
                         << " If this function is known to have no arguments, add the symbol '" << function_decl.symbolname << "' to"
                         << " the LIBWRAP_VARIADIC_IS_VOID_SYMBOLS variable in the Makefile." << endl;
                }
                return;
            }
        }
        else
        {
            if ( m_ellipsis_mapping.count( function_decl.symbolname ) == 0 )
            {
                /* Variadic functions args can only be passed via va_list to a another symbol.
                 * This matching must be done manually. */
                if ( m_verbose >= 0 )
                {
                    cerr << "[Score-P] " << filename << ":" << line << ": Warning:"
                         << " Ignoring variadic function: '" << function_decl.functionname << "'."
                         << " If this function has a 'va_list' variant, add '" << function_decl.symbolname << ":valistvariantfunction' to"
                         << " the LIBWRAP_ELLIPSIS_MAPPING_SYMBOLS variable in the Makefile." << endl;
                }
                return;
            }
            else
            {
                function_decl.va_list_symbol = m_ellipsis_mapping[ function_decl.symbolname  ];
            }
        }
    }

    CXType ret = clang_getCursorResultType( cursor );

    if ( contains_function_proto_or_const_array( clang_getCanonicalType( ret ) ) == false )
    {
        ret = clang_getCanonicalType( ret );
    }

    function_decl.returntype = from_cx( clang_getTypeSpelling( ret ) );
    function_decl.hasreturn  = ret.kind != CXType_Void;
    int nargs = clang_Cursor_getNumArguments( cursor );

    /* Add 'this' argument to member methods and constructors */
    if ( memberMethod )
    {
        function_decl.argnames.push_back( "scorep_libwrap_arg_this" );
        function_decl.argdecls.push_back( "void *scorep_libwrap_arg_this" );
    }

    for ( int i = 0; i < nargs; i++ )
    {
        CXCursor param_cursor = clang_Cursor_getArgument( cursor, i );
        if ( clang_Cursor_isNull( param_cursor ) )
        {
            if ( m_verbose >= 0 )
            {
                cerr << "[Score-P] " << filename << ":" << line << ": Warning:"
                     << " Ignoring function with invalid argument " << ( i + 1 ) << ": '" << function_decl.functionname << "'" << endl;
            }
            return;
        }

        string param_name( from_cx( clang_getCursorSpelling( param_cursor ) ) );
        if ( param_name == "" )
        {
            stringstream new_name;
            new_name << "scorep_libwrap_arg_" << ( i + 1 );
            param_name = new_name.str();
        }
        CXType pc_type           = clang_getCursorType( param_cursor );
        CXType pc_canonical_type = clang_getCanonicalType( pc_type );

        string arg_type_normal( from_cx( clang_getTypeSpelling( pc_type ) ) );
        string arg_type_canonical( from_cx( clang_getTypeSpelling( pc_canonical_type ) ) );

        string arg_type;
        if ( arg_type_normal == "va_list" ) // kind is CXType_Typedef, and would be changed to the incomplete type struct __va_list_tag vl[1]
        {
            arg_type = arg_type_normal;
        }
        else
        {
            arg_type = arg_type_canonical;
        }

        arg_type = put_spaces_back_into_arg_type( arg_type, '(' );
        arg_type = put_spaces_back_into_arg_type( arg_type, '[' );

        /* Catch function pointer types, first, as a function pointer may have array args */
        size_t ppos = arg_type.find( "(*" );

        /* Catch function arguments without pointer, i.e. "int (void)" */
        size_t fpos = arg_type.find( " (" );
        if ( fpos == string::npos )
        {
            /* Catch function arguments without pointer and pointer return type, i.e. "void *(void)" */
            fpos = arg_type.find( "*(" );
        }

        /* Catch array args */
        size_t bpos = arg_type.find( " [" );
        if ( bpos == string::npos )
        {
            /* Catch array of pointer args  */
            bpos = arg_type.find( "*[" );
        }

        if ( ppos != string::npos )
        {
            /* after the initial "(*" pattern may follow more "*"
             * maybe also array rackets "[]" before the closing paranthese
             * thus skip all "*" */
            arg_type.insert( arg_type.find_first_not_of( "*", ppos + 1 ), param_name );
        }
        else if ( fpos != string::npos )
        {
            arg_type.insert( fpos + 1, "(" + param_name + ")" );
        }
        else if ( bpos != string::npos )
        {
            arg_type.insert( bpos + 1, param_name );
        }
        else
        {
            arg_type.append( " " );
            arg_type.append( param_name );
        }

        function_decl.argnames.push_back( param_name );
        function_decl.argdecls.push_back( arg_type );
    }

    if ( function_decl.va_list_symbol.length() )
    {
        function_decl.argdecls.push_back( "..." );
        function_decl.argnames.push_back( "scorep_libwrap_var_vl" );
    }

    m_current_namespace->functions.push_back( function_decl );
    m_current_namespace->empty = false;
}

void
SCOREP_Libwrap_IterateNamespace( const name_space&                  ns,
                                 SCOREP_Libwrap_IterateNamespaceCb& cb )
{
    if ( ns.empty )
    {
        return;
    }

    /* Enter namespace */
    cb( ns.name, true );

    /* Descent into sub namespaces */
    vector<name_space*>::const_iterator ns_it = ns.sub_namespaces.begin();
    while ( ns_it != ns.sub_namespaces.end() )
    {
        SCOREP_Libwrap_IterateNamespace( **ns_it, cb );
        ++ns_it;
    }

    /* Iterate over the function decls */
    deque<macro_information>::const_iterator decl_it = ns.functions.begin();
    while ( decl_it != ns.functions.end() )
    {
        cb( *decl_it );
        ++decl_it;
    }

    /* Leave namespace */
    cb( ns.name, false );
}
