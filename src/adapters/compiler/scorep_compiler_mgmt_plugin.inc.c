/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013, 2015-2016, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015, 2022-2024,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Support for GNU-Compiler and LLVM pass plugins
 * Will be triggered by the '-fplugin' flag of the GNU compiler and the
 * '-fpass-plugin' flag of the LLVM based compilers
 */


#include "scorep_compiler_plugin.h"

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Filtering.h>

#include "scorep_compiler_demangle.h"


/**
 * section markers for runtime instrumentation
 */
extern const scorep_compiler_region_description scorep_region_descriptions_begin;
extern const scorep_compiler_region_description scorep_region_descriptions_end;

/****************************************************************************************
   Adapter management
 *****************************************************************************************/


void
scorep_compiler_plugin_register_region( const scorep_compiler_region_description* regionDescr )
{
    /*
     * If unwinding is enabled, we filter out all regions.
     */
    if ( SCOREP_IsUnwindingEnabled() )
    {
        *regionDescr->handle = SCOREP_FILTERED_REGION;
        return;
    }

    char* demangled_name;
#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_GCC_PLUGIN )
    demangled_name = regionDescr->name;
#elif HAVE( SCOREP_COMPILER_INSTRUMENTATION_LLVM_PLUGIN )
    /* Try to demangle during registration, as LLVM installations might not
     * provide a way to demangle region descriptions */
    char* mangled_name = calloc( strlen( regionDescr->name ) + 1, sizeof( char ) );
    strncpy( mangled_name, regionDescr->name, strlen( regionDescr->name ) );
    demangled_name = NULL;
    if ( regionDescr->name == regionDescr->canonical_name )
    {
        scorep_compiler_demangle( mangled_name, demangled_name );
    }
    else
    {
        demangled_name = mangled_name;
        mangled_name   = NULL;
    }
#else
#error "No compiler instrumentation plugin available for this compiler. Please check your configuration."
#endif /* HAVE( SCOREP_COMPILER_INSTRUMENTATION_LLVM_PLUGIN ) */

    if ( SCOREP_Filtering_Match( regionDescr->file,
                                 demangled_name,
                                 regionDescr->canonical_name ) )
    {
        *regionDescr->handle = SCOREP_FILTERED_REGION;
        return;
    }


    *regionDescr->handle =
        SCOREP_Definitions_NewRegion( demangled_name,
                                      regionDescr->canonical_name,
                                      SCOREP_Definitions_NewSourceFile(
                                          regionDescr->file ),
                                      regionDescr->begin_lno,
                                      regionDescr->end_lno,
                                      SCOREP_PARADIGM_COMPILER,
                                      SCOREP_REGION_FUNCTION );


#if HAVE( SCOREP_COMPILER_INSTRUMENTATION_LLVM_PLUGIN )
    scorep_compiler_demangle_free( mangled_name, demangled_name );
    /* scorep_compiler_demangle_free only frees the pointer allocated
     * if SCOREP_Demangle succeeded. We also need to free the original
     * pointer, which is either mangled_name or demangled_name if demangling
     * was unsuccessful */
    if ( mangled_name )
    {
        free( mangled_name );
    }
    else if ( demangled_name )
    {
        free( demangled_name );
    }
#endif /* HAVE( SCOREP_COMPILER_INSTRUMENTATION_LLVM_PLUGIN ) */

    UTILS_DEBUG( "Registered %s:%d-%d:%s: \"%s\"",
                 regionDescr->file,
                 regionDescr->begin_lno,
                 regionDescr->end_lno,
                 regionDescr->canonical_name,
                 regionDescr->name );
}


static void
plugin_register_regions( void )
{
    /* Initialize plugin instrumentation */
    for ( const scorep_compiler_region_description* region_descr = &scorep_region_descriptions_begin + 1;
          region_descr < &scorep_region_descriptions_end;
          region_descr++ )
    {
        /* This handles SCOREP_IsUnwindingEnabled() and sets all regions handles to `FILTERED`. */
        scorep_compiler_plugin_register_region( region_descr );
    }
}
