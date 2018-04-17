/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015-2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_LIBRARY_WRAPPER_GENERATOR_PARSER_HPP
#define SCOREP_LIBRARY_WRAPPER_GENERATOR_PARSER_HPP

#include <deque>
#include <vector>
#include <string>
#include <set>
#include <map>

#include <clang-c/Index.h>

#include <SCOREP_Filter.h>

struct macro_information
{
    std::string              filename;
    std::string              functionname;
    std::string              symbolname;
    std::string              va_list_symbol;
    bool                     hasreturn;
    std::string              returntype;
    std::vector<std::string> argdecls;
    std::vector<std::string> argnames;
    unsigned int             linenr;
};

struct name_space
{
    std::string                   name;
    name_space*                   parent;
    bool                          empty;
    std::vector<name_space*>      sub_namespaces;
    std::deque<macro_information> functions;
};

class SCOREP_Libwrap_IterateNamespaceCb
{
public:
    virtual ~SCOREP_Libwrap_IterateNamespaceCb()
    {
    }
    virtual void
    operator()( const macro_information& decl )
    {
    }
    virtual void
    operator()( const std::string& ns,
                bool               openClose )
    {
    }
};

void
SCOREP_Libwrap_IterateNamespace( const name_space&                  ns,
                                 SCOREP_Libwrap_IterateNamespaceCb& cb );

class SCOREP_Libwrap_Parser
{
public:
    SCOREP_Libwrap_Parser( int verbse,
                           const std::string& filename,
                           const std::string& language,
                           const std::string& languageStandard,
                           const std::set<std::string>& variadicIsVoid,
                           const std::map<std::string, std::string>& ellipsisMapping,
                           const SCOREP_Filter* filter );
    ~SCOREP_Libwrap_Parser();

    bool
    ParseHeader();

    enum CXChildVisitResult
    Visit( CXCursor cursor,
           CXCursor parent );

    void
    AddFunctionDecl( CXCursor           cursor,
                     const std::string& symbolName,
                     const std::string& filename,
                     unsigned           line,
                     bool               memberMethod );

    const name_space*
    GetToplevelNamespace() const
    {
        return m_toplevel_namespace;
    };

    const std::set<std::string>&
    GetAllWrappedSymbols() const
    {
        return m_all_wrapped_symbols;
    }

private:
    int                                m_verbose;
    name_space*                        m_current_namespace;
    name_space*                        m_toplevel_namespace;
    CXTranslationUnit                  m_tu;
    CXIndex                            m_index;
    CXCursor                           m_start_cursor;
    CXCursorSet                        m_canonical_cursors;
    std::set<std::string>              m_variadic_is_void;
    std::map<std::string, std::string> m_ellipsis_mapping;
    std::set<std::string>              m_all_wrapped_symbols;
    const SCOREP_Filter*               m_filter;
};

#endif // SCOREP_LIBRARY_WRAPPER_GENERATOR_PARSER_HPP
