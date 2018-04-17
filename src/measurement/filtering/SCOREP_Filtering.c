/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 * Implements the filter initialization interface for the filtering system.
 */

#include <config.h>

#include <SCOREP_Filtering.h>

#include <SCOREP_Filter.h>
#include <UTILS_Error.h>

#include <stdlib.h>

/* **************************************************************************************
   Variable and type definitions
****************************************************************************************/

#include "scorep_filtering_confvars.inc.c"

/* **************************************************************************************
   Initialization of filtering system
****************************************************************************************/

static SCOREP_Filter* scorep_filter;

static bool scorep_filter_is_enabled;

void
SCOREP_Filtering_Initialize( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                        "Initialize selective tracing" );

    scorep_filter = SCOREP_Filter_New();
    if ( !scorep_filter )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "Cannot create filter object." );
        return;
    }

    /* Check whether a configuraion file name was specified */
    if ( scorep_filtering_file_name == NULL || *scorep_filtering_file_name == '\0' )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                            "No configuration file for filtering specified.\n"
                            "Disable filtering." );
        return;
    }

    /* Parse configuration file */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CONFIG,
                        "Reading filter configuration file %s.",
                        scorep_filtering_file_name );

    SCOREP_ErrorCode err = SCOREP_Filter_ParseFile( scorep_filter, scorep_filtering_file_name );
    if ( err != SCOREP_SUCCESS )
    {
        UTILS_ERROR( err,
                     "Error while parsing filter file. Abort." );
        exit( EXIT_FAILURE );
    }

    scorep_filter_is_enabled = true;
}

void
SCOREP_Filtering_Finalize( void )
{
    if ( scorep_filter )
    {
        SCOREP_Filter_Delete( scorep_filter );
    }
    scorep_filter            = NULL;
    scorep_filter_is_enabled = false;
}

bool
SCOREP_Filtering_Match( const char* fileName,
                        const char* functionName,
                        const char* mangledName )
{
    if ( !scorep_filter_is_enabled )
    {
        return false;
    }

    int              result;
    SCOREP_ErrorCode err =  SCOREP_Filter_Match( scorep_filter, fileName, functionName, mangledName, &result );
    if ( err != SCOREP_SUCCESS )
    {
        scorep_filter_is_enabled = false;
        return false;
    }
    return !!result;
}

bool
SCOREP_Filtering_MatchFile( const char* fileName )
{
    if ( !scorep_filter_is_enabled )
    {
        return false;
    }

    int              result;
    SCOREP_ErrorCode err = SCOREP_Filter_MatchFile( scorep_filter, fileName, &result );
    if ( err != SCOREP_SUCCESS )
    {
        scorep_filter_is_enabled = false;
        return false;
    }
    return !!result;
}

bool
SCOREP_Filtering_MatchFunction( const char* functionName,
                                const char* mangledName )
{
    if ( !scorep_filter_is_enabled )
    {
        return false;
    }

    int              result;
    SCOREP_ErrorCode err = SCOREP_Filter_MatchFunction( scorep_filter, functionName, mangledName, &result );
    if ( err != SCOREP_SUCCESS )
    {
        scorep_filter_is_enabled = false;
        return false;
    }
    return !!result;
}
