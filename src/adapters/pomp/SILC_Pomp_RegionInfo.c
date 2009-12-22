/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Pomp_RegionInfo.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    POMP
 *
 * @brief Implements functionality for interpretation of pomp region strings.
 *
 */

#include "SILC_Pomp_RegionInfo.h"
#include "SILC_Utils.h"
#include "SILC_Definitions.h"
#include "SILC_Types.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

typedef struct /* silc_pomp_parsing_data */
{
    SILC_Pomp_Region* region;
    char*             stringToParse; /* will be modified */
    char*             stringMemory;
    char*             stringForErrorMsg;
} silc_pomp_parsing_data;

typedef enum
{
    SILC_POMP_TOKEN_TYPE_REGION_TYPE,
    SILC_POMP_TOKEN_TYPE_START_SOURCE_CODE_LOCATION,
    SILC_POMP_TOKEN_TYPE_END_SOURCE_CODE_LOCATION,
    SILC_POMP_TOKEN_TYPE_NUM_SECTIONS,
    SILC_POMP_TOKEN_TYPE_CRITICAL_NAME,
    SILC_POMP_TOKEN_TYPE_USER_REGION_NAME,
    SILC_POMP_TOKEN_TYPE_NO_TOKEN
} silc_pomp_token_type;

typedef struct
{
    char*                tokenString;
    silc_pomp_token_type token;
} silc_pomp_token_map_entry;

static const silc_pomp_token_map_entry silc_pomp_token_map[] =
{
    /* Entries must be sorted to be used in binary search. */
    /* If you add/remove items, silc_pomp_token_map_size   */
    { "criticalName",   SILC_POMP_TOKEN_TYPE_CRITICAL_NAME                        },
    { "escl",           SILC_POMP_TOKEN_TYPE_END_SOURCE_CODE_LOCATION             },
    { "numSections",    SILC_POMP_TOKEN_TYPE_NUM_SECTIONS                         },
    { "regionType",     SILC_POMP_TOKEN_TYPE_REGION_TYPE                          },
    { "sscl",           SILC_POMP_TOKEN_TYPE_START_SOURCE_CODE_LOCATION           },
    { "userRegionName", SILC_POMP_TOKEN_TYPE_USER_REGION_NAME                     }
};

const size_t silc_pomp_token_map_size = 6;

typedef struct
{
    char*                regionTypeString;
    SILC_Pomp_RegionType regionType;
    SILC_RegionType      outerRegionType;
    SILC_RegionType      innerRegionType;
} silc_pomp_region_type_map_entry;

/* *INDENT-OFF* */
static const silc_pomp_region_type_map_entry silc_pomp_region_type_map[] =
{
    /* Entries must be sorted to be used in binary search. */
    /* If you add/remove items, silc_pomp_region_type_map_size. */
    /* Entries must be in same order like SILC_Pomp_RegionType to allow lookup. */
  { "atomic",            SILC_Pomp_Atomic              , SILC_REGION_OMP_ATOMIC,    SILC_REGION_UNKNOWN              },
  { "barrier",           SILC_Pomp_Barrier             , SILC_REGION_OMP_BARRIER,   SILC_REGION_UNKNOWN              },
  { "critical",          SILC_Pomp_Critical            , SILC_REGION_OMP_CRITICAL,  SILC_REGION_OMP_CRITICAL_SBLOCK  },
  { "do",                SILC_Pomp_Do                  , SILC_REGION_OMP_LOOP,      SILC_REGION_UNKNOWN              },
  { "flush",             SILC_Pomp_Flush               , SILC_REGION_OMP_FLUSH,     SILC_REGION_UNKNOWN              },
  { "for",               SILC_Pomp_For                 , SILC_REGION_OMP_LOOP,      SILC_REGION_UNKNOWN              },
  { "master",            SILC_Pomp_Master              , SILC_REGION_OMP_MASTER,    SILC_REGION_UNKNOWN              },
  { "parallel",          SILC_Pomp_Parallel            , SILC_REGION_OMP_PARALLEL,  SILC_REGION_UNKNOWN              },
  { "paralleldo",        SILC_Pomp_ParallelDo          , SILC_REGION_OMP_PARALLEL,  SILC_REGION_OMP_LOOP             },
  { "parallelfor",       SILC_Pomp_ParallelFor         , SILC_REGION_OMP_PARALLEL,  SILC_REGION_OMP_LOOP             },
  { "parallelsections",  SILC_Pomp_ParallelSections    , SILC_REGION_OMP_PARALLEL,  SILC_REGION_OMP_SECTIONS         },
  { "parallelworkshare", SILC_Pomp_ParallelWorkshare   , SILC_REGION_OMP_PARALLEL,  SILC_REGION_OMP_WORKSHARE        },
  { "region",            SILC_Pomp_UserRegion          , SILC_REGION_USER,          SILC_REGION_UNKNOWN              },
  { "sections",          SILC_Pomp_Sections            , SILC_REGION_OMP_SECTIONS,  SILC_REGION_UNKNOWN              },
  { "single",            SILC_Pomp_Single              , SILC_REGION_OMP_SINGLE,    SILC_REGION_UNKNOWN              },
  { "workshare",         SILC_Pomp_Workshare           , SILC_REGION_OMP_WORKSHARE, SILC_REGION_UNKNOWN              }
};
/* *INDENT-OFF* */

