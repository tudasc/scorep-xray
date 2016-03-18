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
 * @brief  The definition for the instrumentation hooks of the GCC plugin.
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
#include "scorep_plugin_inst_handle.h"
#include "scorep_plugin_inst_hook.h"

static const char* register_hook_name = "scorep_plugin_register_region";
static const char* entry_hook_name    = "scorep_plugin_enter_region";
static const char* exit_hook_name     = "scorep_plugin_exit_region";


static tree
build_inst_function( scorep_gcc_plugin_hook_type hook_type,
                     scorep_plugin_inst_handle*  handle,
                     tree                        region_descr_var )
{
    tree func_decl;
    tree func_type_list = build_function_type_list( void_type_node,
                                                    handle->type,
                                                    NULL_TREE );

    switch ( hook_type )
    {
        case REGISTER:
            func_type_list = build_function_type_list( void_type_node,
                                                       build_pointer_type(
                                                           TREE_TYPE( region_descr_var ) ),
                                                       NULL_TREE );
            func_decl = build_fn_decl( register_hook_name, func_type_list );
            break;
        case ENTRY:
            func_decl = build_fn_decl( entry_hook_name, func_type_list );
            break;
        case EXIT:
            func_decl = build_fn_decl( exit_hook_name, func_type_list );
            break;
    }

    TREE_PUBLIC( func_decl ) = 1;
    return func_decl;
} /* build_inst_function */


static GIMPLE
build_condition( scorep_gcc_plugin_hook_type hook_type,
                 GIMPLE                      tmp_assignment,
                 scorep_plugin_inst_handle*  handle )
{
    switch ( hook_type )
    {
        case REGISTER:
            return gimple_build_cond( EQ_EXPR,
                                      gimple_assign_lhs( tmp_assignment ),
                                      build_int_cst( handle->type,
                                                     0 /* SCOREP_INVALID_REGION */ ),
                                      NULL_TREE,
                                      NULL_TREE );

        case ENTRY:
        case EXIT:
            return gimple_build_cond( NE_EXPR,
                                      gimple_assign_lhs( tmp_assignment ),
                                      build_int_cst( handle->type,
                                                     -1 /* SCOREP_FILTERED_REGION */ ),
                                      NULL_TREE,
                                      NULL_TREE );
    }
} /* build_condition */


static GIMPLE
build_fn_call( scorep_gcc_plugin_hook_type hook_type,
               scorep_plugin_inst_hook*    hook,
               GIMPLE                      tmp_assignment,
               tree                        region_descr_var )
{
    switch ( hook_type )
    {
        case REGISTER:
            add_referenced_var( region_descr_var );
            return gimple_build_call( hook->fn_decl,
                                      1,
                                      build_fold_addr_expr( region_descr_var ) );
        case ENTRY:
        case EXIT:
            return gimple_build_call( hook->fn_decl,
                                      1,
                                      gimple_assign_lhs( tmp_assignment ) );
    }
} /* build_fn_call */


void
scorep_plugin_inst_hook_build( scorep_plugin_inst_hook*    hook,
                               scorep_plugin_inst_handle*  handle,
                               tree                        region_descr_var,
                               scorep_gcc_plugin_hook_type hook_type )
{
    if ( !hook
         || !( handle || region_descr_var ) )
    {
        return;
    }

    hook->fn_decl = build_inst_function( hook_type,
                                         handle,
                                         region_descr_var );

    hook->hook_type = hook_type;

    hook->stmt_sequence = 0;

    GIMPLE tmp_assignment = scorep_plugin_inst_handle_build_tmp_assignment( handle );
    gimple_seq_add_stmt( &hook->stmt_sequence,
                         tmp_assignment );

    hook->condition = build_condition( hook_type,
                                       tmp_assignment,
                                       handle );
    gimple_seq_add_stmt( &hook->stmt_sequence,
                         hook->condition );

    hook->fn_call = build_fn_call( hook_type,
                                   hook,
                                   tmp_assignment,
                                   region_descr_var );

    gimple_seq_add_stmt( &hook->stmt_sequence,
                         hook->fn_call );
} /* scorep_plugin_inst_hook_build */


basic_block
scorep_plugin_inst_hook_finalize_condition( scorep_plugin_inst_hook* hook,
                                            basic_block              condition_bb )
{
    basic_block then_bb;
    basic_block join_bb;

    edge e_true = split_block( condition_bb, hook->condition );
    then_bb       = e_true->dest;
    e_true->flags = ( e_true->flags & ~EDGE_FALLTHRU ) | EDGE_TRUE_VALUE;

    edge e_join = split_block( then_bb, hook->fn_call );
    join_bb       = e_join->dest;
    e_join->flags = e_join->flags & EDGE_FALLTHRU;

    make_edge( condition_bb, join_bb, EDGE_FALSE_VALUE );

    if ( dom_info_available_p( CDI_DOMINATORS ) )
    {
        set_immediate_dominator( CDI_DOMINATORS, then_bb, condition_bb );
        set_immediate_dominator( CDI_DOMINATORS, join_bb, condition_bb );
    }

    return join_bb;
} /* scorep_plugin_inst_hook_finalize_condition */
