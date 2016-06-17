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

typedef enum
{
    REGISTER,
    ENTRY,
    EXIT
} scorep_gcc_plugin_hook_type;

typedef struct scorep_plugin_inst_hook
{
    scorep_gcc_plugin_hook_type hook_type;

    tree                        fn_decl;
    GIMPLE                      fn_call;
    GIMPLE                      condition;
    gimple_seq                  stmt_sequence;
} scorep_plugin_inst_hook;

void
scorep_plugin_inst_hook_build( scorep_plugin_inst_hook*    hook,
                               scorep_plugin_inst_handle*  handle,
                               tree                        region_descr_var,
                               scorep_gcc_plugin_hook_type hook_type );

basic_block
scorep_plugin_inst_hook_finalize_condition( scorep_plugin_inst_hook* hook,
                                            basic_block              condition_bb );