const size_t silc_pomp_region_type_map_size = 16;

/* **************************************************************************************
 *                                                                   Conversion functions
 ***************************************************************************************/

static int
silc_pomp_region_type_map_compare( const void* searchKey,
                                   const void* mapElem )
{
    const char* const key  = ( const char* )searchKey;
    silc_pomp_region_type_map_entry* elem = ( silc_pomp_region_type_map_entry* )mapElem;

    return strcmp( key, elem->regionTypeString );
}

static int
silc_pomp_token_map_compare( const void* searchToken,
                    const void* mapElem )
{
    const char* const     token = ( const char* )searchToken;
    silc_pomp_token_map_entry* elem  = ( silc_pomp_token_map_entry* )mapElem;

    return strcmp( token, elem->tokenString );
}

static silc_pomp_token_type
silc_pomp_get_token_from_string( char* token )
{
    silc_pomp_token_map_entry* mapElem = ( silc_pomp_token_map_entry* )bsearch(
        token,
        &silc_pomp_token_map,
        silc_pomp_token_map_size,
        sizeof( silc_pomp_token_map_entry ),
        silc_pomp_token_map_compare );

    if ( mapElem )
    {
        return mapElem->token;
    }
    else
    {
        return SILC_POMP_TOKEN_TYPE_NO_TOKEN;
    }
}

static SILC_Pomp_RegionType
silc_pomp_get_region_type_from_string( const char* regionTypeString )
{
    silc_pomp_region_type_map_entry* mapElem = ( silc_pomp_region_type_map_entry* )
                                     bsearch(
                                              regionTypeString,
                                              &silc_pomp_region_type_map,
                                              silc_pomp_region_type_map_size,
                                              sizeof( silc_pomp_region_type_map_entry ),
                                              silc_pomp_region_type_map_compare );

    if ( mapElem )
    {
        return mapElem->regionType;
    }
    else
    {
        return SILC_Pomp_NoType;
    }
}

/* **************************************************************************************
 *                                                                    Init/free functions
 ***************************************************************************************/

static void
silc_pomp_init_region( SILC_Pomp_Region* region )
{
    region->regionType       = SILC_Pomp_NoType;
    region->name             = 0;
    region->numSections      = 0;
    region->outerBlock       = SILC_INVALID_REGION;
    region->innerBlock       = SILC_INVALID_REGION;
    region->startFileName    = 0;
    region->startLine1       = 0;
    region->startLine2       = 0;
    region->endFileName      = 0;
    region->endLine1         = 0;
    region->endLine2         = 0;
    region->regionName       = 0;
}

