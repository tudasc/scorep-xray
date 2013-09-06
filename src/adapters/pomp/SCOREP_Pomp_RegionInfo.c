/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2013
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SCOREP_Pomp_RegionInfo.c
 * @ingroup    POMP
 *
 * @brief Implements functionality for interpretation of pomp region strings.
 * @note  All functions in this file are only called during initialization.
 *        The initialization is assumed to be executed serially.
 *
 */

#include <config.h>
#include "SCOREP_Pomp_RegionInfo.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <UTILS_IO.h>
#include <UTILS_Debug.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Types.h>
#include <SCOREP_Filter.h>
#include <pomp2_region_info.h>

/** @ingroup POMP
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

/** Contains data for parsing a region info string */
typedef struct /* scorep_pomp_parsing_data */
{
    SCOREP_Pomp_Region* region;
    char*               stringToParse;     /* will be modified */
    char*               stringMemory;
    char*               stringForErrorMsg; /* copy of the string used for error messages */
} scorep_pomp_parsing_data;

/** Possible token types which can appear in a region info string */
typedef enum
{
    SCOREP_POMP_TOKEN_TYPE_REGION_TYPE,
    SCOREP_POMP_TOKEN_TYPE_START_SOURCE_CODE_LOCATION,
    SCOREP_POMP_TOKEN_TYPE_END_SOURCE_CODE_LOCATION,
    SCOREP_POMP_TOKEN_TYPE_NUM_SECTIONS,
    SCOREP_POMP_TOKEN_TYPE_CRITICAL_NAME,
    SCOREP_POMP_TOKEN_TYPE_USER_REGION_NAME,
    SCOREP_POMP_TOKEN_TYPE_NO_TOKEN,
} scorep_pomp_token_type;

/** Contains a token and its type from a region info string */
typedef struct
{
    char*                  tokenString;
    scorep_pomp_token_type token;
} scorep_pomp_token_map_entry;

/** Maps token names to scorep_pomp_token_type */
static const scorep_pomp_token_map_entry scorep_pomp_token_map[] =
{
    /* Entries must be sorted to be used in binary search. */
    /* If you add/remove items, scorep_pomp_token_map_size   */
    { "criticalName",   SCOREP_POMP_TOKEN_TYPE_CRITICAL_NAME                        },
    { "escl",           SCOREP_POMP_TOKEN_TYPE_END_SOURCE_CODE_LOCATION             },
    { "numSections",    SCOREP_POMP_TOKEN_TYPE_NUM_SECTIONS                         },
    { "regionType",     SCOREP_POMP_TOKEN_TYPE_REGION_TYPE                          },
    { "sscl",           SCOREP_POMP_TOKEN_TYPE_START_SOURCE_CODE_LOCATION           },
    { "userRegionName", SCOREP_POMP_TOKEN_TYPE_USER_REGION_NAME                     }
};

/** Number of entries in @ref scorep_pomp_token_map */
const size_t scorep_pomp_token_map_size =
    sizeof( scorep_pomp_token_map ) / sizeof( scorep_pomp_token_map_entry );

/** Contains the data for one region type */
typedef struct
{
    char*                  regionTypeString;
    char*                  outerRegionName;
    char*                  innerRegionName;
    bool                   hasParallel;
    bool                   hasImplicitBarrier;
    SCOREP_Pomp_RegionType regionType;
    SCOREP_RegionType      outerRegionType;
    SCOREP_RegionType      innerRegionType;
} scorep_pomp_region_type_map_entry;

