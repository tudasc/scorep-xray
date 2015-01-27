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

#include "gcc-plugin.h"
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_VERSION
#undef PACKAGE_TARNAME

#include <config.h>

#include "gcc-plugin.h"
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
    .type                 = GIMPLE_PASS,
    .name                 = "scorep_instrument_function",
    .gate                 = NULL,
    .execute              = scorep_plugin_pass_instrument_function,
    .sub                  = NULL,
    .next                 = NULL,
    .static_pass_number   = 0,
    .tv_id                = TV_NONE,
    .properties_required  = PROP_cfg,
    .properties_provided  = 0,
    .properties_destroyed = 0,
    .todo_flags_start     = 0,
    .todo_flags_finish    = TODO_verify_ssa | TODO_update_ssa,
};

static struct register_pass_info pass_instrument_info =
{
    .pass                     = &pass_instrument,
    .reference_pass_name      = "early_optimizations",
    .ref_pass_instance_number = 0,
    .pos_op                   = PASS_POS_INSERT_BEFORE,
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

    return 0;
} /* plugin_init */
