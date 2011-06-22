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

#include <SCOREP_Pomp_RegionInfo.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Types.h>
#include <SCOREP_Filter.h>

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
    SCOREP_POMP_TOKEN_TYPE_NO_TOKEN
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
const size_t scorep_pomp_token_map_size = 6;

/** Contains the data for one region type */
typedef struct
{
    char*                  regionTypeString;
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
  { "atomic",            SCOREP_Pomp_Atomic              , SCOREP_REGION_OMP_ATOMIC,    SCOREP_REGION_UNKNOWN              },
  { "barrier",           SCOREP_Pomp_Barrier             , SCOREP_REGION_OMP_BARRIER,   SCOREP_REGION_OMP_BARRIER          },
  { "critical",          SCOREP_Pomp_Critical            , SCOREP_REGION_OMP_CRITICAL,  SCOREP_REGION_OMP_CRITICAL_SBLOCK  },
  { "do",                SCOREP_Pomp_Do                  , SCOREP_REGION_OMP_LOOP,      SCOREP_REGION_UNKNOWN              },
  { "flush",             SCOREP_Pomp_Flush               , SCOREP_REGION_OMP_FLUSH,     SCOREP_REGION_OMP_FLUSH            },
  { "for",               SCOREP_Pomp_For                 , SCOREP_REGION_OMP_LOOP,      SCOREP_REGION_UNKNOWN              },
  { "master",            SCOREP_Pomp_Master              , SCOREP_REGION_OMP_MASTER,    SCOREP_REGION_OMP_MASTER           },
  { "parallel",          SCOREP_Pomp_Parallel            , SCOREP_REGION_UNKNOWN,       SCOREP_REGION_UNKNOWN              },
  { "paralleldo",        SCOREP_Pomp_ParallelDo          , SCOREP_REGION_OMP_LOOP,      SCOREP_REGION_UNKNOWN              },
  { "parallelfor",       SCOREP_Pomp_ParallelFor         , SCOREP_REGION_OMP_LOOP,      SCOREP_REGION_UNKNOWN              },
  { "parallelsections",  SCOREP_Pomp_ParallelSections    , SCOREP_REGION_OMP_SECTIONS,  SCOREP_REGION_OMP_SECTION          },
  { "parallelworkshare", SCOREP_Pomp_ParallelWorkshare   , SCOREP_REGION_OMP_WORKSHARE, SCOREP_REGION_OMP_WORKSHARE        },
  { "region",            SCOREP_Pomp_UserRegion          , SCOREP_REGION_USER,          SCOREP_REGION_USER,                },
  { "sections",          SCOREP_Pomp_Sections            , SCOREP_REGION_OMP_SECTIONS,  SCOREP_REGION_OMP_SECTION          },
  { "single",            SCOREP_Pomp_Single              , SCOREP_REGION_OMP_SINGLE,    SCOREP_REGION_OMP_SINGLE_SBLOCK,   },
  { "workshare",         SCOREP_Pomp_Workshare           , SCOREP_REGION_OMP_WORKSHARE, SCOREP_REGION_OMP_WORKSHARE        }
};
/* *INDENT-ON* */

/** Number of entries in scorep_pomp_region_type_map */
const size_t scorep_pomp_region_type_map_size = 16;

/* **************************************************************************************
 *                                                                   Conversion functions
 ***************************************************************************************/

/** Comparison function for a string and @mapElem->regionTypeString.
    @param searchKey String containing the name of the region type.
    @param mapElem   Entry of scorep_pomp_region_type_map to whose name @a searchKey is
                     compared.
    @return Returns an integral value indicating the relationship between the strings:
            A zero value indicates that both strings are equal. A value greater than
            zero indicates that the first character that does not match has a greater
            value in @a searchKey than in mapElem->regionTypeString; And a value less
            than zero indicates the opposite.
 */
static int
scorep_pomp_region_type_map_compare( const void* searchKey,
                                     const void* mapElem )
{
    const char* const                  key  = ( const char* )searchKey;
    scorep_pomp_region_type_map_entry* elem = ( scorep_pomp_region_type_map_entry* )mapElem;

    return strcmp( key, elem->regionTypeString );
}


/** Comparison function for a string and @mapElem->tokenString.
    @param searchToken Pointer to a string.
    @param mapElem     Entry of scorep_pomp_token_map to whose tokenString member
                       @a searchToken is compared.
    @return Returns an integral value indicating the relationship between the strings:
            A zero value indicates that both strings are equal. A value greater than
            zero indicates that the first character that does not match has a greater
            value in @a searchToken than in mapElem->tokenString; And a value less
            than zero indicates the opposite.
 */