/* *INDENT-OFF* */
/** Maps region name in the information string to SCOREP_Pomp_RegionType and
    SCOREP_RegionType of the inner and outer region.
*/
static const scorep_pomp_region_type_map_entry scorep_pomp_region_type_map[] =
{
    /* Entries must be sorted to be used in binary search. */
    /* If you add/remove items, scorep_pomp_region_type_map_size. */
    /* Entries must be in same order like SCOREP_Pomp_RegionType to allow lookup. */
  { "atomic",            "atomic",      "",                false, false, SCOREP_Pomp_Atomic            , SCOREP_REGION_ATOMIC,      SCOREP_REGION_UNKNOWN         },
  { "barrier",           "barrier",     "",                false, false, SCOREP_Pomp_Barrier           , SCOREP_REGION_BARRIER,     SCOREP_REGION_UNKNOWN         },
  { "critical",          "critical",    "critical sblock", false, false, SCOREP_Pomp_Critical          , SCOREP_REGION_CRITICAL,    SCOREP_REGION_CRITICAL_SBLOCK },
  { "do",                "do",          "",                false, true,  SCOREP_Pomp_Do                , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "flush",             "flush",       "",                false, false, SCOREP_Pomp_Flush             , SCOREP_REGION_FLUSH,       SCOREP_REGION_UNKNOWN         },
  { "for",               "for",         "",                false, true,  SCOREP_Pomp_For               , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "master",            "",            "master",          false, false, SCOREP_Pomp_Master            , SCOREP_REGION_UNKNOWN,     SCOREP_REGION_MASTER          },
  { "ordered",           "ordered",     "ordered sblock",  false, false, SCOREP_Pomp_Ordered           , SCOREP_REGION_ORDERED,     SCOREP_REGION_ORDERED_SBLOCK  },
  { "parallel",          "",            "",                true,  true,  SCOREP_Pomp_Parallel          , SCOREP_REGION_UNKNOWN,     SCOREP_REGION_UNKNOWN         },
  { "parallel do",       "do",          "",                true,  true,  SCOREP_Pomp_ParallelDo        , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "parallel for",      "for",         "",                true,  true,  SCOREP_Pomp_ParallelFor       , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "parallel sections", "sections",    "section",         true,  true,  SCOREP_Pomp_ParallelSections  , SCOREP_REGION_SECTIONS,    SCOREP_REGION_SECTION         },
  { "parallel workshare","workshare",   "",                true,  true,  SCOREP_Pomp_ParallelWorkshare , SCOREP_REGION_WORKSHARE,   SCOREP_REGION_UNKNOWN         },
  { "region",            "",            "region",          false, false, SCOREP_Pomp_UserRegion        , SCOREP_REGION_UNKNOWN,     SCOREP_REGION_USER,           },
  { "sections",          "sections",    "section",         false, true,  SCOREP_Pomp_Sections          , SCOREP_REGION_SECTIONS,    SCOREP_REGION_SECTION         },
  { "single",            "single",      "single sblock",   false, true,  SCOREP_Pomp_Single            , SCOREP_REGION_SINGLE,      SCOREP_REGION_SINGLE_SBLOCK,  },
  { "task",              "create task", "task",            false, false, SCOREP_Pomp_Task              , SCOREP_REGION_TASK_CREATE, SCOREP_REGION_TASK            },
  { "taskwait",          "taskwait",    "",                false, false, SCOREP_Pomp_Taskwait          , SCOREP_REGION_TASK_WAIT,   SCOREP_REGION_UNKNOWN         },
  { "workshare",         "workshare",   "",                false, true,  SCOREP_Pomp_Workshare         , SCOREP_REGION_WORKSHARE,   SCOREP_REGION_UNKNOWN         }
};
/* *INDENT-ON* */

/** Number of entries in scorep_pomp_region_type_map */
const size_t scorep_pomp_region_type_map_size =
    sizeof( scorep_pomp_region_type_map ) / sizeof( scorep_pomp_region_type_map_entry );

/* **************************************************************************************
 *                                                                    Init/free functions
 ***************************************************************************************/

/** Initializes a SCOREP_Pomp_Region instance @region  with NULL values.
    @param region the SCOREP_Pomp_Region instance which get initialized.
 */
static void
scorep_pomp_init_region( SCOREP_Pomp_Region* region )
{
    region->regionType    = SCOREP_Pomp_NoType;
    region->name          = 0;
    region->numSections   = 0;
    region->innerParallel = SCOREP_INVALID_REGION;
    region->outerBlock    = SCOREP_INVALID_REGION;
    region->innerBlock    = SCOREP_INVALID_REGION;
    region->barrier       = SCOREP_INVALID_REGION;
    region->startFileName = 0;
    region->startLine1    = 0;
    region->startLine2    = 0;
    region->endFileName   = 0;
    region->endLine1      = 0;
    region->endLine2      = 0;
}

/** Registers the pomp regions to scorep and sets the SCOREP region handle fields in
    a SCOREP_Pomp_Region instance. All necessary data must be already contained in the
    SCOREP_Pomp_Region instance.
    @param region  The region infromation struct for a POMP region which gets registered
                   to SCOREP. The region handle fields of it are set in this function.
 */
