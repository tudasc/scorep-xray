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

/** @ingroup POMP
    @{
 */

/* **************************************************************************************
                                                                             Declarations
****************************************************************************************/

/** Contains data for parsing a region info string */
typedef struct /* silc_pomp_parsing_data */
{
    SILC_Pomp_Region* region;
    char*             stringToParse;      /* will be modified */
    char*             stringMemory;
    char*             stringForErrorMsg;  /* copy of the string used for error messages */
} silc_pomp_parsing_data;

/** Possible token types which can appear in a region info string */
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

/** Contains a token and its type from a region info string */
typedef struct
{
    char*                tokenString;
    silc_pomp_token_type token;
} silc_pomp_token_map_entry;

/** Maps token names to silc_pomp_token_type */
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

/** Number of entries in @ref silc_pomp_token_map */
const size_t silc_pomp_token_map_size = 6;

/** Contains the data for one region type */
typedef struct
{
    char*                regionTypeString;
    SILC_Pomp_RegionType regionType;
    SILC_RegionType      outerRegionType;
    SILC_RegionType      innerRegionType;
} silc_pomp_region_type_map_entry;

/* *INDENT-OFF* */
/** Maps region name in the information string to SILC_Pomp_RegionType and
    SILC_RegionType of the inner and outer region.
*/
static const silc_pomp_region_type_map_entry silc_pomp_region_type_map[] =
{
    /* Entries must be sorted to be used in binary search. */
    /* If you add/remove items, silc_pomp_region_type_map_size. */
    /* Entries must be in same order like SILC_Pomp_RegionType to allow lookup. */
  { "atomic",            SILC_Pomp_Atomic              , SILC_REGION_OMP_ATOMIC,    SILC_REGION_UNKNOWN              },
  { "barrier",           SILC_Pomp_Barrier             , SILC_REGION_OMP_BARRIER,   SILC_REGION_OMP_BARRIER          },
  { "critical",          SILC_Pomp_Critical            , SILC_REGION_OMP_CRITICAL,  SILC_REGION_OMP_CRITICAL         },
  { "do",                SILC_Pomp_Do                  , SILC_REGION_OMP_LOOP,      SILC_REGION_UNKNOWN              },
  { "flush",             SILC_Pomp_Flush               , SILC_REGION_OMP_FLUSH,     SILC_REGION_OMP_FLUSH            },
  { "for",               SILC_Pomp_For                 , SILC_REGION_OMP_LOOP,      SILC_REGION_UNKNOWN              },
  { "master",            SILC_Pomp_Master              , SILC_REGION_OMP_MASTER,    SILC_REGION_OMP_MASTER           },
  { "parallel",          SILC_Pomp_Parallel            , SILC_REGION_UNKNOWN,       SILC_REGION_UNKNOWN              },
  { "paralleldo",        SILC_Pomp_ParallelDo          , SILC_REGION_OMP_LOOP,      SILC_REGION_UNKNOWN              },
  { "parallelfor",       SILC_Pomp_ParallelFor         , SILC_REGION_OMP_LOOP,      SILC_REGION_UNKNOWN              },
  { "parallelsections",  SILC_Pomp_ParallelSections    , SILC_REGION_OMP_SECTIONS,  SILC_REGION_OMP_SECTION          },
  { "parallelworkshare", SILC_Pomp_ParallelWorkshare   , SILC_REGION_OMP_WORKSHARE, SILC_REGION_OMP_WORKSHARE        },
  { "region",            SILC_Pomp_UserRegion          , SILC_REGION_USER,          SILC_REGION_USER,                },
  { "sections",          SILC_Pomp_Sections            , SILC_REGION_OMP_SECTIONS,  SILC_REGION_OMP_SECTION          },
  { "single",            SILC_Pomp_Single              , SILC_REGION_OMP_SINGLE,    SILC_REGION_OMP_SINGLE,          },
  { "workshare",         SILC_Pomp_Workshare           , SILC_REGION_OMP_WORKSHARE, SILC_REGION_OMP_WORKSHARE        }
};
/* *INDENT-ON* */

/** Number of entries in silc_pomp_region_type_map */
const size_t silc_pomp_region_type_map_size = 16;

/* **************************************************************************************
 *                                                                   Conversion functions
 ***************************************************************************************/

/** Comparison function for a string and @mapElem->regionTypeString.
    @param searchKey String containing the name of the region type.
    @param mapElem   Entry of silc_pomp_region_type_map to whose name @a searchKey is
                     compared.
    @return Returns an integral value indicating the relationship between the strings:
            A zero value indicates that both strings are equal. A value greater than
            zero indicates that the first character that does not match has a greater
            value in @a searchKey than in mapElem->regionTypeString; And a value less
            than zero indicates the opposite.
 */