static void
silc_pomp_init_parsing_data( silc_pomp_parsing_data*          obj,
                             const char        string[],
                             SILC_Pomp_Region* region )
{
    /* Size of the init string */
    const size_t nBytes = strlen( string ) * sizeof( char ) + 1;

    /* Set fields */
    obj->region = region;
    obj->stringToParse     = 0;
    obj->stringMemory      = 0;
    obj->stringForErrorMsg = 0;

    /* Allocate memory for strings */
    obj->stringMemory      = malloc( nBytes );
    obj->stringForErrorMsg = malloc( nBytes );
    obj->stringToParse     = obj->stringMemory;

    /* Copy strings */
    strcpy( obj->stringMemory, string );
    strcpy( obj->stringForErrorMsg, string );
}


static void
silc_pomp_free_parsing_data( silc_pomp_parsing_data* obj )
{
    /* Free memory */
    free (obj->stringMemory);
    free (obj->stringForErrorMsg);

    /* Set to 0 */
    obj->stringMemory      = 0;
    obj->stringForErrorMsg = 0;
    obj->stringToParse     = 0;
}

/* **************************************************************************************
 *                                                            internal parsings functions
 ***************************************************************************************/

static void
silc_pomp_assign_string( char**      aString,
              const char* value )
{
    *aString = malloc( strlen( value ) * sizeof( char ) + 1 );
    strcpy( *aString, value );
}

static bool
silc_pomp_extract_next_token( char**     string,
                  const char tokenDelimiter )
{
    *string = strchr( *string, tokenDelimiter );
    if ( !( *string && **string == tokenDelimiter ) )
    {
        return false;
    }
    **string = '\0'; /* extraction */
    ++( *string );
    return true;
}

static bool
silc_pomp_get_key_value_pair( silc_pomp_parsing_data* obj,
                 char**   key,
                 char**   value )
{
    /* We expect ctcString to look like "key=value*...**" or "*".   */
    if ( *( obj->stringToParse ) == '*' )
    {
        return false; /* end of ctc string */
    }

    if ( *( obj->stringToParse ) == '\0' )
    {
        return false; /* also end of ctc string. we don't force the second "*" */
    }

    *key = obj->stringToParse;
    if ( !silc_pomp_extract_next_token( &obj->stringToParse, '=' ) )
    {
      SILC_ERROR(SILC_ERROR_PARSE_NO_KEY,
                 "Parsed String: %s\nKey: %s", obj->stringForErrorMsg, *key);
    }
    if ( strlen( *key ) == 0 )
    {
      SILC_ERROR(SILC_ERROR_PARSE_NO_KEY,
                 "Parsed String: %s\n", obj->stringForErrorMsg);
    }

    *value = obj->stringToParse;
    if ( !silc_pomp_extract_next_token( &obj->stringToParse, '*' ) )
    {
      SILC_ERROR(SILC_ERROR_PARSE_NO_VALUE,
                 "Parsed String: %s\nTried to get value from: %s\n",
                 obj->stringForErrorMsg, *value);
    }
    if ( strlen( *value ) == 0 )
    {
      SILC_ERROR(SILC_ERROR_PARSE_NO_VALUE,
                 "Parsed String: %s\n", obj->stringForErrorMsg);
    }
    return true;
}

static void
silc_pomp_assign_region_type( silc_pomp_parsing_data*    obj,
                              const char* value )
{
    /* Convert string to type */
    obj->region->regionType = silc_pomp_get_region_type_from_string( value );

    if ( obj->region->regionType ==  SILC_Pomp_NoType )
    {
      SILC_ERROR(SILC_ERROR_UNKNOWN_REGION_TYPE,
                 "Parsed String: %s\nRegion type: %s",
                 obj->stringForErrorMsg, value);
    }
}

