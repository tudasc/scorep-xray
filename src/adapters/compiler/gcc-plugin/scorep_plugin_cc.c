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

#include <UTILS_Error.h>

#include <SCOREP_Filter.h>

#include <SCOREP_ErrorCallback.h>

#include "scorep_plugin.h"

int plugin_is_GPL_compatible = 1;

#include "scorep_plugin_parameters.inc.c"

static struct opt_pass pass_instrument =
{
    GIMPLE_PASS,                            /* .type */
    "scorep_instrument_function",           /* .name */
#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 4008
    OPTGROUP_NONE,                          /* .optinfo_flags */
#endif
    NULL,                                   /* .gate */
    scorep_plugin_pass_instrument_function, /* .execute */
    NULL,                                   /* .sub */
    NULL,                                   /* .next */
    0,                                      /* .static_pass_number */
    TV_NONE,                                /* .tv_id */
    PROP_cfg,                               /* .properties_required */
    0,                                      /* .properties_provided */
    0,                                      /* .properties_destroyed */
    0,                                      /* .todo_flags_start */
    TODO_verify_ssa | TODO_update_ssa,      /* .todo_flags_finish */
};

static struct register_pass_info pass_instrument_info =
{
    &pass_instrument,           /* .pass */
    "early_optimizations",      /* .reference_pass_name */
    0,                          /* .ref_pass_instance_number */
    PASS_POS_INSERT_BEFORE,     /* .pos_op */
};

int
plugin_init( struct plugin_name_args*   plugin_info,
             struct plugin_gcc_version* version )
{
    const char* plugin_name = plugin_info->base_name;

    SCOREP_InitErrorCallback();

    int ret = scorep_plugin_parameters_init( plugin_info );
    if ( ret )
    {
        return ret;
    }

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