static int
silc_pomp_region_type_map_compare( const void* searchKey,
                                   const void* mapElem )
{
    const char* const                key  = ( const char* )searchKey;
    silc_pomp_region_type_map_entry* elem = ( silc_pomp_region_type_map_entry* )mapElem;

    return strcmp( key, elem->regionTypeString );
}


/** Comparison function for a string and @mapElem->tokenString.
    @param searchToken Pointer to a string.
    @param mapElem     Entry of silc_pomp_token_map to whose tokenString member
                       @a searchToken is compared.
    @return Returns an integral value indicating the relationship between the strings:
            A zero value indicates that both strings are equal. A value greater than
            zero indicates that the first character that does not match has a greater
            value in @a searchToken than in mapElem->tokenString; And a value less
            than zero indicates the opposite.
 */
static int
silc_pomp_token_map_compare( const void* searchToken,
                             const void* mapElem )
{
    const char* const          token = ( const char* )searchToken;
    silc_pomp_token_map_entry* elem  = ( silc_pomp_token_map_entry* )mapElem;

    return strcmp( token, elem->tokenString );
}

/** Converts the string representation of a token type to silc_pomp_token_type.
    @param token  String representation of the token type.
    @returns If the token name is contained in @ref silc_pomp_token_map it returns
             the particular silc_pomp_token_type, else SILC_POMP_TOKEN_TYPE_NO_TOKEN
             is returned.
 */
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

/** Converts the string representation of a region type to SILC_Pomp_RegionType
    @param regionTypeString String representation of the region type.
    @returns If the token name is contained in @ref silc_pomp_region_type_map it returns
             the particular SILC_Pomp_RegionType, else SILC_Pomp_NoType
             is returned.
 */
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

/** Initializes a SILC_Pomp_Region instance @region  with NULL values.
    @param region the SILC_Pomp_Region instance which get initialized.
 */
static void
silc_pomp_init_region( SILC_Pomp_Region* region )
{
    region->regionType    = SILC_Pomp_NoType;
    region->name          = 0;
    region->numSections   = 0;
    region->outerParallel = SILC_INVALID_REGION;
    region->innerParallel = SILC_INVALID_REGION;
    region->outerBlock    = SILC_INVALID_REGION;
    region->innerBlock    = SILC_INVALID_REGION;
    region->startFileName = 0;
    region->startLine1    = 0;
    region->startLine2    = 0;
    region->endFileName   = 0;
    region->endLine1      = 0;
    region->endLine2      = 0;
    region->regionName    = 0;
}

/** Initailizes a silc_pomp_parsing_data instance.
    @param obj    The silc_pomp_parsing_data instance that is initialized.
    @param string The region information string that is parsed.
    @param region The pointer to the SILC_Pomp_Region that will be filled during
                  the parsing process.
 */
static void
silc_pomp_init_parsing_data( silc_pomp_parsing_data* obj,
                             const char              string[],
                             SILC_Pomp_Region*       region )
{
    /* Size of the init string */
    const size_t nBytes = strlen( string ) * sizeof( char ) + 1;

    /* Set fields */
    obj->region            = region;
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

/** Frees allocated memory for mempers of a silc_pomp_parsing_data instance.
    @param obj The silc_pomp_parsing_data instance whose members are freed.
 */
static void
silc_pomp_free_parsing_data( silc_pomp_parsing_data* obj )
{
    /* Free memory */
    free( obj->stringMemory );
    free( obj->stringForErrorMsg );

    /* Set to 0 */
    obj->stringMemory      = 0;
    obj->stringForErrorMsg = 0;
    obj->stringToParse     = 0;
}

/* **************************************************************************************
 *                                                             internal parsing functions
 ***************************************************************************************/

/** Allocates memory for @a aString and copy the content of @a value into it.
    @param aString Pointer to a string for which the new memory is allocated and into
                   which the string is copied.
    @param value   String which is copied to @a aString.
 */
static void
silc_pomp_assign_string( char**      aString,
                         const char* value )
{
    *aString = malloc( strlen( value ) * sizeof( char ) + 1 );
    strcpy( *aString, value );
}

/** Extracts a new token from a string. Replaces the first appearance of
    @a tokenDelimiter in @a string by zero and places the pointer of string on the
    first character after the zero.
    @param string         Pointer to the string which is parsed.
    @param tokenDelimiter Character which separates two tokens.
    @retuns true if a tokenDelimiter was found, else false.
 */
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

/** Extracts the next key-value pair from a the parsing data.
    @param obj    The parsing data, containing the string to be parsed.
    @param key    Returns the string representation of the next key.
    @param value  Returns the string representation of the value.
    @return false if the end of the string is reached, else true.
 */
static bool
silc_pomp_get_key_value_pair( silc_pomp_parsing_data* obj,
                              char**                  key,
                              char**                  value )
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
        SILC_ERROR( SILC_ERROR_PARSE_NO_KEY,
                    "Parsed String: %s\nKey: %s", obj->stringForErrorMsg, *key );
    }
    if ( strlen( *key ) == 0 )
    {
        SILC_ERROR( SILC_ERROR_PARSE_NO_KEY,
                    "Parsed String: %s", obj->stringForErrorMsg );
    }

    *value = obj->stringToParse;
    if ( !silc_pomp_extract_next_token( &obj->stringToParse, '*' ) )
    {
        SILC_ERROR( SILC_ERROR_PARSE_NO_VALUE,
                    "Parsed String: %s\nTried to get value from: %s",
                    obj->stringForErrorMsg, *value );
    }
    if ( strlen( *value ) == 0 )
    {
        SILC_ERROR( SILC_ERROR_PARSE_NO_VALUE,
                    "Parsed String: %s", obj->stringForErrorMsg );
    }
    return true;
}

