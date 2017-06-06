/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
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

/* bool add_referenced_var( tree ) */
#if SCOREP_GCC_PLUGIN_TARGET_VERSION >= 4008
#define add_referenced_var( t ) do { } while ( 0 )
#endif

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
