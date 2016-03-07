/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
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
 * @brief  The definition of the scorep_no_instrument attribute.
 *
 */

#include "tree.h"
#include "plugin.h"

#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 4009
#include "attribs.h"
#include "stringpool.h"
#endif

#include "tree-pass.h"

#include "scorep_plugin_gcc_version_compatibility.h"
#include "scorep_plugin.h"

struct attribute_spec scorep_no_instrument_attribute =
{
    "scorep_no_instrument",
    0,
    0,
    false,
    false,
    false,
    NULL,
    #if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 4007
    false
    #endif
};

#ifdef __cplusplus
extern "C"
{
#endif
void
scorep_register_attributes( void* event_data,
                            void* data )
{
    if ( lookup_attribute_spec( get_identifier( scorep_no_instrument_attribute.name ) ) == NULL )
    {
        register_attribute( &scorep_no_instrument_attribute );
    }
}
#ifdef __cplusplus
}
#endif
