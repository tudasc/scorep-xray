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
 * Copyright (c) 2009-2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Implements functionality for the management of OpenMP regions.
 *
 */

#include <config.h>
#include "SCOREP_Opari2_Openmp_Regions.h"

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
#include <pomp2_region_info.h>

#include <opari2/pomp2_lib.h>

/** @ingroup OPARI2
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

/**
   Lock to protect on-the-fly assignments.
 */
SCOREP_Mutex scorep_opari2_openmp_assign_lock = SCOREP_INVALID_MUTEX;

/** Contains the data for one region type */
typedef struct
{
    char*                            outerRegionName;
    char*                            innerRegionName;
    bool                             hasParallel;
    bool                             hasImplicitBarrier;
    SCOREP_Opari2_Openmp_Region_Type regionType;
    SCOREP_RegionType                outerRegionType;
    SCOREP_RegionType                innerRegionType;
} region_type_map_entry;

/* *INDENT-OFF* */
/** Maps region name in the information string to SCOREP_Opari2_RegionType and
    SCOREP_RegionType of the inner and outer region.
*/
static const region_type_map_entry region_type_map[] =
{
    /* Entries must be sorted to be used in binary search. */
    /* If you add/remove items, region_type_map_size. */
    /* Entries must be in same order like SCOREP_Opari2_RegionType to allow lookup. */
  { "atomic",        "",                false, false, SCOREP_POMP2_Atomic            , SCOREP_REGION_ATOMIC,      SCOREP_REGION_UNKNOWN         },
  { "barrier",       "",                false, false, SCOREP_POMP2_Barrier           , SCOREP_REGION_BARRIER,     SCOREP_REGION_UNKNOWN         },
  { "critical",      "critical sblock", false, false, SCOREP_POMP2_Critical          , SCOREP_REGION_CRITICAL,    SCOREP_REGION_CRITICAL_SBLOCK },
  { "do",            "",                false, true,  SCOREP_POMP2_Do                , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "flush",         "",                false, false, SCOREP_POMP2_Flush             , SCOREP_REGION_FLUSH,       SCOREP_REGION_UNKNOWN         },
  { "for",           "",                false, true,  SCOREP_POMP2_For               , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "",              "master",          false, false, SCOREP_POMP2_Master            , SCOREP_REGION_UNKNOWN,     SCOREP_REGION_MASTER          },
  { "ordered",       "ordered sblock",  false, false, SCOREP_POMP2_Ordered           , SCOREP_REGION_ORDERED,     SCOREP_REGION_ORDERED_SBLOCK  },
  { "",              "",                true,  true,  SCOREP_POMP2_Parallel          , SCOREP_REGION_UNKNOWN,     SCOREP_REGION_UNKNOWN         },
  { "do",            "",                true,  true,  SCOREP_POMP2_ParallelDo        , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "for",           "",                true,  true,  SCOREP_POMP2_ParallelFor       , SCOREP_REGION_LOOP,        SCOREP_REGION_UNKNOWN         },
  { "sections",      "section",         true,  true,  SCOREP_POMP2_ParallelSections  , SCOREP_REGION_SECTIONS,    SCOREP_REGION_SECTION         },
  { "workshare",     "",                true,  true,  SCOREP_POMP2_ParallelWorkshare , SCOREP_REGION_WORKSHARE,   SCOREP_REGION_UNKNOWN         },
  { "sections",      "section",         false, true,  SCOREP_POMP2_Sections          , SCOREP_REGION_SECTIONS,    SCOREP_REGION_SECTION         },
  { "single",        "single sblock",   false, true,  SCOREP_POMP2_Single            , SCOREP_REGION_SINGLE,      SCOREP_REGION_SINGLE_SBLOCK,  },
  { "create task",   "task",            false, false, SCOREP_POMP2_Task              , SCOREP_REGION_TASK_CREATE, SCOREP_REGION_TASK            },
  { "create untied", "untied task",     false, false, SCOREP_POMP2_Taskuntied        , SCOREP_REGION_TASK_CREATE, SCOREP_REGION_TASK_UNTIED     },
  { "taskwait",      "",                false, false, SCOREP_POMP2_Taskwait          , SCOREP_REGION_TASK_WAIT,   SCOREP_REGION_UNKNOWN         },
  { "workshare",     "",                false, true,  SCOREP_POMP2_Workshare         , SCOREP_REGION_WORKSHARE,   SCOREP_REGION_UNKNOWN         }
};
/* *INDENT-ON* */

