/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2014,
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
 * @brief  The definitions for inserting the GCC plugin instrumentation handle.
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

#include "scorep_plugin_gcc_version_compatibility.h"
#include "scorep_plugin_inst_handle.h"

static const char* region_handle_var_name = "__scorep_region_handle";
static const char* region_handles_section = ".scorep.region.handles";

static void
var_build( scorep_plugin_inst_handle* handle )
{
    /* static uint32_t __scorep_region_handle = 0; */
    tree handle_var = build_decl( UNKNOWN_LOCATION,
                                  VAR_DECL,
                                  get_identifier( region_handle_var_name ),
                                  handle->type );

    DECL_INITIAL( handle_var )    = build_int_cst( handle->type, 0 );
    DECL_CONTEXT( handle_var )    = current_function_decl;
    TREE_STATIC( handle_var )     = 1;
    TREE_PUBLIC( handle_var )     = 0;
    TREE_USED( handle_var )       = 1;
    DECL_PRESERVE_P( handle_var ) = 1;
    DECL_ARTIFICIAL( handle_var ) = 1;
    DECL_IGNORED_P( handle_var )  = 0;
    varpool_finalize_decl( handle_var );
    set_decl_section_name( handle_var, region_handles_section );

    DECL_SEEN_IN_BIND_EXPR_P( handle_var ) = 1;
    record_vars( handle_var );

    handle->var = handle_var;
}

GIMPLE
scorep_plugin_inst_handle_build_tmp_assignment( scorep_plugin_inst_handle* handle )
{
    if ( !handle )
    {
        return NULL;
    }

    tree handle_tmp_var = create_tmp_var( handle->type,
                                          NULL );
    GIMPLE stmt = gimple_build_assign( handle_tmp_var,
                                       handle->var );
    gimple_assign_set_lhs( stmt,
                           make_ssa_name( handle_tmp_var,
                                          stmt ) );

    return stmt;
}

void
scorep_plugin_inst_handle_build( scorep_plugin_inst_handle* handle )
{
    if ( !handle )
    {
        return;
    }

    handle->type = uint32_type_node;
    var_build( handle );
}
