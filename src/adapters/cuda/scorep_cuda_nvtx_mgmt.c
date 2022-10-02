/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 */

#include <config.h>

#include "scorep_cuda_nvtx_mgmt.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_FastHashtab.h>

#include <UTILS_Error.h>

#include <jenkins_hash.h>

#include <stdio.h>
#include <wchar.h>

#include "scorep_cupti.h"

/*************** Types ********************************************************/

/* We provide an implementation for NVTX, thus we also need to declare the
   opaque types used in the NVTX API */

/* NVTX API: typedef struct nvtxStringHandle* nvtxStringHandle_t; */
struct nvtxStringHandle
{
    SCOREP_StringHandle str;
};

/* NVTX API: typedef struct nvtxDomainHandle* nvtxDomainHandle_t; */
struct nvtxDomainHandle
{
    nvtxStringHandle_t name;
};

/* NVTX API: typedef struct nvtxResourceHandle* nvtxResourceHandle_t; */
struct nvtxResourceHandle
{
    nvtxStringHandle_t name;
};

/*************** Variables ****************************************************/

static nvtxDomainHandle_t default_domain = NULL;

static SCOREP_AttributeHandle nvtx_attribute_uint32;
static SCOREP_AttributeHandle nvtx_attribute_uint64;
static SCOREP_AttributeHandle nvtx_attribute_int32;
static SCOREP_AttributeHandle nvtx_attribute_int64;
static SCOREP_AttributeHandle nvtx_attribute_float;
static SCOREP_AttributeHandle nvtx_attribute_double;

static SCOREP_ParameterHandle nvtx_parameter_category;

/*************** Widestring table *********************************************/

typedef struct
{
    uint32_t       hash_value;
    const wchar_t* wide_string;
} widestring_table_key_t;
typedef SCOREP_StringHandle widestring_table_value_t;

#define WIDESTRING_TABLE_HASH_EXPONENT 8

static inline uint32_t
widestring_table_bucket_idx( widestring_table_key_t key )
{
    return key.hash_value & hashmask( WIDESTRING_TABLE_HASH_EXPONENT );
}

static inline bool
widestring_table_equals( widestring_table_key_t key1,
                         widestring_table_key_t key2 )
{
    return key1.hash_value == key2.hash_value && wcscmp( key1.wide_string, key2.wide_string ) == 0;
}

static inline void*
widestring_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
widestring_table_free_chunk( void* chunk )
{
}

static void
widestring_to_string( size_t stringLength,
                      char*  string,
                      void*  arg )
{
    const wchar_t* wide            = arg;
    size_t         bytes_converted = wcstombs( string, wide, stringLength + 1 );
    // returns -1 iff an invalid wide char was encountered
    UTILS_BUG_ON( bytes_converted == ( size_t )( -1 ), "Invalid wide char conversion." );
}

static inline widestring_table_value_t
widestring_table_value_ctor( widestring_table_key_t* key,
                             void*                   ctorData )
{
    size_t              bytes_needed = wcstombs( NULL, key->wide_string, 0 );
    SCOREP_StringHandle new_string   = SCOREP_Definitions_NewStringGenerator( bytes_needed,
                                                                              widestring_to_string,
                                                                              ( void* )key->wide_string );

    size_t wide_size       = *( size_t* )ctorData;
    void*  copy_widestring = SCOREP_Memory_AllocForMisc( wide_size );
    memcpy( copy_widestring, key->wide_string, wide_size );
    key->wide_string = copy_widestring;

    return new_string;
}

/* nPairsPerChunk: 16+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( widestring_table,
                             6,
                             hashsize( WIDESTRING_TABLE_HASH_EXPONENT ) );

#undef WIDESTRING_TABLE_HASH_EXPONENT

/*************** Domain specific region table *********************************/

typedef struct
{
    uint32_t           hash_value;
    nvtxDomainHandle_t domain;
    const char*        region_name;
} region_table_key_t;
typedef SCOREP_RegionHandle region_table_value_t;

#define REGION_TABLE_HASH_EXPONENT 8

static inline uint32_t
region_table_bucket_idx( region_table_key_t key )
{
    return key.hash_value & hashmask( REGION_TABLE_HASH_EXPONENT );
}

static inline bool
region_table_equals( region_table_key_t key1,
                     region_table_key_t key2 )
{
    return key1.hash_value == key2.hash_value
           && key1.domain == key2.domain
           && strcmp( key1.region_name, key2.region_name ) == 0;
}

static inline void*
region_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
region_table_free_chunk( void* chunk )
{
}

