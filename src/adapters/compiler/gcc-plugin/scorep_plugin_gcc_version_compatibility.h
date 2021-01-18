/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015, 2020,
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
 * @brief  Definitions for garanting compatibility of the plugin to all versions of GCC.
 *
 */

#if SCOREP_GCC_PLUGIN_TARGET_VERSION < 5000
#define set_decl_section_name( decl, section ) \
    DECL_SECTION_NAME( decl ) = build_string( strlen( section ), section )
#endif

#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 5000
#define varpool_finalize_decl( decl ) varpool_node::finalize_decl( decl )
#define cgraph_get_node( decl ) cgraph_node::get( decl )
#define cgraph_function_body_availability( node ) ( node )->get_availability()
#define TODO_verify_ssa TODO_verify_il
#endif

#if SCOREP_GCC_PLUGIN_TARGET_VERSION < 6000
#define GIMPLE gimple
#else
#define GIMPLE gimple *
#endif

#if SCOREP_GCC_PLUGIN_TARGET_VERSION < 7000
#define SET_DECL_ALIGN( decl, x ) DECL_ALIGN( decl ) = ( x )
#endif

#if SCOREP_GCC_PLUGIN_TARGET_VERSION < 10000
#define cgraph_inlined_to( node ) ( node )->global.inlined_to
#else
#define cgraph_inlined_to( node ) ( node )->inlined_to
#endif


#if SCOREP_GCC_PLUGIN_TARGET_VERSION < 11000
/* DECL_IS_UNDECLARED_BUILTIN was named DECL_IS_BUILTIN prior to GCC 11. */
#define DECL_IS_UNDECLARED_BUILTIN( decl ) DECL_IS_BUILTIN( decl )
#endif