static int
scorep_pomp_token_map_compare( const void* searchToken,
                               const void* mapElem )
{
    const char* const            token = ( const char* )searchToken;
    scorep_pomp_token_map_entry* elem  = ( scorep_pomp_token_map_entry* )mapElem;

    return strcmp( token, elem->tokenString );
}

/** Converts the string representation of a token type to scorep_pomp_token_type.
    @param token  String representation of the token type.
    @returns If the token name is contained in @ref scorep_pomp_token_map it returns
             the particular scorep_pomp_token_type, else SCOREP_POMP_TOKEN_TYPE_NO_TOKEN
             is returned.
 */
static scorep_pomp_token_type
scorep_pomp_get_token_from_string( char* token )
{
    scorep_pomp_token_map_entry* mapElem = ( scorep_pomp_token_map_entry* )bsearch(
        token,
        &scorep_pomp_token_map,
        scorep_pomp_token_map_size,
        sizeof( scorep_pomp_token_map_entry ),
        scorep_pomp_token_map_compare );

    if ( mapElem )
    {
        return mapElem->token;
    }
    else
    {
        return SCOREP_POMP_TOKEN_TYPE_NO_TOKEN;
    }
}

/** Converts the string representation of a region type to SCOREP_Pomp_RegionType
    @param regionTypeString String representation of the region type.
    @returns If the token name is contained in @ref scorep_pomp_region_type_map it returns
             the particular SCOREP_Pomp_RegionType, else SCOREP_Pomp_NoType
             is returned.
 */
static SCOREP_Pomp_RegionType
scorep_pomp_get_region_type_from_string( const char* regionTypeString )
{
    scorep_pomp_region_type_map_entry* mapElem = ( scorep_pomp_region_type_map_entry* )
                                                 bsearch(
        regionTypeString,
        &scorep_pomp_region_type_map,
        scorep_pomp_region_type_map_size,
        sizeof( scorep_pomp_region_type_map_entry ),
        scorep_pomp_region_type_map_compare );

    if ( mapElem )
    {
        return mapElem->regionType;
    }
    else
    {
        return SCOREP_Pomp_NoType;
    }
}

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
    region->outerParallel = SCOREP_INVALID_REGION;
    region->innerParallel = SCOREP_INVALID_REGION;
    region->outerBlock    = SCOREP_INVALID_REGION;
    region->innerBlock    = SCOREP_INVALID_REGION;
    region->startFileName = 0;
    region->startLine1    = 0;
    region->startLine2    = 0;
    region->endFileName   = 0;
    region->endLine1      = 0;
    region->endLine2      = 0;
    region->regionName    = 0;
}

/** Initailizes a scorep_pomp_parsing_data instance.
    @param obj    The scorep_pomp_parsing_data instance that is initialized.
    @param string The region information string that is parsed.
    @param region The pointer to the SCOREP_Pomp_Region that will be filled during
                  the parsing process.
 */
static void
scorep_pomp_init_parsing_data( scorep_pomp_parsing_data* obj,
                               const char                string[],
                               SCOREP_Pomp_Region*       region )
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

/** Frees allocated memory for mempers of a scorep_pomp_parsing_data instance.
    @param obj The scorep_pomp_parsing_data instance whose members are freed.
 */