/** Sets the region type in a silc_pomp_parsing_data instance from a string
    representation.
    @param obj   The silc_pomp_parsing_data where the region type is set.
    @param value The string representation of the region type.
 */
static void
silc_pomp_assign_region_type( silc_pomp_parsing_data* obj,
                              const char*             value )
{
    /* Convert string to type */
    obj->region->regionType = silc_pomp_get_region_type_from_string( value );

    if ( obj->region->regionType ==  SILC_Pomp_NoType )
    {
        SILC_ERROR( SILC_ERROR_UNKNOWN_REGION_TYPE,
                    "Parsed String: %s\nRegion type: %s",
                    obj->stringForErrorMsg, value );
    }
}

/** Parses the string representation of a source code location and sets the filename
    and linenumbers.
    @param obj      Parsing object containing tha full parsed string. Used only for
                    error messages.
    @param filename Returns the file name of the source code location.
    @param line1    Returns the starting file number.
    @param line2    Returns the ending file number.
    @param value    Contains the string representation of the source code location.
                    It is expeced that the string looks like  "foo.c:42:43".
 */
static void
silc_pomp_assign_source_code_location( silc_pomp_parsing_data* obj,
                                       char**                  filename,
                                       int32_t*                line1,
                                       int32_t*                line2,
                                       char*                   value )
{
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
            SILC_ERROR( SILC_ERROR_INVALID_LINENO,
                        "Line1 (%d) > Line2 (%d)\nParsed String: %s",
                        *line1, *line2, obj->stringForErrorMsg );
        }
    }
    else
    {
        SILC_ERROR( SILC_ERROR_POMP_SCL_BROKEN,
                    "Parsed String: %s\n", obj->stringForErrorMsg );
    }
}

/** Assigns the value of an unsigne integer from its string representation.
    @param obj        Parsing object containing tha full parsed string. Used only for
                      error messages.
    @param anUnsigned Pointer to an unsigned to which the value is written.
    @param value      String representation of an unsinged.
 */
static void
silc_pomp_assign_unsigned( silc_pomp_parsing_data* obj,
                           uint32_t*               anUnsigned,
                           const char*             value )
{
    int tmp = atoi( value );
    if ( tmp < 0 )
    {
        SILC_ERROR( SILC_ERROR_PARSE_INVALID_VALUE,
                    "Parsed String: %s\n%s: Must be >= 0",
                    obj->stringForErrorMsg, value );
    }
    *anUnsigned = tmp;
}

/** Seeks forwards after the length field of the information string.
    @param obj   silc_pomp_parsing_data instance containing the parsed string.
 */
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
        SILC_ERROR( SILC_ERROR_PARSE_UNEXPECTED_END,
                    "Parsed String: %s", obj->stringForErrorMsg );
    }
    if ( *obj->stringToParse != '*' )
    {
        SILC_ERROR( SILC_ERROR_PARSE_NO_SEPARATOR,
                    "Parsed string: %s", obj->stringForErrorMsg );
    }
    ++( obj->stringToParse );
    if ( !obj->stringToParse )
    {
        SILC_ERROR( SILC_ERROR_PARSE_UNEXPECTED_END,
                    "Parsed String: %s", obj->stringForErrorMsg );
    }
}

/** Checks if the information retrieved from the parsed string is consistent. Otherwise,
    error messages are printed.
    @param obj silc_pomp_parsing_data instance containing the parsed string.
 */
