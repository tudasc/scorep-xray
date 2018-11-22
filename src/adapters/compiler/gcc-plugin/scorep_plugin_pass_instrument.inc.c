/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014, 2016, 2018,
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
 * @brief  Definition of the GCC instrumentation pass.
 *
 */

#include "tree.h"

#include <stdarg.h>

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
#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 4009
#include "stringpool.h"
#include "attribs.h"
#endif

#include <SCOREP_Filter.h>

#include "scorep_plugin_gcc_version_compatibility.h"
#include "scorep_plugin.h"
#include "scorep_plugin_inst_descriptor.h"
#include "scorep_plugin_inst_handle.h"
#include "scorep_plugin_inst_hook.h"

#define VERBOSE_MSG( level, msg, ... ) \
    do \
    { \
        if ( level < scorep_plugin_verbosity ) \
        { \
            fprintf( stderr, "[" PACKAGE_NAME "] " msg "\n", ##__VA_ARGS__ ); \
        } \
    } while ( 0 )

static const char dir_separator_str[] = { DIR_SEPARATOR, 0 };

static const char*
is_instrumentable( const char* assemblerName )
{
    struct cgraph_node* node = cgraph_get_node( current_function_decl );
    if ( !node )
    {
        return "no call node";
    }

    location_t function_loc = DECL_SOURCE_LOCATION( current_function_decl );
    if ( in_system_header_at( function_loc ) )
    {
        return "in system header";
    }

    if ( node->global.inlined_to )
    {
        return "was inlined";
    }

    if ( DECL_ARTIFICIAL( current_function_decl ) )
    {
        return "is artificial";
    }

    if ( DECL_IS_BUILTIN( current_function_decl ) )
    {
        return "is built-in";
    }

    if ( strncmp( assemblerName, "POMP", 4 ) == 0
         || strncmp( assemblerName, "Pomp", 4 ) == 0
         || strncmp( assemblerName, "pomp", 4 ) == 0 )
    {
        return "is POMP";
    }

    if ( cgraph_function_body_availability( node ) == AVAIL_NOT_AVAILABLE )
    {
        return "no function body";
    }

    return NULL;
} /* is_instrumentable */


static int
is_declared_inline( void )
{
    return DECL_DECLARED_INLINE_P( current_function_decl );
}


static int
has_no_instrument_attribute( void )
{
    tree attribute_args = lookup_attribute( scorep_no_instrument_attribute.name,
                                            DECL_ATTRIBUTES( current_function_decl ) );
    if ( attribute_args != NULL_TREE )
    {
        return 1;
    }

    return 0;
} /* has_no_instrument_attribute */


#ifdef __cplusplus
extern "C"
#endif /* ifdef __cplusplus */
unsigned int
scorep_plugin_pass_instrument_function( void )
{
    char* function_name = xstrdup( lang_hooks.decl_printable_name( current_function_decl,
                                                                   scorep_plugin_symbol_verbosity ) );
    const char* assembler_name = IDENTIFIER_POINTER( DECL_ASSEMBLER_NAME_SET_P( current_function_decl )
                                                     ? DECL_ASSEMBLER_NAME( current_function_decl )
                                                     : DECL_NAME( current_function_decl ) );

    /*
     * use a prefix compare for the language, as later Fortran compilers are
     * named like "GNU Fortran2008"
     */
    if ( 0 == strncmp( lang_hooks.name, "GNU Fortran", strlen( "GNU Fortran" ) ) )
    {
        /* "main" is considered an artificial function in Fortran, the main
           user function (entry point) is "MAIN__". */
        if ( 0 == strcmp( assembler_name, "main" ) )
        {
            VERBOSE_MSG( 0, "Ignoring artifical 'main' in %s program", lang_hooks.name );
            return 0;
        }

        /*
         * fortran module mangling scheme:
         *    '__' + <module-name> + '_MOD_' + <function-name>
         * where <module-name> and <function-name> are always lowercase, thus
         * _MOD_ can occur at most once, the two underscores are not affected by
         * any command line options like -f[no-]underscoring or
         * -f[no-]second-underscore
         */
        if ( assembler_name[ 0 ] == '_'
             && assembler_name[ 1 ] == '_'
             && strstr( assembler_name + 2, "_MOD_" ) )
        {
            const char* module_name     = assembler_name + 2;
            size_t      module_name_len = strstr( module_name, "_MOD_" ) - module_name;

            char* fortran_function_name =
                ( char* )xmalloc( module_name_len + 2 + strlen( function_name ) + 1 );
            sprintf( fortran_function_name, "%.*s::%s",
                     ( int )module_name_len, module_name, function_name );

            free( function_name );
            function_name = fortran_function_name;
        }
    }

    /* Don't instrument un-instrumentable functions */
    const char* reason = is_instrumentable( assembler_name );
    if ( reason )
    {
        VERBOSE_MSG( 0, "Function is not instrumentable: '%s' [reason: %s]", function_name, reason );
        free( function_name );
        return 0;
    }

    /* Don't instrument functions which are declared "inline"
     */
    if ( is_declared_inline() )
    {
        /* Check if this function is explicitly included in filter file */
        int              result;
        SCOREP_ErrorCode err = SCOREP_Filter_IncludeFunction( scorep_instrument_filter,
                                                              function_name, assembler_name,
                                                              &result );
        if ( err == SCOREP_SUCCESS && !result )
        {
            VERBOSE_MSG( 1, "Function is excluded because of inline declaration: '%s'", function_name );
            free( function_name );
            return 0;
        }
    }

    /* Don't instrument functions which are marked with the "scorep_no_instrument"
     * attribute
     */
    if ( has_no_instrument_attribute() )
    {
        VERBOSE_MSG( 1, "Function is excluded by user attribute: '%s'", function_name );
        free( function_name );
        return 0;
    }

    char*      tmp_full_path;
    char*      full_path;
    location_t function_loc = DECL_SOURCE_LOCATION( current_function_decl );
    if ( !IS_ABSOLUTE_PATH( LOCATION_FILE( function_loc ) ) )
    {
        tmp_full_path = concat( getpwd(),
                                dir_separator_str,
                                LOCATION_FILE( function_loc ),
                                NULL );
    }
    else
    {
        tmp_full_path = xstrdup( LOCATION_FILE( function_loc ) );
    }
    full_path = lrealpath( tmp_full_path );
    free( tmp_full_path );

    /* Don't instrument filtered functions */
    int              result;
    SCOREP_ErrorCode err = SCOREP_Filter_Match( scorep_instrument_filter,
                                                full_path, function_name, assembler_name,
                                                &result );
    if ( err == SCOREP_SUCCESS && result )
    {
        VERBOSE_MSG( 1, "Function excluded by filter file: '%s'", function_name );
        free( full_path );
        free( function_name );
        return 0;
    }

    VERBOSE_MSG( 2, "Instrumenting function: '%s'", function_name );

    scorep_plugin_inst_handle handle;
    scorep_plugin_inst_hook   entry_hook;
    scorep_plugin_inst_hook   register_hook;
    tree                      region_descr_var;

    scorep_plugin_inst_handle_build( &handle );

    region_descr_var = scorep_plugin_region_description_build( function_name,
                                                               assembler_name,
                                                               full_path,
                                                               handle.var );

    /* build & insert entry hook */
    scorep_plugin_inst_hook_build( &entry_hook,
                                   &handle,
                                   region_descr_var,
                                   ENTRY );

    basic_block          bb_start  = split_block_after_labels( ENTRY_BLOCK_PTR )->dest;
    gimple_stmt_iterator gsi_start = gsi_start_bb( bb_start );

    gsi_insert_seq_before( &gsi_start, entry_hook.stmt_sequence, GSI_SAME_STMT );
    scorep_plugin_inst_hook_finalize_condition( &entry_hook,
                                                bb_start );


    /* build & insert register hook */
    scorep_plugin_inst_hook_build( &register_hook,
                                   &handle,
                                   region_descr_var,
                                   REGISTER );

    bb_start  = split_block_after_labels( ENTRY_BLOCK_PTR )->dest;
    gsi_start = gsi_start_bb( bb_start );

    gsi_insert_seq_before( &gsi_start, register_hook.stmt_sequence, GSI_SAME_STMT );
    scorep_plugin_inst_hook_finalize_condition( &register_hook,
                                                bb_start );


    /* build & insert exit hook(s) */
    basic_block          bb_end;
    gimple_stmt_iterator gsi_end;

    bool have_exit_instrumentation = false;
    FOR_EACH_BB( bb_end )
    {
        for ( gsi_end = gsi_start_bb( bb_end ); !gsi_end_p( gsi_end ); gsi_next( &gsi_end ) )
        {
            if ( gimple_code( gsi_stmt( gsi_end ) ) == GIMPLE_RETURN )
            {
                scorep_plugin_inst_hook exit_hook;

                scorep_plugin_inst_hook_build( &exit_hook,
                                               &handle,
                                               region_descr_var,
                                               EXIT );

                gsi_insert_seq_before( &gsi_end, exit_hook.stmt_sequence, GSI_SAME_STMT );
                bb_end = scorep_plugin_inst_hook_finalize_condition( &exit_hook,
                                                                     bb_end );

                have_exit_instrumentation = true;
            }
        }
    }
    if ( !have_exit_instrumentation )
    {
        VERBOSE_MSG( 0, "warning: Function has no exit to instrument: '%s'", function_name );
    }

    free( full_path );
    free( function_name );

    return 0;
} /* scorep_plugin_pass_instrument_function */
