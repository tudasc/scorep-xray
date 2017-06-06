/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief  The definition for inserting the GCC plugin instrumentation descriptors.
 *
 */

#include "tree.h"

#include "libiberty.h"
#include "filenames.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "function.h"
#include "toplev.h"
#include "tree.h"
#include "input.h"
#include "hashtab.h"
#include "splay-tree.h"
#include "langhooks.h"
#include "cgraph.h"
#include "intl.h"
#include "function.h"
#include "diagnostic.h"
#include "timevar.h"
#include "tree-iterator.h"
#include "scorep_plugin_tree-flow.h"
#include "tree-pass.h"
#include "plugin.h"
#include "gimple.h"
#include "tree-pass.h"

#include "scorep_plugin_gcc_version_compatibility.h"
#include "scorep_plugin.h"
#include "scorep_plugin_inst_descriptor.h"

static const char* region_descr_var_name        = "__scorep_region_descr";
static const char* region_descrs_section        = ".scorep.region.descrs";
static const char* struct_region_descr_var_name = "__scorep_region_description";

#define ADD_STRUCT_MEMBER( NAME, TYPE, VALUE )                                         \
    field               = build_decl( UNKNOWN_LOCATION, FIELD_DECL, NULL_TREE, TYPE ); \
    DECL_NAME( field )  = get_identifier( NAME );                                      \
    TREE_CHAIN( field ) = fields;                                                      \
    fields              = field;                                                       \
    value               = tree_cons( field, VALUE, value );

static tree
build_string_ptr( const char* string )
{
    size_t string_len;
    tree   string_tree;

    tree string_ref;
    tree ret;

    string_len = strlen( string ) + 1;

    string_tree              = build_string( string_len, string );
    TREE_TYPE( string_tree ) = build_array_type( char_type_node,
                                                 build_index_type( size_int( string_len ) ) );

    string_ref = build4( ARRAY_REF,
                         char_type_node,
                         string_tree,
                         build_int_cst( TYPE_DOMAIN( TREE_TYPE( string_tree ) ), 0 ),
                         NULL,
                         NULL );

    ret = build1( ADDR_EXPR,
                  build_pointer_type( TREE_TYPE( string_ref ) ),
                  string_ref );

    return ret;
} /* build_string_ptr */


static tree
build_region_descr_value( tree        handle,
                          const char* name,
                          const char* canonical_name,
                          const char* file,
                          int         begin_lno,
                          int         end_lno )
{
    /* typedef struct
     * {
     *     uint32_t*   handle_ref;
     *     const char* name;
     *     const char* canonical_name;
     *     const char* file;
     *     int         begin_lno;
     *     int         end_lno;
     *     unsigned    flags;
     * } scorep_compiler_region_description;
     */
    tree type = lang_hooks.types.make_type( RECORD_TYPE );
    tree field, fields = NULL_TREE;
    tree value = NULL_TREE;

    tree handle_ptr_type     = build_pointer_type( TREE_TYPE( handle ) );
    tree const_char_ptr_type = build_qualified_type(
        build_pointer_type( char_type_node ), TYPE_QUAL_CONST );

    ADD_STRUCT_MEMBER( "handle_ref",
                       handle_ptr_type,
                       build1( ADDR_EXPR, handle_ptr_type, handle ) )

    ADD_STRUCT_MEMBER( "name",
                       const_char_ptr_type,
                       build_string_ptr( name ) )

    ADD_STRUCT_MEMBER( "canonical_name",
                       const_char_ptr_type,
                       build_string_ptr( canonical_name ) )

    ADD_STRUCT_MEMBER( "file",
                       const_char_ptr_type,
                       build_string_ptr( file ) )

    ADD_STRUCT_MEMBER( "begin_lno",
                       integer_type_node,
                       build_int_cst( integer_type_node, begin_lno ) )

    ADD_STRUCT_MEMBER( "end_lno",
                       integer_type_node,
                       build_int_cst( integer_type_node, end_lno ) )

    ADD_STRUCT_MEMBER( "flags",
                       unsigned_type_node,
                       build_int_cst( unsigned_type_node, 0 ) )

    finish_builtin_struct( type,
                           struct_region_descr_var_name,
                           fields,
                           NULL_TREE );

    return build_constructor_from_list( type, nreverse( value ) );
} /* build_region_descr_value */


tree
scorep_plugin_region_description_build( const char* function_name,
                                        const char* assembler_name,
                                        const char* file_name,
                                        tree        handle_var )
{
    int        begin_lno    = 0;
    location_t function_loc = DECL_SOURCE_LOCATION( current_function_decl );
    if ( function_loc != UNKNOWN_LOCATION )
    {
        begin_lno = LOCATION_LINE( function_loc );
    }
    int end_lno = 0;
    if ( cfun && cfun->function_end_locus != UNKNOWN_LOCATION )
    {
        end_lno = LOCATION_LINE( cfun->function_end_locus );
    }

    /*
     * static const scorep_compiler_region_description __scorep_region_descr =
     * {
     *     .handle_ref     = &__scorep_region_handle,
     *     .name           = __PRETTY_FUNCTION__,
     *     .canonical_name = __func__,
     *     .file           = __FILE__,
     *     .begin_lno      = input_line,
     *     .end_lno        = end_lno,
     *     .flags          = 0
     * };
     */
    tree region_descr_value = build_region_descr_value( handle_var,
                                                        function_name,
                                                        assembler_name,
                                                        file_name,
                                                        begin_lno,
                                                        end_lno );
    tree region_descr_var = build_decl( UNKNOWN_LOCATION,
                                        VAR_DECL,
                                        get_identifier( region_descr_var_name ),
                                        TREE_TYPE( region_descr_value ) );

    /* Align the struct generously, so that it works for 32 and 64 bit */
    SET_DECL_ALIGN( region_descr_var, 64 * BITS_PER_UNIT );
    DECL_USER_ALIGN( region_descr_var ) = 1;

    /* The struct is 64 bytes, because of reserved entries */
    DECL_SIZE( region_descr_var )      = build_int_cst( size_type_node, 64 * BITS_PER_UNIT );
    DECL_SIZE_UNIT( region_descr_var ) = build_int_cst( size_type_node, 64 );

    DECL_INITIAL( region_descr_var )    = region_descr_value;
    DECL_CONTEXT( region_descr_var )    = current_function_decl;
    TREE_PUBLIC( region_descr_var )     = 0;
    TREE_USED( region_descr_var )       = 1;
    DECL_PRESERVE_P( region_descr_var ) = 1;
    DECL_ARTIFICIAL( region_descr_var ) = 1;
    DECL_IGNORED_P( region_descr_var )  = 1;
    TREE_STATIC( region_descr_var )     = 1;
    set_decl_section_name( region_descr_var, region_descrs_section );

    mark_decl_referenced( region_descr_var );
    varpool_finalize_decl( region_descr_var );

    DECL_SEEN_IN_BIND_EXPR_P( region_descr_var ) = 1;
    record_vars( region_descr_var );

    return region_descr_var;
} /* scorep_plugin_region_description_build */