void
scorep_pomp_register_region( SCOREP_Pomp_Region* region )
{
    char*               region_name = 0;
    SCOREP_RegionType   type_outer  = SCOREP_REGION_UNKNOWN;
    SCOREP_RegionType   type_inner  = SCOREP_REGION_UNKNOWN;
    SCOREP_ParadigmType paradigm    = SCOREP_PARADIGM_OPENMP;
    int32_t             start, end;

    /* Assume that all regions from one file are registered in a row.
       Thus, remember the last file handle and reuse it if the next region stems
       from the same source file.                                                 */
    static char*                   last_file_name = 0;
    static SCOREP_SourceFileHandle last_file      = SCOREP_INVALID_SOURCE_FILE;

    /* Consistency checks */
    if ( ( 0 > region->regionType ) ||
         ( region->regionType > scorep_pomp_region_type_map_size ) )
    {
        UTILS_ERROR( SCOREP_ERROR_INDEX_OUT_OF_BOUNDS,
                     "Region type %d not found in region type table.",
                     region->regionType );
        exit( EXIT_FAILURE );
    }

    /* Check paradigm */
    if ( region->regionType == SCOREP_Pomp_UserRegion )
    {
        paradigm = SCOREP_PARADIGM_USER;
    }

    /* Evtl. register new source file */
    if ( ( last_file == SCOREP_INVALID_SOURCE_FILE ) ||
         ( strcmp( last_file_name, region->startFileName ) != 0 ) )
    {
        last_file_name = region->startFileName;
        last_file      = SCOREP_Definitions_NewSourceFile( last_file_name );
    }

    /* Construct file:lno string */
    const char* basename    = UTILS_IO_GetWithoutPath( region->startFileName );
    char*       source_name = ( char* )malloc( strlen( basename ) + 12 );
    sprintf( source_name, "@%s:%" PRIi32, basename, region->startLine1 );


    /* User regions can be filtered */
    if ( ( region->regionType == SCOREP_Pomp_UserRegion ) &&
         SCOREP_Filter_Match( region->startFileName, region_name, NULL ) )
    {
        region->innerBlock = SCOREP_INVALID_REGION;
        free( source_name );
        free( region_name );
        return;
    }

    /* Determine type of inner and outer regions */
    type_outer = scorep_pomp_region_type_map[ region->regionType ].outerRegionType;
    type_inner = scorep_pomp_region_type_map[ region->regionType ].innerRegionType;

    /* Register parallel region */
    if ( scorep_pomp_region_type_map[ region->regionType ].hasParallel )
    {
        int   length        = 16 + strlen( source_name ) + 1;
        char* parallel_name = ( char* )malloc( length );
        sprintf( parallel_name, "!$omp parallel %s", source_name );

        region->innerParallel = SCOREP_Definitions_NewRegion( parallel_name,
                                                              NULL,
                                                              last_file,
                                                              region->startLine1,
                                                              region->endLine2,
                                                              SCOREP_PARADIGM_OPENMP,
                                                              SCOREP_REGION_PARALLEL );
        free( parallel_name );
    }

    /* Register outer region */
    if ( type_outer != SCOREP_REGION_UNKNOWN )
    {
        char* type_name = scorep_pomp_region_type_map[ region->regionType ].outerRegionName;
        int   length    = strlen( type_name ) + 7 + strlen( source_name ) + 1;
        region_name = ( char* )malloc( length );
        sprintf( region_name, "!$omp %s %s", type_name, source_name );

        if ( scorep_pomp_region_type_map[ region->regionType ].hasParallel )
        {
            start = region->startLine2;
            end   = region->endLine1;
        }
        else
        {
            start = region->startLine1;
            end   = region->endLine2;
        }

        region->outerBlock = SCOREP_Definitions_NewRegion( region_name,
                                                           NULL,
                                                           last_file,
                                                           start,
                                                           end,
                                                           paradigm,
                                                           type_outer );
        free( region_name );
    }

    /* Register inner region */
    if ( type_inner != SCOREP_REGION_UNKNOWN )
    {
        char* type_name = NULL;
        if ( region->name == 0 )
        {
            type_name = scorep_pomp_region_type_map[ region->regionType ].innerRegionName;
        }
        else
        {
            type_name = region->name;
        }
        int length = strlen( type_name ) + 8 + strlen( source_name ) + 1;
        region_name = ( char* )malloc( length );
        if ( region->regionType == SCOREP_Pomp_UserRegion )
        {
            sprintf( region_name, "!$pomp %s %s", type_name, source_name );
        }
        else
        {
            sprintf( region_name, "!$omp %s %s", type_name, source_name );
        }

        region->innerBlock = SCOREP_Definitions_NewRegion( region_name,
                                                           NULL,
                                                           last_file,
                                                           region->startLine2,
                                                           region->endLine1,
                                                           paradigm,
                                                           type_inner );
        free( region_name );
    }

    /* Register implicit barrier */
    if ( scorep_pomp_region_type_map[ region->regionType ].hasImplicitBarrier )
    {
        int   length       = 24 + strlen( basename ) + 1 + 10 + 1;
        char* barrier_name = ( char* )malloc( length );
        sprintf( barrier_name, "!$omp implicit barrier @%s:%" PRIu32, basename, region->endLine1 );

        region->barrier = SCOREP_Definitions_NewRegion( barrier_name,
                                                        NULL,
                                                        last_file,
                                                        region->endLine1,
                                                        region->endLine2,
                                                        SCOREP_PARADIGM_OPENMP,
                                                        SCOREP_REGION_IMPLICIT_BARRIER );
        free( barrier_name );
    }


    free( source_name );

    /* Register locks for critical regions */
    if ( region->regionType == SCOREP_Pomp_Critical )
    {
        region->lock = SCOREP_Pomp_GetLock( region->name );
        if ( region->lock == NULL )
        {
            region->lock = scorep_pomp_lock_init( region->name );
        }
    }
}

