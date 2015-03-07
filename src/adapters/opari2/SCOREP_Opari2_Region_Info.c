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
 * Copyright (c) 2009-2014
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
 * @ingroup OPARI2
 *
 * @brief Implements functionality for interpretation of opari2 region
 *        strings.
 *
 * @note All functions in this file are only called during
 *       initialization. The initialization is assumed to be executed
 *       serially.
 *
 */

#include <config.h>
#include "SCOREP_Opari2_Region_Info.h"

#include <SCOREP_Definitions.h>

#include <UTILS_Error.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/** @ingroup OPARI2
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

bool scorep_opari2_recording_on = true;

/** Contains data for parsing a region info string */
typedef struct /* scorep_opari2_parsing_data */
{
    SCOREP_Opari2_Region* region;
    char*                 stringToParse;     /* will be modified */
    char*                 stringMemory;
    char*                 stringForErrorMsg; /* copy of the string used for error messages */
} scorep_opari2_parsing_data;

/** Possible token types which can appear in a region info string */
typedef enum
{
    SCOREP_OPARI2_TOKEN_TYPE_START_SOURCE_CODE_LOCATION,
    SCOREP_OPARI2_TOKEN_TYPE_END_SOURCE_CODE_LOCATION,
} scorep_opari2_token_type;

/* /\** Contains a token and its type from a region info string *\/ */
/* typedef struct */
/* { */
/*     char*                    tokenString; */
/*     scorep_opari2_token_type token; */
/* } scorep_opari2_token_map_entry; */

/* /\** Maps token names to scorep_opari2_token_type *\/ */
/* static const scorep_opari2_token_map_entry scorep_opari2_token_map[] = */
/* { */
/*     /\* Entries must be sorted to be used in binary search. *\/ */
/*     /\* If you add/remove items, scorep_opari2_token_map_size   *\/ */
/*     { "escl", SCOREP_OPARI2_TOKEN_TYPE_END_SOURCE_CODE_LOCATION   }, */
/*     { "sscl", SCOREP_OPARI2_TOKEN_TYPE_START_SOURCE_CODE_LOCATION }, */
/* }; */

/* /\** Number of entries in @ref scorep_opari2_token_map *\/ */
/* const size_t scorep_opari2_token_map_size = */
/*     sizeof( scorep_opari2_token_map ) / sizeof( scorep_opari2_token_map_entry ); */

typedef struct file_list_node file_list_node;

struct file_list_node
{
    SCOREP_Opari2_Region*   region;
    char*                   file_name;
    SCOREP_SourceFileHandle file_handle;
    file_list_node*         next;
};

static file_list_node* file_list_top = NULL;

/* **************************************************************************************
 *                                                                    Init/free functions
 ***************************************************************************************/

/** Initializes a SCOREP_Opari2_Region instance @region  with NULL values.
    @param region the SCOREP_Opari2_Region instance which get initialized.
 */
static void
scorep_opari2_init_region( SCOREP_Opari2_Region* region )
{
    region->startFileName = 0;
    region->startLine1    = 0;
    region->startLine2    = 0;
    region->endFileName   = 0;
    region->endLine1      = 0;
    region->endLine2      = 0;
}

/** Frees allocated memory and sets the pointer to 0. Used to free
    members of a SCOREP_Opari2_Region instance.
    @param member Pointer to the pointer which should be freed. If
                  member is 0, nothing happens.
 */
static void
scorep_opari2_free_region_member( char** member )
{
    if ( *member )
    {
        free( *member );
        *member = 0;
    }
}

void
scorep_opari2_free_region_members( SCOREP_Opari2_Region* region )
{
    scorep_opari2_free_region_member( &( region->startFileName ) );
    scorep_opari2_free_region_member( &( region->endFileName ) );
}

void
scorep_opari2_assign_generic_region_info( SCOREP_Opari2_Region* region,
                                          OPARI2_Region_info*   regionInfo )
{
    UTILS_ASSERT( region );
    if ( regionInfo->mStartFileName )
    {
        region->startFileName = ( char* )malloc( sizeof( char ) * ( strlen( regionInfo->mStartFileName ) + 1 ) );
        strcpy( region->startFileName, regionInfo->mStartFileName );
    }
    region->startLine1 = regionInfo->mStartLine1;
    region->startLine2 = regionInfo->mStartLine2;

    if ( regionInfo->mEndFileName )
    {
        region->endFileName = ( char* )malloc( sizeof( char ) * ( strlen( regionInfo->mEndFileName ) + 1 ) );
        strcpy( region->endFileName, regionInfo->mEndFileName );
    }
    region->endLine1 = regionInfo->mEndLine1;
    region->endLine2 = regionInfo->mEndLine2;
}

/* Maintains the file name of the current region */
SCOREP_SourceFileHandle
scorep_opari2_get_file_handle( SCOREP_Opari2_Region* region )
{
    static file_list_node* current = 0;

    /* If necessary register new source file */
    /* if ( ( last_file == SCOREP_INVALID_SOURCE_FILE ) || */
    /*      ( strcmp( last_file_name, genericRegion->startFileName ) != 0 ) ) */
    if ( file_list_top == NULL )
    {
        file_list_top              = malloc( sizeof( file_list_node ) );
        file_list_top->next        = 0;
        file_list_top->region      = region;
        file_list_top->file_name   = region->startFileName;
        file_list_top->file_handle =
            SCOREP_Definitions_NewSourceFile( region->startFileName );
        current = file_list_top;

        return file_list_top->file_handle;
    }

    if ( strcmp( current->file_name, region->startFileName ) != 0 )
    {
        current = file_list_top;
        while ( current != 0 )
        {
            if ( strcmp( current->file_name, region->startFileName ) == 0 )
            {
                return current->file_handle;
            }
            else
            {
                current = current->next;
            }
        }
    }

    current              = malloc( sizeof( file_list_node ) );
    current->region      = region;
    current->file_name   = region->startFileName;
    current->file_handle = SCOREP_Definitions_NewSourceFile( current->file_name );
    current->next        = file_list_top;
    file_list_top        = current;

    return current->file_handle;
}


/** @} */
