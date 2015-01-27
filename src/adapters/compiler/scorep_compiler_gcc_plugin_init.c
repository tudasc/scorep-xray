/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013,
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
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-fplugin' flag of the GNU compiler.
 */

#include <config.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Filter.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_gcc_plugin.h"

/**
 * section markers for runtime instrumentation
 */
extern const scorep_compiler_region_description scorep_region_descriptions_begin;
extern const scorep_compiler_region_description scorep_region_descriptions_end;


/****************************************************************************************
   Adapter management
 *****************************************************************************************/


void
scorep_compiler_register_region( const scorep_compiler_region_description* regionDescr )
{
    /* Do not filter if this function does not have a conditional trigger. */
    if ( ~regionDescr->flags & SCOREP_COMPILER_REGION_FLAG_NO_CONDITION
         && SCOREP_Filter_Match( regionDescr->file,
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


SCOREP_ErrorCode
scorep_compiler_subsystem_init( void )
{
    if ( !scorep_compiler_initialized )
    {
        UTILS_DEBUG( "inititialize GCC plugin compiler adapter" );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize plugin instrumentation */
        for ( const scorep_compiler_region_description* region_descr = &scorep_region_descriptions_begin + 1;
              region_descr < &scorep_region_descriptions_end;
              region_descr++ )
        {
            scorep_compiler_register_region( region_descr );
        }

        /* Set flag */
        scorep_compiler_initialized = true;
    }

    return SCOREP_SUCCESS;
}


SCOREP_ErrorCode
scorep_compiler_subsystem_init_location( struct SCOREP_Location* locationData )
{
    return SCOREP_SUCCESS;
}


/* Adapter finalization */
void
scorep_compiler_subsystem_finalize( void )
{
    /* call only, if previously initialized */
    if ( scorep_compiler_initialized )
    {
        /* Set initialization flag */
        scorep_compiler_initialized = false;
        scorep_compiler_finalized   = true;

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
}