/** Number of entries in region_type_map */
static const size_t region_type_map_size =
    sizeof( region_type_map ) / sizeof( region_type_map_entry );

/* **************************************************************************************
 *                                                                    Init/free functions
 ***************************************************************************************/

/** Initializes a SCOREP_Opari2_Openmp_Region instance @region  with NULL values.
    @param region the SCOREP_Opari2_Openmp_Region instance which get initialized.
 */
static void
scorep_opari2_init_openmp_region( SCOREP_Opari2_Openmp_Region* region )
{
    region->name          = 0;
    region->numSections   = 0;
    region->regionType    = SCOREP_POMP2_NONE;
    region->innerParallel = SCOREP_INVALID_REGION;
    region->outerBlock    = SCOREP_INVALID_REGION;
    region->innerBlock    = SCOREP_INVALID_REGION;
    region->barrier       = SCOREP_INVALID_REGION;
}

/** Registers the opari2_openmp regions to scorep and sets the SCOREP region handle fields in
    a SCOREP_Opari2_Openmp_Region instance. All necessary data must be already contained in the
    SCOREP_Opari2_Openmp_Region instance.
    @param region  The region infromation struct for a POMP region which gets registered
                   to SCOREP. The region handle fields of it are set in this function.
 */
void
scorep_opari2_register_openmp_region( SCOREP_Opari2_Openmp_Region* region )
{
    char*                 region_name = 0;
    SCOREP_Opari2_Region* genericInfo = &( region->genericInfo );
    SCOREP_RegionType     type_outer  = SCOREP_REGION_UNKNOWN;
    SCOREP_RegionType     type_inner  = SCOREP_REGION_UNKNOWN;
    SCOREP_ParadigmType   paradigm    = SCOREP_PARADIGM_OPENMP;
    int32_t               start, end;

    /* Consistency checks */
    if ( ( 0 > region->regionType ) ||
         ( region->regionType > region_type_map_size ) )
    {
        UTILS_ERROR( SCOREP_ERROR_INDEX_OUT_OF_BOUNDS,
                     "Region type %d not found in region type table.",
                     region->regionType );
        exit( EXIT_FAILURE );
    }

    SCOREP_SourceFileHandle file_handle =
        scorep_opari2_get_file_handle( ( SCOREP_Opari2_Region* )region );
    /* Construct file:lno string */
    const char* basename    = UTILS_IO_GetWithoutPath( genericInfo->startFileName );
    char*       source_name = ( char* )malloc( strlen( basename ) + 12 );
    sprintf( source_name, "@%s:%" PRIi32, basename, genericInfo->startLine1 );

    /* Determine type of inner and outer regions */
    type_outer = region_type_map[ region->regionType ].outerRegionType;
    type_inner = region_type_map[ region->regionType ].innerRegionType;

    /* Register parallel region */
    if ( region_type_map[ region->regionType ].hasParallel )
    {
        int   length        = 16 + strlen( source_name ) + 1;
        char* parallel_name = ( char* )malloc( length );
        sprintf( parallel_name, "!$omp parallel %s", source_name );

        region->innerParallel = SCOREP_Definitions_NewRegion( parallel_name,
                                                              NULL,
                                                              file_handle,
                                                              genericInfo->startLine1,
                                                              genericInfo->endLine2,
                                                              SCOREP_PARADIGM_OPENMP,
                                                              SCOREP_REGION_PARALLEL );
        free( parallel_name );
    }

    /* Register outer region */
    if ( type_outer != SCOREP_REGION_UNKNOWN )
    {
        char* type_name;
        if ( region->name == 0 )
        {
            type_name = region_type_map[ region->regionType ].outerRegionName;
        }
        else
        {
            type_name = region->name;
        }
        int length = strlen( type_name ) + 7 + strlen( source_name ) + 1;
        region_name = ( char* )malloc( length );
        sprintf( region_name, "!$omp %s %s", type_name, source_name );
        if ( region_type_map[ region->regionType ].hasParallel )
        {
            start = genericInfo->startLine2;
            end   = genericInfo->endLine1;
        }
        else
        {
            start = genericInfo->startLine1;
            end   = genericInfo->endLine2;
        }
        region->outerBlock = SCOREP_Definitions_NewRegion( region_name,
                                                           NULL,
                                                           file_handle,
                                                           start,
                                                           end,
                                                           paradigm,
                                                           type_outer );
        free( region_name );
    }

    /* Register inner region */
    if ( type_inner != SCOREP_REGION_UNKNOWN )
    {
        char* type_name = region_type_map[ region->regionType ].innerRegionName;
        int   length    = strlen( type_name ) + 8 + strlen( source_name ) + 1;
        region_name = ( char* )malloc( length );
        sprintf( region_name, "!$omp %s %s", type_name, source_name );
        region->innerBlock = SCOREP_Definitions_NewRegion( region_name,
                                                           NULL,
                                                           file_handle,
                                                           genericInfo->startLine2,
                                                           genericInfo->endLine1,
                                                           paradigm,
                                                           type_inner );
        free( region_name );
    }

    /* Register implicit barrier */
    if ( region_type_map[ region->regionType ].hasImplicitBarrier )
    {
        int   length       = 24 + strlen( basename ) + 1 + 10 + 1;
        char* barrier_name = ( char* )malloc( length );
        sprintf( barrier_name, "!$omp implicit barrier @%s:%" PRIu32, basename, genericInfo->endLine1 );

        region->barrier = SCOREP_Definitions_NewRegion( barrier_name,
                                                        NULL,
                                                        file_handle,
                                                        genericInfo->endLine1,
                                                        genericInfo->endLine2,
                                                        SCOREP_PARADIGM_OPENMP,
                                                        SCOREP_REGION_IMPLICIT_BARRIER );
        free( barrier_name );
    }


    free( source_name );

    /* Register locks for critical regions */
    if ( region->regionType == SCOREP_POMP2_Critical )
    {
        region->lock = SCOREP_Opari2_Openmp_GetLock( region->name );
        if ( region->lock == NULL )
        {
            region->lock = scorep_opari2_openmp_lock_init( region->name );
        }
    }
}

