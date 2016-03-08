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

typedef struct scorep_plugin_inst_handle
{
    tree type;
    tree var;
} scorep_plugin_inst_handle;

void
scorep_plugin_inst_handle_build( scorep_plugin_inst_handle* handle );

GIMPLE
scorep_plugin_inst_handle_build_tmp_assignment( scorep_plugin_inst_handle* handle );
