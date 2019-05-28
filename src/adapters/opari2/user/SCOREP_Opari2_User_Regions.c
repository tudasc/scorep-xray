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
 * @brief Implements functionality for management of user regions.
 *
 */

#include <config.h>
#include "SCOREP_Opari2_User_Regions.h"

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
#include <SCOREP_Filtering.h>

#include <opari2/pomp2_user_lib.h>

/** @ingroup OPARI2
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

/* **************************************************************************************
 *                                                                    Init/free functions
 ***************************************************************************************/

/** Initializes a SCOREP_Opari2_User_Region instance @region  with NULL values.
    @param region the SCOREP_Opari2_User_Region instance which get initialized.
 */
static void
scorep_opari2_user_init_region( SCOREP_Opari2_User_Region* region )
{
    region->name = 0;
}

/** Registers the pomp regions to scorep and sets the SCOREP region handle fields in
    a SCOREP_Pomp_Region instance. All necessary data must be already contained in the
    SCOREP_Pomp_Region instance.
    @param region  The region infromation struct for a POMP region which gets registered
                   to SCOREP. The region handle fields of it are set in this function.
 */
void
scorep_opari2_user_register_region( SCOREP_Opari2_User_Region* region )
{
    SCOREP_ParadigmType   paradigm = SCOREP_PARADIGM_USER;
    int32_t               start, end;
    SCOREP_Opari2_Region* genericInfo = &( region->genericInfo );

    SCOREP_SourceFileHandle file_handle =
        scorep_opari2_get_file_handle( ( SCOREP_Opari2_Region* )region );

    /* Construct file:lno string */
    const char* basename    = UTILS_IO_GetWithoutPath( genericInfo->startFileName );
    char*       source_name = ( char* )malloc( strlen( basename ) + 12 );
    sprintf( source_name, "@%s:%" PRIi32, basename, genericInfo->startLine1 );

    /* User regions can be filtered */
    if ( SCOREP_Filtering_Match( genericInfo->startFileName, NULL, NULL ) )
    {
        free( source_name );
        return;
    }

    /* Register region */
    int   length      = 0;
    char* region_name = NULL;
    if ( region->name )
    {
        length      = 18 + strlen( source_name ) + strlen( region->name ) + 1;
        region_name = ( char* )malloc( length );
        sprintf( region_name, "!$pomp user(%s) %s", region->name, source_name );
    }
    else
    {
        length      = 16 + strlen( source_name ) + 1;
        region_name = ( char* )malloc( length );
        sprintf( region_name, "!$pomp user %s", source_name );
    }

    region->regionHandle = SCOREP_Definitions_NewRegion( region_name,
                                                         NULL,
                                                         file_handle,
                                                         genericInfo->startLine1,
                                                         genericInfo->endLine2,
                                                         SCOREP_PARADIGM_USER,
                                                         SCOREP_REGION_USER );
    free( region_name );
    free( source_name );
}


/**
 * parse_user_init_string() fills the SCOREP_Pomp_Region object with data read
 * from the @a initString. If the initString does not comply with the
 * specification, the program aborts with exit code 1. @n Rationale:
 * parse_user_init_string() is used during initialization of the measurement
 * system. If an error occurs, it is better to abort than to struggle with
 * undefined behaviour or @e guessing the meaning of the broken string.
 *
 * @note Can be called from multiple threads concurrently, assuming malloc is
 * thread-safe.
 *
 * @note parse_user_init_string() will assign memory to the members of @e
 * region.
 *
 * @param initString A string in the format
 * "length*key=value*[key=value]*". The length field is parsed but not used by
 * this implementation. Possible values for key are listed in
 * scorep_pomp_token_map. The string must at least contain values for the keys @c
 * regionType, @c sscl and @c escl. Possible values for the key @c regionType
 * are listed in @ref scorep_pomp_region_type_map. The format for @c sscl resp.
 * @c escl values
 * is @c "filename:lineNo1:lineNo2".
 *
 * @param region must be a valid object
 *
 * @post At least the required attributes (see SCOREP_Pomp_Region) are set. @n
 * All other members of @e region are set to 0 resp. false.
 * @n If @c regionType=sections than
 * SCOREP_Pomp_Region::numSections has a value > 0. @n If @c regionType=region
 * than SCOREP_Pomp_Region::regionName has a value != 0. @n If @c
 * regionType=critical than SCOREP_Pomp_RegionInfo::regionName may have a value
 * != 0.
 *
 */
static void
parse_user_init_string( const char                 initString[],
                        SCOREP_Opari2_User_Region* region )
{
    UTILS_ASSERT( region );
    POMP2_USER_Region_info regionInfo;
    ctcString2UserRegionInfo( initString, &regionInfo );

    scorep_opari2_assign_generic_region_info( ( SCOREP_Opari2_Region* )region,
                                              ( OPARI2_Region_info* )&regionInfo );
    if ( regionInfo.mUserRegionName )
    {
        region->name = ( char* )malloc( sizeof( char ) * ( strlen( regionInfo.mUserRegionName ) + 1 ) );
        strcpy( region->name, regionInfo.mUserRegionName );
    }
    /*register handles*/
    scorep_opari2_user_register_region( region );
    /*free regionInfo since all information is copied to Score-P */
    freePOMP2UserRegionInfoMembers( &regionInfo );
}


/* ***********************************************************************
 *                                           Assign handles
 ************************************************************************/

extern SCOREP_Opari2_User_Region* scorep_opari2_user_regions;

void
POMP2_USER_Assign_handle( POMP2_USER_Region_handle* opari2Handle,
                          const char                initString[] )
{
    SCOREP_MutexLock( scorep_opari2_user_assign_lock );
    if ( *opari2Handle == NULL )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPARI2, "In POMP2_USER_Assign_handle" );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPARI2, "   %s", initString );

        /* Index counter */
        static size_t              count       = 0;
        const size_t               max_regions = POMP2_USER_Get_num_regions();
        SCOREP_Opari2_User_Region* new_handle  = 0;

        /* If we have on-the-fly registration, we might need to increase the
           buffer for our regions */
        if ( count < max_regions )
        {
            new_handle = &scorep_opari2_user_regions[ count ];
        }
        else
        {
            new_handle = malloc( sizeof( SCOREP_Opari2_User_Region ) );
        }

        /* Initialize new region struct */
        parse_user_init_string( initString, new_handle );

        /* Set return value */
        *opari2Handle = new_handle;

        /* Increase array index */
        ++count;
    }
    SCOREP_MutexUnlock( scorep_opari2_user_assign_lock );
}

/** @} */