typedef struct criticals_list criticals_list;
struct criticals_list
{
    char*           name;
    criticals_list* next;
};
static criticals_list* criticals_head;

/**
 * parse_openmp_init_string() fills the SCOREP_Opari2_Openmp_Region
 * object with data read from the @a initString. If the initString
 * does not comply with the specification, the program aborts with
 * exit code 1. @n Rationale: parse_openmp_init_string() is used
 * during initialization of the measurement system. If an error
 * occurs, it is better to abort than to struggle with undefined
 * behaviour or @e guessing the meaning of the broken string.
 *
 * @note Can be called from multiple threads concurrently, assuming
 * malloc is thread-safe.
 *
 * @note parse_openmp_init_string() will assign memory to the members
 * of @e region.
 *
 * @param initString A string in the format
 * "length*key=value*[key=value]*". The length field is parsed but not
 * used by this implementation. The string must at least contain
 * values for the keys @c regionType, @c sscl and @c escl. Possible
 * values for the key @c regionType are listed in @ref
 * region_type_map. The format for @c sscl resp.  @c escl values is @c
 * "filename:lineNo1:lineNo2".
 *
 * @param region must be a valid object
 *
 * @post At least the required attributes (see
 * SCOREP_Opari2_Openmp_Region) are set. @n All other members of @e
 * region are set to 0 resp. false. @n If @c regionType=sections than
 * SCOREP_Opari2_Openmp_Region::numSections has a value > 0. @n If @c
 * regionType=region than SCOREP_Opari2_Openmp_Region::regionName has
 * a value != 0. @n If @c regionType=critical than
 * SCOREP_Opari2_Openmp_Regions::regionName may have a value != 0.
 *
 */
