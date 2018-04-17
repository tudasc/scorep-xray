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
 * @brief  The Score-P GCC plugin definition.
 *
 */

#include "scorep_plugin_gcc-plugin.h"

#include <config.h>

#include "plugin.h"
#include "coretypes.h"
#include "tree-pass.h"
#include "context.h"

#include <UTILS_Error.h>

#include <SCOREP_Filter.h>

#include <SCOREP_ErrorCallback.h>

#include "scorep_plugin_gcc_version_compatibility.h"
#include "scorep_plugin.h"

int plugin_is_GPL_compatible = 1;

#include "scorep_plugin_parameters.inc.c"

namespace
{
const pass_data pass_data_instrument =
{
    GIMPLE_PASS,                          // .type
    "scorep_instrument_function",         // .name
    OPTGROUP_NONE,                        // .optinfo_flags
#if SCOREP_GCC_PLUGIN_TARGET_VERSION < 5000
    false,                                // .has_gate
    true,                                 // .has_execute
#endif
    TV_NONE,                              // .tv_id
    PROP_cfg,                             // .properties_required
    0,                                    // .properties_provided
    0,                                    // .properties_destroyed
    0,                                    // .todo_flags_start
    TODO_verify_ssa | TODO_update_ssa,    // .todo_flags_finish
};

class pass_instrument : public gimple_opt_pass
{
public:
    pass_instrument( gcc::context* ctxt )
        : gimple_opt_pass( pass_data_instrument, ctxt )
    {
    }

    /* opt_pass methods: */
    unsigned int
#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 5000
    execute( function* )
#else
    execute()
#endif
#if __cplusplus >= 201103L
    override
#endif
    {
        return scorep_plugin_pass_instrument_function();
    }

    opt_pass*
    clone()
#if __cplusplus >= 201103L
    override
#endif
    {
        return new pass_instrument( m_ctxt );
    }
};  /* class pass_instrument */
}

gimple_opt_pass*
make_pass_instrument( gcc::context* ctxt )
{
    return new pass_instrument( ctxt );
} /* make_pass_instrument */

int
plugin_init( struct plugin_name_args*   plugin_info,
             struct plugin_gcc_version* version )
{
    struct register_pass_info pass_instrument_info;
    const char*               plugin_name = plugin_info->base_name;

    SCOREP_InitErrorCallback();

    int ret = scorep_plugin_parameters_init( plugin_info );
    if ( ret )
    {
        return ret;
    }

    pass_instrument_info.pass                     = make_pass_instrument( g );
    pass_instrument_info.reference_pass_name      = "early_optimizations";
    pass_instrument_info.ref_pass_instance_number = 0;
    pass_instrument_info.pos_op                   = PASS_POS_INSERT_BEFORE;

    register_callback( plugin_name,
                       PLUGIN_PASS_MANAGER_SETUP,
                       NULL,
                       &pass_instrument_info );

    register_callback( plugin_name,
                       PLUGIN_ATTRIBUTES,
                       scorep_register_attributes,
                       NULL );

    register_callback( plugin_name,
                       PLUGIN_FINISH,
                       scorep_plugin_parameters_fini,
                       NULL );

    return 0;
} /* plugin_init */