static inline region_table_value_t
region_table_value_ctor( region_table_key_t* key,
                         void*               ctorData )
{
    SCOREP_RegionHandle new_region =
        SCOREP_Definitions_NewRegion( key->region_name,
                                      NULL,
                                      SCOREP_INVALID_SOURCE_FILE,
                                      0, 0,
                                      SCOREP_PARADIGM_CUDA,
                                      SCOREP_REGION_USER );

    SCOREP_RegionHandle_SetGroup( new_region,
                                  SCOREP_StringHandle_Get( key->domain->name->str ) );

    key->region_name = SCOREP_RegionHandle_GetName( new_region );

    return new_region;
}

/* nPairsPerChunk: 24+4 bytes per pair, 8 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( region_table,
                             4,
                             hashsize( REGION_TABLE_HASH_EXPONENT ) );

#undef REGION_TABLE_HASH_EXPONENT

/*************** Domain specific categories table *********************************/

typedef struct
{
    nvtxDomainHandle_t domain;
    uint32_t           category;
} category_table_key_t;
typedef SCOREP_StringHandle category_table_value_t;

#define CATEGORY_TABLE_HASH_EXPONENT 4

static inline uint32_t
category_table_bucket_idx( category_table_key_t key )
{
    uint32_t hash_value = jenkins_hash( &key, sizeof( key ), 0 );
    return hash_value & hashmask( CATEGORY_TABLE_HASH_EXPONENT );
}

static inline bool
category_table_equals( category_table_key_t key1,
                       category_table_key_t key2 )
{
    return key1.domain == key2.domain && key1.category == key2.category;
}

static inline void*
category_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
category_table_free_chunk( void* chunk )
{
}

static inline category_table_value_t
category_table_value_ctor( category_table_key_t* key,
                           void*                 ctorData )
{
    return SCOREP_Definitions_NewString( ctorData );
}

/* nPairsPerChunk: 16+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( category_table,
                             6,
                             hashsize( CATEGORY_TABLE_HASH_EXPONENT ) );

#undef CATEGORY_TABLE_HASH_EXPONENT

/*************** Functions ****************************************************/

void
scorep_cuda_nvtx_init( void )
{
    default_domain = scorep_cuda_nvtx_create_domain( NULL );

    nvtx_attribute_uint32 = SCOREP_Definitions_NewAttribute(
        "NVTX Payload UNSIGNED_INT32", "",
        SCOREP_ATTRIBUTE_TYPE_UINT32 );
    nvtx_attribute_uint64 = SCOREP_Definitions_NewAttribute(
        "NVTX Payload UNSIGNED_INT64", "",
        SCOREP_ATTRIBUTE_TYPE_UINT64 );

    nvtx_attribute_int32 = SCOREP_Definitions_NewAttribute(
        "NVTX Payload INT32", "",
        SCOREP_ATTRIBUTE_TYPE_INT32 );
    nvtx_attribute_int64 = SCOREP_Definitions_NewAttribute(
        "NVTX Payload INT64", "",
        SCOREP_ATTRIBUTE_TYPE_INT64 );

    nvtx_attribute_float = SCOREP_Definitions_NewAttribute(
        "NVTX Payload FLOAT", "",
        SCOREP_ATTRIBUTE_TYPE_FLOAT );
    nvtx_attribute_double = SCOREP_Definitions_NewAttribute(
        "NVTX Payload DOUBLE", "",
        SCOREP_ATTRIBUTE_TYPE_DOUBLE );

    nvtx_parameter_category = SCOREP_Definitions_NewParameter(
        "NVTX Category", SCOREP_PARAMETER_STRING );
}

const char*
scorep_cuda_nvtx_unicode_to_ascii( const wchar_t* wide )
{
    size_t                 bytes = ( wcslen( wide ) + 1 ) * sizeof( wchar_t );
    widestring_table_key_t key   = {
        .hash_value  = jenkins_hash( wide, bytes, 0 ),
        .wide_string = wide
    };

    SCOREP_StringHandle string = SCOREP_INVALID_STRING;
    widestring_table_get_and_insert( key, &bytes, &string );

    return SCOREP_StringHandle_Get( string );
}

static nvtxStringHandle_t
create_nvtx_string_handle( SCOREP_StringHandle string )
{
    nvtxStringHandle_t result = SCOREP_Memory_AllocForMisc( sizeof( struct nvtxStringHandle ) );
    result->str = string;
    return result;
}

static void
domain_name_generator( size_t stringLength,
                       char*  string,
                       void*  arg )
{
    snprintf( string, stringLength, "NVTX Domain '%s'", ( const char* )arg );
}

nvtxDomainHandle_t
scorep_cuda_nvtx_create_domain( const char* name )
{
    nvtxDomainHandle_t new_domain =
        SCOREP_Memory_AllocForMisc( sizeof( struct nvtxDomainHandle ) );

    if ( name )
    {
        new_domain->name = create_nvtx_string_handle(
            SCOREP_Definitions_NewStringGenerator( strlen( "NVTX Domain ''" ) + strlen( name ),
                                                   domain_name_generator, ( void* )name ) );
    }
    else
    {
        new_domain->name = scorep_cuda_nvtx_create_string( "NVTX" );
    }

    return new_domain;
}