static void
scorep_pomp_free_parsing_data( scorep_pomp_parsing_data* obj )
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
scorep_pomp_assign_string( char**      aString,
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
scorep_pomp_extract_next_token( char**     string,
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
scorep_pomp_get_key_value_pair( scorep_pomp_parsing_data* obj,
                                char**                    key,
                                char**                    value )
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
    if ( !scorep_pomp_extract_next_token( &obj->stringToParse, '=' ) )
    {
        SCOREP_ERROR( SCOREP_ERROR_PARSE_NO_KEY,
                      "Parsed String: %s\nKey: %s", obj->stringForErrorMsg, *key );
    }
    if ( strlen( *key ) == 0 )
    {
        SCOREP_ERROR( SCOREP_ERROR_PARSE_NO_KEY,
                      "Parsed String: %s", obj->stringForErrorMsg );
    }

    *value = obj->stringToParse;
    if ( !scorep_pomp_extract_next_token( &obj->stringToParse, '*' ) )
    {
        SCOREP_ERROR( SCOREP_ERROR_PARSE_NO_VALUE,
                      "Parsed String: %s\nTried to get value from: %s",
                      obj->stringForErrorMsg, *value );
    }
    if ( strlen( *value ) == 0 )
    {
        SCOREP_ERROR( SCOREP_ERROR_PARSE_NO_VALUE,
                      "Parsed String: %s", obj->stringForErrorMsg );
    }
    return true;
}

/** Sets the region type in a scorep_pomp_parsing_data instance from a string
    representation.
    @param obj   The scorep_pomp_parsing_data where the region type is set.
    @param value The string representation of the region type.
 */
static void
scorep_pomp_assign_region_type( scorep_pomp_parsing_data* obj,
                                const char*               value )
{
    /* Convert string to type */
    obj->region->regionType = scorep_pomp_get_region_type_from_string( value );

    if ( obj->region->regionType ==  SCOREP_Pomp_NoType )
    {
        SCOREP_ERROR( SCOREP_ERROR_UNKNOWN_REGION_TYPE,
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
scorep_pomp_assign_source_code_location( scorep_pomp_parsing_data* obj,
                                         char**                    filename,
                                         int32_t*                  line1,
                                         int32_t*                  line2,
                                         char*                     value )
{
    char* token    = value;
    int   line1Tmp = -1;
    int   line2Tmp = -1;
    bool  continueExtraction;
    assert( *filename == 0 );

    if ( ( continueExtraction = scorep_pomp_extract_next_token( &value, ':' ) ) )
    {
        *filename = malloc( strlen( token ) * sizeof( char ) + 1 );
        strcpy( *filename, token );
    }
    token = value;
    if ( continueExtraction &&
         ( continueExtraction = scorep_pomp_extract_next_token( &value, ':' ) ) )
    {
        line1Tmp = atoi( token );
    }
    token = value;
    if ( continueExtraction && scorep_pomp_extract_next_token( &value, '\0' ) )
    {
        line2Tmp = atoi( token );
    }

    if ( *filename != 0 && line1Tmp > -1 && line2Tmp > -1 )
    {
        *line1 = line1Tmp;
        *line2 = line2Tmp;
        if ( *line1 > *line2 )
        {
            SCOREP_ERROR( SCOREP_ERROR_INVALID_LINENO,
                          "Line1 (%d) > Line2 (%d)\nParsed String: %s",
                          *line1, *line2, obj->stringForErrorMsg );
        }
    }
    else
    {
        SCOREP_ERROR( SCOREP_ERROR_POMP_SCL_BROKEN,
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
scorep_pomp_assign_unsigned( scorep_pomp_parsing_data* obj,
                             uint32_t*                 anUnsigned,
                             const char*               value )
{
    int tmp = atoi( value );
    if ( tmp < 0 )
    {
        SCOREP_ERROR( SCOREP_ERROR_PARSE_INVALID_VALUE,
                      "Parsed String: %s\n%s: Must be >= 0",
                      obj->stringForErrorMsg, value );
    }
    *anUnsigned = tmp;
}

/** Seeks forwards after the length field of the information string.
    @param obj   scorep_pomp_parsing_data instance containing the parsed string.
 */
static void
scorep_pomp_ignore_length_field( scorep_pomp_parsing_data* obj )
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
        SCOREP_ERROR( SCOREP_ERROR_PARSE_UNEXPECTED_END,
                      "Parsed String: %s", obj->stringForErrorMsg );
    }
    if ( *obj->stringToParse != '*' )
    {
        SCOREP_ERROR( SCOREP_ERROR_PARSE_NO_SEPARATOR,
                      "Parsed string: %s", obj->stringForErrorMsg );
    }
    ++( obj->stringToParse );
    if ( !obj->stringToParse )
    {
        SCOREP_ERROR( SCOREP_ERROR_PARSE_UNEXPECTED_END,
                      "Parsed String: %s", obj->stringForErrorMsg );
    }
}

/** Checks if the information retrieved from the parsed string is consistent. Otherwise,
    error messages are printed.
    @param obj scorep_pomp_parsing_data instance containing the parsed string.
 */
static void
scorep_pomp_check_consistency( scorep_pomp_parsing_data* obj )
{
    bool requiredAttributesFound;

    if ( obj->region->regionType == SCOREP_Pomp_NoType )
    {
        SCOREP_ERROR( SCOREP_ERROR_UNKNOWN_REGION_TYPE,
                      "Parsed String: %s", obj->stringForErrorMsg );
        return;
    }

    requiredAttributesFound = ( obj->region->startFileName
                                && obj->region->endFileName );
    if ( !requiredAttributesFound )
    {
        SCOREP_ERROR( SCOREP_ERROR_POMP_SCL_BROKEN,
                      "Parsed String: %s", obj->stringForErrorMsg );
        return;
    }

    if ( obj->region->regionType == SCOREP_Pomp_UserRegion
         && obj->region->regionName == 0 )
    {
        SCOREP_ERROR( SCOREP_ERROR_POMP_NO_NAME,
                      "Parsed String: %s", obj->stringForErrorMsg );
        return;
    }
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
    char*             name        = 0;
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

    /* Determine name */
    if ( region->regionName == 0 )
    {
        name = scorep_pomp_region_type_map[ region->regionType ].regionTypeString;
    }
    else
    {
        name = region->regionName;
    }

    if ( region->regionType != SCOREP_Pomp_UserRegion )
    {
        int length = strlen( name ) + 7;
        region_name = ( char* )malloc( length );
        strcpy( region_name, "!$omp " );
        strcpy( &region_name[ 6 ], name );
        region_name[ length - 1 ] = '\0';
    }
    else
    {
        region_name = SCOREP_CStr_dup( name );
    }

    /* User regions can be filtered */
    if ( ( region->regionType == SCOREP_Pomp_UserRegion ) &&
         SCOREP_Filter_Match( region->startFileName, region_name, false ) )
    {
        region->innerBlock = SCOREP_INVALID_REGION;
        free( region_name );
        return;
    }

    /* Register parallel regions */
    if ( ( region->regionType >= SCOREP_Pomp_Parallel ) &&
         ( region->regionType <= SCOREP_Pomp_ParallelWorkshare ) )
    {
        region->outerParallel = SCOREP_DefineRegion( region_name,
                                                     last_file,
                                                     region->startLine1,
                                                     region->endLine2,
                                                     SCOREP_ADAPTER_POMP,
                                                     SCOREP_REGION_OMP_PARALLEL );

        if ( region->regionType == SCOREP_Pomp_Parallel )
        {
            start = region->startLine1;
            end   = region->endLine2;
        }
        else
        {
            start = region->startLine2;
            end   = region->endLine1;
        }
        region->innerParallel = SCOREP_DefineRegion( region_name,
                                                     last_file,
                                                     start,
                                                     end,
                                                     SCOREP_ADAPTER_POMP,
                                                     SCOREP_REGION_OMP_PARALLEL );
    }

    /* Determine type of inner and outer regions */
    type_outer = scorep_pomp_region_type_map[ region->regionType ].outerRegionType;
    type_inner = scorep_pomp_region_type_map[ region->regionType ].innerRegionType;

    /* Register other regions */
    if ( type_outer != SCOREP_REGION_UNKNOWN )
    {
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
    }

    if ( type_inner != SCOREP_REGION_UNKNOWN )
    {
        region->innerBlock = SCOREP_DefineRegion( region_name,
                                                  last_file,
                                                  region->startLine2,
                                                  region->endLine1,
                                                  SCOREP_ADAPTER_POMP,
                                                  type_inner );
    }

    free( region_name );

#ifdef _OPENMP
    /* Register locks for critical regions */
    if ( region->regionType == SCOREP_Pomp_Critical )
    {
        region->lock = scorep_pomp_get_lock_handle( region->regionName );
        if ( region->lock == SCOREP_POMP_INVALID_LOCK )
        {
            region->lock = scorep_pomp_lock_init( region->regionName );
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
    scorep_pomp_parsing_data data;
    char*                    key;
    char*                    value;

    /* Initizialize the data objects */
    scorep_pomp_init_region( region );
    scorep_pomp_init_parsing_data( &data, initString, region );

    /* Ignore the length entry in the init string */
    scorep_pomp_ignore_length_field( &data );

    /* Process all data fields */
    while ( scorep_pomp_get_key_value_pair( &data, &key, &value ) )
    {
        /* Identify key type */
        switch ( scorep_pomp_get_token_from_string( key ) )
        {
            case SCOREP_POMP_TOKEN_TYPE_REGION_TYPE:
                scorep_pomp_assign_region_type( &data, value );
                break;
            case SCOREP_POMP_TOKEN_TYPE_START_SOURCE_CODE_LOCATION:
                scorep_pomp_assign_source_code_location( &data,
                                                         &region->startFileName,
                                                         &region->startLine1,
                                                         &region->startLine2,
                                                         value );
                break;
            case SCOREP_POMP_TOKEN_TYPE_END_SOURCE_CODE_LOCATION:
                scorep_pomp_assign_source_code_location( &data,
                                                         &region->endFileName,
                                                         &region->endLine1,
                                                         &region->endLine2,
                                                         value );
                break;
            case SCOREP_POMP_TOKEN_TYPE_NUM_SECTIONS:
                scorep_pomp_assign_unsigned( &data, &data.region->numSections, value );
                break;
            case SCOREP_POMP_TOKEN_TYPE_CRITICAL_NAME:
                scorep_pomp_assign_string( &region->regionName, value );
                break;
            case SCOREP_POMP_TOKEN_TYPE_USER_REGION_NAME:
                scorep_pomp_assign_string( &region->regionName, value );
                break;
            default:
                SCOREP_ERROR( SCOREP_ERROR_PARSE_UNKNOWN_TOKEN, "%s\n", key );
        }
    }

    /* Check consistency */
    scorep_pomp_check_consistency( &data );

    /* Register handles */
    scorep_pomp_register_region( region );

    /* Clean up */
    scorep_pomp_free_parsing_data( &data );
}

/** @} */