static void
parse_openmp_init_string( const char                   initString[],
                          SCOREP_Opari2_Openmp_Region* region )
{
    UTILS_ASSERT( region );

    POMP2_Region_info regionInfo;

    ctcString2RegionInfo( initString, &regionInfo );
    scorep_opari2_assign_generic_region_info( ( SCOREP_Opari2_Region* )region,
                                              ( OPARI2_Region_info* )&regionInfo );

    scorep_opari2_init_openmp_region( region );

    /* Assign criticals a name but reuse the name if it already exists. */
    if ( regionInfo.mRegionType == POMP2_Critical )
    {
        if ( regionInfo.mCriticalName == NULL )
        {
            static char* unnamed_critical = "critical";
            region->name = unnamed_critical;
        }
        else
        {
            char name[ sizeof( char ) * ( strlen( regionInfo.mCriticalName ) + 12 ) ];
            strcpy( &name[ 0 ], "critical (" );
            strcat( &name[ 0 ], regionInfo.mCriticalName );
            strcat( &name[ 0 ], ")" );

            criticals_list* critical = criticals_head;
            while ( critical )
            {
                if ( strcmp( &name[ 0 ], critical->name ) == 0 )
                {
                    break;
                }
                critical = critical->next;
            }
            if ( !critical )
            {
                criticals_list* new_critical =  malloc( sizeof( criticals_list ) );
                new_critical->name = malloc( sizeof( char ) * ( strlen( name ) + 1 ) );
                strcpy( new_critical->name, &name[ 0 ] );
                new_critical->next = criticals_head;
                criticals_head     = new_critical;
                critical           = criticals_head;
            }
            region->name = critical->name;
        }
    }
    region->numSections = regionInfo.mNumSections;


    switch ( regionInfo.mRegionType )
    {
        case POMP2_No_type:
            region->regionType = SCOREP_POMP2_NONE;
            break;
        case POMP2_Atomic:
            region->regionType = SCOREP_POMP2_Atomic;
            break;
        case POMP2_Barrier:
            region->regionType = SCOREP_POMP2_Barrier;
            break;
        case POMP2_Critical:
            region->regionType = SCOREP_POMP2_Critical;
            break;
        case POMP2_Do:
            region->regionType = SCOREP_POMP2_Do;
            break;
        case POMP2_Flush:
            region->regionType = SCOREP_POMP2_Flush;
            break;
        case POMP2_For:
            region->regionType = SCOREP_POMP2_For;
            break;
        case POMP2_Master:
            region->regionType = SCOREP_POMP2_Master;
            break;
        case POMP2_Ordered:
            region->regionType = SCOREP_POMP2_Ordered;
            break;
        case POMP2_Parallel:
            region->regionType = SCOREP_POMP2_Parallel;
            break;
        case POMP2_Parallel_do:
            region->regionType = SCOREP_POMP2_ParallelDo;
            break;
        case POMP2_Parallel_for:
            region->regionType = SCOREP_POMP2_ParallelFor;
            break;
        case POMP2_Parallel_sections:
            region->regionType = SCOREP_POMP2_ParallelSections;
            break;
        case POMP2_Parallel_workshare:
            region->regionType = SCOREP_POMP2_ParallelWorkshare;
            break;
        case POMP2_Sections:
            region->regionType = SCOREP_POMP2_Sections;
            break;
        case POMP2_Single:
            region->regionType = SCOREP_POMP2_Single;
            break;
        case POMP2_Task:
            region->regionType = SCOREP_POMP2_Task;
            break;
        case POMP2_Taskuntied:
            region->regionType = SCOREP_POMP2_Taskuntied;
            break;
        case POMP2_Taskwait:
            region->regionType = SCOREP_POMP2_Taskwait;
            break;
        case POMP2_Workshare:
            region->regionType = SCOREP_POMP2_Workshare;
            break;
        default:
            UTILS_ERROR( SCOREP_ERROR_POMP_UNKNOWN_REGION_TYPE, "%s", initString );
    }
    /*register handles*/
    scorep_opari2_register_openmp_region( region );
    /*free regionInfo since all information is copied to Score-P */
    freePOMP2RegionInfoMembers( &regionInfo );
}

void
scorep_opari2_openmp_criticals_finalize( void )
{
    criticals_list* critical = criticals_head;
    while ( critical )
    {
        criticals_list* next = critical->next;
        free( critical->name );
        free( critical );
        critical = next;
    }
}

/* ***********************************************************************
 *                                           Assign handles
 ************************************************************************/

extern SCOREP_Opari2_Openmp_Region* scorep_opari2_openmp_regions;

void
POMP2_Assign_handle( POMP2_Region_handle* opari2Handle,
                     const char           initString[] )
{
    SCOREP_MutexLock( scorep_opari2_openmp_assign_lock );
    if ( *opari2Handle == NULL )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPARI2, "In POMP2_Assign_handle" );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPARI2, "   %s", initString );

        /* Index counter */
        static size_t                count       = 0;
        const size_t                 max_regions = POMP2_Get_num_regions();
        SCOREP_Opari2_Openmp_Region* new_handle  = 0;

        /* If we have on-the-fly registration, we might need to increase the
           buffer for our regions */
        if ( count < max_regions )
        {
            new_handle = &scorep_opari2_openmp_regions[ count ];
        }
        else
        {
            new_handle = malloc( sizeof( SCOREP_Opari2_Openmp_Region ) );
        }

        /* Initialize new region struct */
        parse_openmp_init_string( initString, new_handle );

        /* Set return value */
        *opari2Handle = new_handle;

        /* Increase array index */
        ++count;
    }
    SCOREP_MutexUnlock( scorep_opari2_openmp_assign_lock );
}


/** @} */
