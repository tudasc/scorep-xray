/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SCOREP_Pomp_RegionInfo.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP
 *
 * @brief Implements functionality for interpretation of pomp region strings.
 * @note  All functions in this file are only called during initialization.
 *        The initialization is assumed to be executed serially.
 *
 */

#include <config.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <SCOREP_Pomp_RegionInfo.h>
#include <SCOREP_IO.h>
#include <SCOREP_Debug.h>
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
    char*                  innerRegionName;
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
  { "atomic",            "",                SCOREP_Pomp_Atomic              , SCOREP_REGION_OMP_ATOMIC,      SCOREP_REGION_UNKNOWN             },
  { "barrier",           "",                SCOREP_Pomp_Barrier             , SCOREP_REGION_OMP_BARRIER,     SCOREP_REGION_UNKNOWN             },
  { "critical",          "critical sblock", SCOREP_Pomp_Critical            , SCOREP_REGION_OMP_CRITICAL,    SCOREP_REGION_OMP_CRITICAL_SBLOCK },
  { "do",                "",                SCOREP_Pomp_Do                  , SCOREP_REGION_OMP_LOOP,        SCOREP_REGION_UNKNOWN             },
  { "flush",             "",                SCOREP_Pomp_Flush               , SCOREP_REGION_OMP_FLUSH,       SCOREP_REGION_UNKNOWN             },
  { "for",               "",                SCOREP_Pomp_For                 , SCOREP_REGION_OMP_LOOP,        SCOREP_REGION_UNKNOWN             },
  { "master",            "master",          SCOREP_Pomp_Master              , SCOREP_REGION_UNKNOWN,         SCOREP_REGION_OMP_MASTER          },
  { "ordered",           "ordered sblock",  SCOREP_Pomp_Ordered             , SCOREP_REGION_OMP_ORDERED,     SCOREP_REGION_OMP_ORDERED_SBLOCK  },
  { "parallel",          "",                SCOREP_Pomp_Parallel            , SCOREP_REGION_UNKNOWN,         SCOREP_REGION_UNKNOWN             },
  { "parallel do",       "",                SCOREP_Pomp_ParallelDo          , SCOREP_REGION_OMP_LOOP,        SCOREP_REGION_UNKNOWN             },
  { "parallel for",      "",                SCOREP_Pomp_ParallelFor         , SCOREP_REGION_OMP_LOOP,        SCOREP_REGION_UNKNOWN             },
  { "parallel sections", "section",         SCOREP_Pomp_ParallelSections    , SCOREP_REGION_OMP_SECTIONS,    SCOREP_REGION_OMP_SECTION         },
  { "parallel workshare","",                SCOREP_Pomp_ParallelWorkshare   , SCOREP_REGION_OMP_WORKSHARE,   SCOREP_REGION_UNKNOWN             },
  { "region",            "region",          SCOREP_Pomp_UserRegion          , SCOREP_REGION_UNKNOWN,         SCOREP_REGION_USER,               },
  { "sections",          "section",         SCOREP_Pomp_Sections            , SCOREP_REGION_OMP_SECTIONS,    SCOREP_REGION_OMP_SECTION         },
  { "single",            "single sblock",   SCOREP_Pomp_Single              , SCOREP_REGION_OMP_SINGLE,      SCOREP_REGION_OMP_SINGLE_SBLOCK,  },
  { "task",              "create task",     SCOREP_Pomp_Task                , SCOREP_REGION_OMP_TASK,        SCOREP_REGION_OMP_TASK_CREATE     },
  { "taskwait",          "",                SCOREP_Pomp_Taskwait            , SCOREP_REGION_OMP_TASKWAIT,    SCOREP_REGION_UNKNOWN             },
  { "workshare",         "",                SCOREP_Pomp_Workshare           , SCOREP_REGION_OMP_WORKSHARE,   SCOREP_REGION_UNKNOWN             }
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
    char*             region_name = 0;
    SCOREP_RegionType type_outer  = SCOREP_REGION_UNKNOWN;
    SCOREP_RegionType type_inner  = SCOREP_REGION_UNKNOWN;
    int32_t           start, end;

    /* Assume that all regions from one file are registered in a row.
       Thus, remember the last file handle and reuse it if the next region stems
       from the same source file.                                                 */
    static char*                   last_file_name = 0;
    static SCOREP_SourceFileHandle last_file      = SCOREP_INVALID_SOURCE_FILE;

    /* Evtl. register new source file */
    if ( ( last_file == SCOREP_INVALID_SOURCE_FILE ) ||
         ( strcmp( last_file_name, region->startFileName ) != 0 ) )
    {
        last_file_name = region->startFileName;
        last_file      = SCOREP_DefineSourceFile( last_file_name );
    }

    /* Construct file:lno string */
    const char* basename    = SCOREP_IO_GetWithoutPath( region->startFileName );
    char*       source_name = ( char* )malloc( strlen( basename ) + 12 );
    sprintf( source_name, "@%s:%" PRIi32, basename, region->startLine1 );


    /* User regions can be filtered */
    if ( ( region->regionType == SCOREP_Pomp_UserRegion ) &&
         SCOREP_Filter_Match( region->startFileName, region_name, false ) )
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
    if ( ( region->regionType >= SCOREP_Pomp_Parallel ) &&
         ( region->regionType <= SCOREP_Pomp_ParallelWorkshare ) )
    {
        int   length        = 16 + strlen( source_name ) + 1;
        char* parallel_name = ( char* )malloc( length );
        sprintf( parallel_name, "!$omp parallel %s", source_name );

        region->innerParallel = SCOREP_DefineRegion( parallel_name,
                                                     last_file,
                                                     region->startLine1,
                                                     region->endLine2,
                                                     SCOREP_ADAPTER_POMP,
                                                     SCOREP_REGION_OMP_PARALLEL );
        free( parallel_name );
    }

    /* Register outer region */
    if ( type_outer != SCOREP_REGION_UNKNOWN )
    {
        char* type_name = scorep_pomp_region_type_map[ region->regionType ].regionTypeString;
        int   length    = strlen( type_name ) + 7 + strlen( source_name ) + 1;
        region_name = ( char* )malloc( length );
        sprintf( region_name, "!$omp %s %s", type_name, source_name );

        if ( ( region->regionType >= SCOREP_Pomp_ParallelDo ) &&
             ( region->regionType <= SCOREP_Pomp_ParallelWorkshare ) )
        {
            start = region->startLine2;
            end   = region->endLine1;
        }
        else
        {
            start = region->startLine1;
            end   = region->endLine2;
        }

        region->outerBlock = SCOREP_DefineRegion( region_name,
                                                  last_file,
                                                  start,
                                                  end,
                                                  SCOREP_ADAPTER_POMP,
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
        int length = strlen( type_name ) + 7 + strlen( source_name ) + 1;
        region_name = ( char* )malloc( length );
        sprintf( region_name, "!$omp %s %s", type_name, source_name );

        region->innerBlock = SCOREP_DefineRegion( region_name,
                                                  last_file,
                                                  region->startLine2,
                                                  region->endLine1,
                                                  SCOREP_ADAPTER_POMP,
                                                  type_inner );
        free( region_name );
    }

    free( source_name );

#ifdef _OPENMP
    /* Register locks for critical regions */
    if ( region->regionType == SCOREP_Pomp_Critical )
    {
        region->lock = SCOREP_Pomp_GetLock( region->name );
        if ( region->lock == NULL )
        {
            region->lock = scorep_pomp_lock_init( region->name );
        }
    }
#endif
}

/* **************************************************************************************
 *                                                                     Exported functions
 ***************************************************************************************/

void
SCOREP_Pomp_ParseInitString( const char          initString[],
                             SCOREP_Pomp_Region* region )
{
    SCOREP_ASSERT( region );
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
            SCOREP_ERROR( SCOREP_ERROR_POMP_UNKNOWN_REGION_TYPE, "%s", initString );
    }
    /*register handles*/
    scorep_pomp_register_region( region );
    /*free regionInfo since all information is copied to Score-P */
    freePOMP2RegionInfoMembers( &regionInfo );
}

/** @} */