static void
silc_pomp_assign_source_code_location( silc_pomp_parsing_data*  obj,
                          char**    filename,
                          unsigned* line1,
                          unsigned* line2,
                          char*     value )
{
    /* We assume that value looks like "foo.c:42:43" */
    char* token    = value;
    int   line1Tmp = -1;
    int   line2Tmp = -1;
    bool  continueExtraction;
    assert( *filename == 0 );

    if ( ( continueExtraction = silc_pomp_extract_next_token( &value, ':' ) ) )
    {
        *filename = malloc( strlen( token ) * sizeof( char ) + 1 );
        strcpy( *filename, token );
    }
    token = value;
    if ( continueExtraction &&
         ( continueExtraction = silc_pomp_extract_next_token( &value, ':' ) ) )
    {
        line1Tmp = atoi( token );
    }
    token = value;
    if ( continueExtraction && silc_pomp_extract_next_token( &value, '\0' ) )
    {
        line2Tmp = atoi( token );
    }

    if ( *filename != 0 && line1Tmp > -1 && line2Tmp > -1 )
    {
        *line1 = line1Tmp;
        *line2 = line2Tmp;
        if ( *line1 > *line2 )
        {
          SILC_ERROR(SILC_ERROR_INVALID_LINENO,
                     "Line1 (%d) > Line2 (%d)\nParsed String: %s\n",
		     *line1, *line2, obj->stringForErrorMsg);
        }
    }
    else
    {
      SILC_ERROR(SILC_ERROR_POMP_SCL_BROKEN,
                 "Parsed String: %s\n", obj->stringForErrorMsg);

    }
}

static void
silc_pomp_assign_unsigned( silc_pomp_parsing_data*    obj,
                unsigned*   anUnsigned,
                const char* value )
{
    int tmp = atoi( value );
    if ( tmp < 0 )
    {
      SILC_ERROR(SILC_ERROR_PARSE_INVALID_VALUE,
                 "Parsed String: %s\n%s: Must be >= 0",
                 obj->stringForErrorMsg, value);
    }
    *anUnsigned = tmp;
}

static void
silc_pomp_ignore_length_field( silc_pomp_parsing_data* obj )
{
    /* We expect ctcString to look like "42*key=value*...**"
     * The length field is redundant and we don't use it in our parsing
     * implementation. */
    while ( obj->stringToParse && isdigit( *obj->stringToParse ) )
    {
        ++( obj->stringToParse );
    }

    if ( !obj->stringToParse )
    {
      SILC_ERROR(SILC_ERROR_PARSE_UNEXPECTED_END,
                 "Parsed String: %s\n", obj->stringForErrorMsg);
    }
    if ( *obj->stringToParse != '*' )
    {
      SILC_ERROR(SILC_ERROR_PARSE_NO_SEPARATOR,
                 "Parsed string: %s\n", obj->stringForErrorMsg);
    }
    ++( obj->stringToParse );
    if ( !obj->stringToParse )
    {
      SILC_ERROR(SILC_ERROR_PARSE_UNEXPECTED_END,
                 "Parsed String: %s\n", obj->stringForErrorMsg);
    }
}

static void
silc_pomp_check_consistency( silc_pomp_parsing_data* obj )
{
    bool requiredAttributesFound;

    if ( obj->region->regionType == SILC_Pomp_NoType )
    {
      SILC_ERROR(SILC_ERROR_UNKNOWN_REGION_TYPE,
                 "Parsed String: %s\n", obj->stringForErrorMsg);
      return;
    }

    requiredAttributesFound = ( obj->region->startFileName
                                && obj->region->endFileName );
    if ( !requiredAttributesFound )
    {
      SILC_ERROR(SILC_ERROR_POMP_SCL_BROKEN,
                 "Parsed String: %s\n", obj->stringForErrorMsg);
      return;
    }


    if ( obj->region->regionType == SILC_Pomp_Sections
         && obj->region->numSections <= 0 )
    {
      SILC_ERROR(SILC_ERROR_POMP_INVALID_SECNUM,
                 "Parsed String: %s\nNumber of sections: %d",
                 obj->stringForErrorMsg, obj->region->numSections);
      return;
    }

    if ( obj->region->regionType == SILC_Pomp_UserRegion
         && obj->region->regionName == 0 )
    {
      SILC_ERROR(SILC_ERROR_POMP_NO_NAME,
                   "Parsed String: %s\n", obj->stringForErrorMsg);
        return;
    }
}

