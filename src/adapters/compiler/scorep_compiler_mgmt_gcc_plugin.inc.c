/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013, 2015-2016, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015, 2022,
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
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-fplugin' flag of the GNU compiler.
 */


#include "scorep_compiler_gcc_plugin.h"

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Filtering.h>


/**
 * section markers for runtime instrumentation
 */
extern const scorep_compiler_region_description scorep_region_descriptions_begin;
extern const scorep_compiler_region_description scorep_region_descriptions_end;

/****************************************************************************************
   Adapter management
 *****************************************************************************************/


void
scorep_compiler_gcc_plugin_register_region( const scorep_compiler_region_description* regionDescr )
{
    /*
     * If unwinding is enabled, we filter out all regions.
     */
    if ( SCOREP_IsUnwindingEnabled()
         || SCOREP_Filtering_Match( regionDescr->file,
                                    regionDescr->name,
                                    regionDescr->canonical_name ) )
    {
        *regionDescr->handle = SCOREP_FILTERED_REGION;
        return;
    }

    *regionDescr->handle =
        SCOREP_Definitions_NewRegion( regionDescr->name,
                                      regionDescr->canonical_name,
                                      SCOREP_Definitions_NewSourceFile(
                                          regionDescr->file ),
                                      regionDescr->begin_lno,
                                      regionDescr->end_lno,
                                      SCOREP_PARADIGM_COMPILER,
                                      SCOREP_REGION_FUNCTION );

    UTILS_DEBUG( "registered %s:%d-%d:%s: \"%s\"",
                 regionDescr->file,
                 regionDescr->begin_lno,
                 regionDescr->end_lno,
                 regionDescr->canonical_name,
                 regionDescr->name );
}


static void
gcc_plugin_register_regions( void )
{
    /* Initialize plugin instrumentation */
    for ( const scorep_compiler_region_description* region_descr = &scorep_region_descriptions_begin + 1;
          region_descr < &scorep_region_descriptions_end;
          region_descr++ )
    {
        /* This handles SCOREP_IsUnwindingEnabled() and sets all regions handles to `FILTERED`. */
        scorep_compiler_gcc_plugin_register_region( region_descr );
    }
}