nvtxStringHandle_t
scorep_cuda_nvtx_create_string( const char* string )
{
    return create_nvtx_string_handle( SCOREP_Definitions_NewString( string ) );
}

const char*
scorep_cuda_nvtx_get_name_from_attributes( const nvtxEventAttributes_t* eventAttrib )
{
    UTILS_ASSERT( eventAttrib );
    switch ( eventAttrib->messageType )
    {
        case NVTX_MESSAGE_TYPE_ASCII:
            return eventAttrib->message.ascii;
        case NVTX_MESSAGE_TYPE_UNICODE:
            return scorep_cuda_nvtx_unicode_to_ascii( eventAttrib->message.unicode );
        case NVTX_MESSAGE_TYPE_REGISTERED:
            return SCOREP_StringHandle_Get( eventAttrib->message.registered->str );
        default:
            break;
    }

    return "<unknown>";
}

SCOREP_RegionHandle
scorep_cuda_nvtx_get_user_region( nvtxDomainHandle_t           domain,
                                  const nvtxEventAttributes_t* eventAttrib )
{
    if ( domain == SCOREP_CUDA_NVTX_DEFAULT_DOMAIN )
    {
        domain = default_domain;
    }

    const char*        name = scorep_cuda_nvtx_get_name_from_attributes( eventAttrib );
    region_table_key_t key  = {
        .hash_value  = jenkins_hash( name, strlen( name ), jenkins_hash( &domain, sizeof( domain ), 0 ) ),
        .domain      = domain,
        .region_name = name
    };

    SCOREP_RegionHandle region = SCOREP_INVALID_REGION;
    region_table_get_and_insert( key, NULL, &region );

    return region;
}

void
scorep_cuda_nvtx_apply_payload( const nvtxEventAttributes_t* eventAttrib )
{
    switch ( eventAttrib->payloadType )
    {
        case NVTX_PAYLOAD_TYPE_UNSIGNED_INT32:
            SCOREP_AddAttribute( nvtx_attribute_uint32, &eventAttrib->payload.uiValue );
            break;
        case NVTX_PAYLOAD_TYPE_UNSIGNED_INT64:
            SCOREP_AddAttribute( nvtx_attribute_uint64, &eventAttrib->payload.ullValue );
            break;

        case NVTX_PAYLOAD_TYPE_INT32:
            SCOREP_AddAttribute( nvtx_attribute_int32, &eventAttrib->payload.iValue );
            break;
        case NVTX_PAYLOAD_TYPE_INT64:
            SCOREP_AddAttribute( nvtx_attribute_int64, &eventAttrib->payload.llValue );
            break;

        case NVTX_PAYLOAD_TYPE_FLOAT:
            SCOREP_AddAttribute( nvtx_attribute_float, &eventAttrib->payload.fValue );
            break;
        case NVTX_PAYLOAD_TYPE_DOUBLE:
            SCOREP_AddAttribute( nvtx_attribute_double, &eventAttrib->payload.dValue );
            break;
        default:
            return;
    }
}

void
scorep_cuda_nvtx_name_category( nvtxDomainHandle_t domain,
                                uint32_t           category,
                                const char*        name )
{
    if ( domain == SCOREP_CUDA_NVTX_DEFAULT_DOMAIN )
    {
        domain = default_domain;
    }

    category_table_key_t key = {
        .domain   = domain,
        .category = category
    };

    SCOREP_StringHandle string = SCOREP_INVALID_STRING;
    category_table_get_and_insert( key, &name, &string );
}

void
scorep_cuda_nvtx_apply_category( nvtxDomainHandle_t           domain,
                                 const nvtxEventAttributes_t* eventAttrib )
{
    if ( domain == SCOREP_CUDA_NVTX_DEFAULT_DOMAIN )
    {
        domain = default_domain;
    }

    category_table_key_t key = {
        .domain   = domain,
        .category = eventAttrib->category
    };

    SCOREP_StringHandle category_name;
    if ( category_table_get( key, &category_name ) )
    {
        SCOREP_TriggerParameterStringHandle( nvtx_parameter_category, category_name );
    }
}

void
scorep_cuda_nvtx_set_stream_name( void*       stream,
                                  const char* name )
{
    scorep_cupti_stream_set_name(
        scorep_cupti_stream_get( ( CUstream )stream ), name );
}

void
scorep_cuda_nvtx_set_context_name( void*       context,
                                   const char* name )
{
    scorep_cupti_context_set_name(
        scorep_cupti_context_get( ( CUcontext )context ), name );
}
