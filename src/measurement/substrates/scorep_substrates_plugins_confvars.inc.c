/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */
#include <SCOREP_Config.h>


/** Contains the name of requested substrates. */
static char* scorep_substrate_plugins = NULL;

/** Contains the separator of substrate names. */
static char* scorep_substrate_plugins_separator = NULL;

/** List of configuration variables for the substrate plugins.
 *
 *  Configuration variables for the substrate plugins.
 *  Current configuration variables are:
 *  @li @c SCOREP_SUBSTRATE_PLUGINS list of requested substrate plugins.
 *  @li @c SCOREP_SUBSTRATE_PLUGINS_SEP separator for SCOREP_SUBSTRATE_PLUGINS
 */
static const SCOREP_ConfigVariable scorep_substrates_plugins_confvars[] = {
    {
        "plugins",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_substrate_plugins,
        NULL,
        "",
        "Specify list of used plugins",
        "List of requested substrate plugin names that will be used during program run."
    },
    {
        "plugins_sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_substrate_plugins_separator,
        NULL,
        ",",
        "Separator of substrate plugin names",
        "Character that separates plugin names in SCOREP_SUBSTRATE_PLUGINS."
    },
    SCOREP_CONFIG_TERMINATOR
};
