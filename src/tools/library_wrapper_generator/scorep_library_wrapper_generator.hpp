/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_LIBRARY_WRAPPER_GENERATOR_HPP
#define SCOREP_LIBRARY_WRAPPER_GENERATOR_HPP

#include <deque>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <set>

#include "scorep_library_wrapper_generator_parser.hpp"

#include <SCOREP_Filter.h>

struct generator_configuration
{
    int                                verbose;
    std::string                        wrapper_name;
    std::string                        display_name;
    std::string                        output_directory;
    /** allowed to overwrite existing files? */
    bool                               overwrite;
    /** Path to header file (e.g., /opt/netcdf/include/netcdf.h) */
    std::string                        header_file;
    /** Path to library (e.g., /opt/netcdf/lib/libnetcdf.so) */
    std::vector<std::string>           library_names;
    /** The include statement in the generated code (e.g., "<netcdf.h>") */
    std::string                        include_statement;
    std::string                        filter_file_name;
    SCOREP_Filter*                     filter;
    std::string                        wrap_file_name;
    /** Name of the file containing the forward declarations of the wrapper functions */
    std::string                        function_list_file_name;
    /** Whether to create a file containing the wrapper functions for a Score-P internal adapter */
    bool                               create_internal_wrapper_code_file;
    /** Name of GNU ld -wrap flags */
    std::string                        wrap_flags_file_name;
    /** Name of GNU ld -wrap flags, for nvcc */
    std::string                        nvcc_wrap_flags_file_name;
    /** The language in which the parser should parse the header "c" or "c++" */
    std::string                        language;
    /** The language standard in which the parser should parse the header "c99", "c++11, ..." */
    std::string                        language_standard;
    /** Set of symbols which have an unknown argument list, but should be treated as having not arguments at all */
    std::set<std::string>              variadic_is_void;
    /** Map of symbols which have an ellipsis argument, map target is the va_list variant of this symbol */
    std::map<std::string, std::string> ellipsis_mapping;
};


class SCOREP_Libwrap_Generator
{
public:

    SCOREP_Libwrap_Generator( const generator_configuration& config );
    ~SCOREP_Libwrap_Generator();

    int
    generate_source_code_files();

    bool
    noSymbolsWrapped() const;

private:

    enum include_mode
    {
        INCLUDE_LOCAL_FILE,
        INCLUDE_GLOBAL_FILE
    };

    enum output_mode
    {
        OUTPUT_INTERNAL_ADAPTER_CODE,
        OUTPUT_EXTERNAL_ADAPTER_CODE,
        OUTPUT_FUNCTION_LIST,
        OUTPUT_WRAP_FLAGS,
        OUTPUT_NVCC_WRAP_FLAGS
    };

    friend class macro_writer_adapter;
    friend class macro_writer_function_list;
    friend class macro_writer_wrap_flags;
    friend class macro_writer_nvcc_wrap_flags;

    std::string                    m_output_file;
    const generator_configuration& m_config;
    std::vector<std::string>       m_includes;
    const name_space*              m_toplevel_namespace;
    std::set<std::string>          m_all_wrapped_symbols;

    void
    set_outputfile_name( const std::string& );

    const std::string&
    get_outputfile_name() const;

    void
    clear_outputfile();

    void
    output( output_mode mode );

    void
    add_include( const std::string& includeFile,
                 include_mode       includeMode );

    const std::string
    argdecls_iterate( const macro_information& data ) const;

    const std::string
    argnames_iterate( const macro_information& data ) const;

    const std::string
    iterate_includes() const;

    void
    write_wrapper_code( const macro_information& data,
                        std::ostream&            out ) const;

    void
    write_internal_wrapper_code( const macro_information& data,
                                 std::ostream&            out ) const;

    void
    write_function_process_macro( const macro_information& data,
                                  const std::string&       enclosingNamespace,
                                  std::ostream&            out ) const;
};

#endif // SCOREP_LIBRARY_WRAPPER_GENERATOR_HPP