silc_pomp_register_region(SILC_Pomp_Region* region)
{
  char *name = 0;
  SILC_RegionType type_outer = SILC_REGION_UNKNOWN;
  SILC_RegionType type_inner = SILC_REGION_UNKNOWN;

    /* Assume that all regions from one file are registered in a row.
       Thus, remember the last file handle and reuse it if the next region stems
       from the same source file.                                                 */
    static char* last_file_name = 0;
    static SILC_SourceFileHandle last_file = SILC_INVALID_SOURCE_FILE;

    /* Evtl. register new source file */
    if ( (last_file == SILC_INVALID_SOURCE_FILE) ||
         (strcmp(last_file_name, region->startFileName) != 0) )
    {
         last_file_name = region->startFileName;
         last_file = SILC_DefineSourceFile(last_file_name);
    }

    /* Determine name.*/
    if (region->regionName == 0)
    {
      name = silc_pomp_region_type_map[region->regionType].regionTypeString;
    }
    else
    {
      name = region->regionName;
    }

    /* Determine type of inner and outer regions */
    type_outer = silc_pomp_region_type_map[region->regionType].outerRegionType;
    type_inner = silc_pomp_region_type_map[region->regionType].innerRegionType;

    /* Register regions */
    region->outerBlock = SILC_DefineRegion ( name,
                                             last_file,
                                             region->startLine1,
                                             region->endLine2,
                                             SILC_ADAPTER_POMP,
                                             type_outer);

    if (type_inner != SILC_REGION_UNKNOWN)
    {
      region->innerBlock = SILC_DefineRegion ( name,
                                               last_file,
                                               region->startLine2,
                                               region->endLine1,
                                               SILC_ADAPTER_POMP,
                                               type_inner);
    }
    else
    {
      region->innerBlock = region->outerBlock;
    }
}

/* **************************************************************************************
 *                                                                     Exported functions
 ***************************************************************************************/

void
SILC_Pomp_ParseInitString( const char        initString[],
                           SILC_Pomp_Region* region )
{
    SILC_ASSERT( region );
    silc_pomp_parsing_data data;
    char* key;
    char* value;

    /* Initizialize the data objects */
    silc_pomp_init_region( region );
    silc_pomp_init_parsing_data( &data, initString, region );

    /* Ignore the length entry in the init string */
    silc_pomp_ignore_length_field( &data );

    /* Process all data fields */
    while ( silc_pomp_get_key_value_pair( &data, &key, &value ) )
    {
        /* Identify key type */
        switch ( silc_pomp_get_token_from_string( key ) )
        {
            case SILC_POMP_TOKEN_TYPE_REGION_TYPE:
                silc_pomp_assign_region_type( &data, value );
                break;
            case SILC_POMP_TOKEN_TYPE_START_SOURCE_CODE_LOCATION:
                silc_pomp_assign_source_code_location( &data,
                                                       &region->startFileName,
                                                       &region->startLine1,
                                                       &region->startLine2,
                                                       value );
                break;
            case SILC_POMP_TOKEN_TYPE_END_SOURCE_CODE_LOCATION:
                silc_pomp_assign_source_code_location( &data,
                                                       &region->endFileName,
                                                       &region->endLine1,
                                                       &region->endLine2,
                                                       value );
                break;
            case SILC_POMP_TOKEN_TYPE_NUM_SECTIONS:
                silc_pomp_assign_unsigned( &data, &data.region->numSections, value );
                break;
            case SILC_POMP_TOKEN_TYPE_CRITICAL_NAME:
                silc_pomp_assign_string( &region->regionName, value );
                break;
            case SILC_POMP_TOKEN_TYPE_USER_REGION_NAME:
                silc_pomp_assign_string( &region->regionName, value );
                break;
            default:
	      SILC_ERROR(SILC_ERROR_PARSE_UNKNOWN_TOKEN, "%s\n", key);
        }
    }

    /* Check consistency */
    silc_pomp_check_consistency( &data );

    /* Register handles */
    silc_pomp_register_region( region );

    /* Clean up */
    silc_pomp_free_parsing_data( &data );
}