/* **************************************************************************************
 *                                                                     Exported functions
 ***************************************************************************************/

void
SCOREP_Pomp_ParseInitString( const char          initString[],
                             SCOREP_Pomp_Region* region )
{
    UTILS_ASSERT( region );
    POMP2_Region_info regionInfo;
    ctcString2RegionInfo( initString, &regionInfo );
    scorep_pomp_init_region( region );
    if ( regionInfo.mCriticalName )
    {
        region->name = ( char* )malloc( sizeof( char ) * ( strlen( regionInfo.mCriticalName ) + 12 ) );
        strcpy( region->name, "critical (" );
        strcat( region->name, regionInfo.mCriticalName );
        strcat( region->name, ")" );
    }
    if ( regionInfo.mUserRegionName )
    {
        region->name = ( char* )malloc( sizeof( char ) * ( strlen( regionInfo.mUserRegionName ) + 1 ) );
        strcpy( region->name, regionInfo.mUserRegionName );
    }
    if ( regionInfo.mStartFileName )
    {
        region->startFileName = ( char* )malloc( sizeof( char ) * ( strlen( regionInfo.mStartFileName ) + 1 ) );
        strcpy( region->startFileName, regionInfo.mStartFileName );
    }
    if ( regionInfo.mEndFileName )
    {
        region->endFileName = ( char* )malloc( sizeof( char ) * ( strlen( regionInfo.mEndFileName ) + 1 ) );
        strcpy( region->endFileName, regionInfo.mEndFileName );
    }
    /* copy attributes from regionInfo into SCOREPs internal data structures*/
    region->numSections = regionInfo.mNumSections;
    region->startLine1  = regionInfo.mStartLine1;
    region->startLine2  = regionInfo.mStartLine2;
    region->endLine1    = regionInfo.mEndLine1;
    region->endLine2    = regionInfo.mEndLine2;
    switch ( regionInfo.mRegionType )
    {
        case POMP2_No_type:
            region->regionType = SCOREP_Pomp_NoType;
            break;
        case POMP2_Atomic:
            region->regionType = SCOREP_Pomp_Atomic;
            break;
        case POMP2_Barrier:
            region->regionType = SCOREP_Pomp_Barrier;
            break;
        case POMP2_Critical:
            region->regionType = SCOREP_Pomp_Critical;
            break;
        case POMP2_Do:
            region->regionType = SCOREP_Pomp_Do;
            break;
        case POMP2_Flush:
            region->regionType = SCOREP_Pomp_Flush;
            break;
        case POMP2_For:
            region->regionType = SCOREP_Pomp_For;
            break;
        case POMP2_Master:
            region->regionType = SCOREP_Pomp_Master;
            break;
        case POMP2_Ordered:
            region->regionType = SCOREP_Pomp_Ordered;
            break;
        case POMP2_Parallel:
            region->regionType = SCOREP_Pomp_Parallel;
            break;
        case POMP2_Parallel_do:
            region->regionType = SCOREP_Pomp_ParallelDo;
            break;
        case POMP2_Parallel_for:
            region->regionType = SCOREP_Pomp_ParallelFor;
            break;
        case POMP2_Parallel_sections:
            region->regionType = SCOREP_Pomp_ParallelSections;
            break;
        case POMP2_Parallel_workshare:
            region->regionType = SCOREP_Pomp_ParallelWorkshare;
            break;
        case POMP2_Sections:
            region->regionType = SCOREP_Pomp_Sections;
            break;
        case POMP2_Single:
            region->regionType = SCOREP_Pomp_Single;
            break;
        case POMP2_Task:
            region->regionType = SCOREP_Pomp_Task;
            break;
        case POMP2_Taskuntied:
            region->regionType = SCOREP_Pomp_Task;
            break;
        case POMP2_Taskwait:
            region->regionType = SCOREP_Pomp_Taskwait;
            break;
        case POMP2_User_region:
            region->regionType = SCOREP_Pomp_UserRegion;
            break;
        case POMP2_Workshare:
            region->regionType = SCOREP_Pomp_Workshare;
            break;
        default:
            UTILS_ERROR( SCOREP_ERROR_POMP_UNKNOWN_REGION_TYPE, "%s", initString );
    }
    /*register handles*/
    scorep_pomp_register_region( region );
    /*free regionInfo since all information is copied to Score-P */
    freePOMP2RegionInfoMembers( &regionInfo );
}

/** @} */