static void
silc_pomp_check_consistency( silc_pomp_parsing_data* obj )
{
    bool requiredAttributesFound;

    if ( obj->region->regionType == SILC_Pomp_NoType )
    {
        SILC_ERROR( SILC_ERROR_UNKNOWN_REGION_TYPE,
                    "Parsed String: %s", obj->stringForErrorMsg );
        return;
    }

    requiredAttributesFound = ( obj->region->startFileName
                                && obj->region->endFileName );
    if ( !requiredAttributesFound )
    {
        SILC_ERROR( SILC_ERROR_POMP_SCL_BROKEN,
                    "Parsed String: %s", obj->stringForErrorMsg );
        return;
    }

    if ( obj->region->regionType == SILC_Pomp_UserRegion
         && obj->region->regionName == 0 )
    {
        SILC_ERROR( SILC_ERROR_POMP_NO_NAME,
                    "Parsed String: %s", obj->stringForErrorMsg );
        return;
    }
}

/** Registers the pomp regions to silc and sets the SILC region handle fields in
    a SILC_Pomp_Region instance. All necessary data must be already contained in the
    SILC_Pomp_Region instance.
    @param region  The region infromation struct for a POMP region which gets registered
                   to SILC. The region handle fields of it are set in this function.
 */
void
silc_pomp_register_region( SILC_Pomp_Region* region )
{
    char*           name       = 0;
    SILC_RegionType type_outer = SILC_REGION_UNKNOWN;
    SILC_RegionType type_inner = SILC_REGION_UNKNOWN;
    int32_t         start, end;

    /* Assume that all regions from one file are registered in a row.
       Thus, remember the last file handle and reuse it if the next region stems
       from the same source file.                                                 */
    static char*                 last_file_name = 0;
    static SILC_SourceFileHandle last_file      = SILC_INVALID_SOURCE_FILE;

    /* Evtl. register new source file */
    if ( ( last_file == SILC_INVALID_SOURCE_FILE ) ||
         ( strcmp( last_file_name, region->startFileName ) != 0 ) )
    {
        last_file_name = region->startFileName;
        last_file      = SILC_DefineSourceFile( last_file_name );
    }

    /* Determine name */
    if ( region->regionName == 0 )
    {
        name = silc_pomp_region_type_map[ region->regionType ].regionTypeString;
    }
    else
    {
        name = region->regionName;
    }

    /* Register parallel regions */
    if ( ( region->regionType >= SILC_Pomp_Parallel ) &&
         ( region->regionType <= SILC_Pomp_ParallelWorkshare ) )
    {
        region->outerParallel = SILC_DefineRegion( name,
                                                   last_file,
                                                   region->startLine1,
                                                   region->endLine2,
                                                   SILC_ADAPTER_POMP,
                                                   SILC_REGION_OMP_PARALLEL );

        if ( region->regionType == SILC_Pomp_Parallel )
        {
            start = region->startLine1;
            end   = region->endLine2;
        }
        else
        {
            start = region->startLine2;
            end   = region->endLine1;
        }
        region->innerParallel = SILC_DefineRegion( name,
                                                   last_file,
                                                   start,
                                                   end,
                                                   SILC_ADAPTER_POMP,
                                                   SILC_REGION_OMP_PARALLEL );
    }

    /* Determine type of inner and outer regions */
    type_outer = silc_pomp_region_type_map[ region->regionType ].outerRegionType;
    type_inner = silc_pomp_region_type_map[ region->regionType ].innerRegionType;

    /* Register other regions */
    if ( type_outer != SILC_REGION_UNKNOWN )
    {
        if ( ( region->regionType >= SILC_Pomp_ParallelDo ) &&
             ( region->regionType <= SILC_Pomp_ParallelWorkshare ) )
        {
            start = region->startLine2;
            end   = region->endLine1;
        }
        else
        {
            start = region->startLine1;
            end   = region->endLine2;
        }

        region->outerBlock = SILC_DefineRegion( name,
                                                last_file,
                                                start,
                                                end,
                                                SILC_ADAPTER_POMP,
                                                type_outer );
    }

    if ( type_inner != SILC_REGION_UNKNOWN )
    {
        region->innerBlock = SILC_DefineRegion( name,
                                                last_file,
                                                region->startLine2,
                                                region->endLine1,
                                                SILC_ADAPTER_POMP,
                                                type_inner );
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
    char*                  key;
    char*                  value;

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
                SILC_ERROR( SILC_ERROR_PARSE_UNKNOWN_TOKEN, "%s\n", key );
        }
    }

    /* Check consistency */
    silc_pomp_check_consistency( &data );

    /* Register handles */
    silc_pomp_register_region( region );

    /* Clean up */
    silc_pomp_free_parsing_data( &data );
}

/** @} */
