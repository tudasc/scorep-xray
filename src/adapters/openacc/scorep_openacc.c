/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Utility functionality for OpenACC adapter.
 */

#include <config.h>
#include "scorep_openacc_confvars.h"
#include "scorep_openacc.h"

#define SCOREP_DEBUG_MODULE_NAME OPENACC
#include <UTILS_Debug.h>

#include <SCOREP_Memory.h>

#include <string.h>

/****************************** OpenACC mutex *********************************/
#define ACC_LOCK() SCOREP_MutexLock( scorep_openacc_mutex )
#define ACC_UNLOCK() SCOREP_MutexUnlock( scorep_openacc_mutex )
/**************************** END: OpenACC mutex ******************************/

/************************* Hashing of OpenACC regions *************************/

/*
 * The key of the hash node is a the triple line, type and file. The value is
 * the region handle.
 */
typedef struct acc_region_node
{
    int                     line;   //!< line number the event is associated with
    acc_event_t             type;   //!< type of OpenACC event
    SCOREP_RegionHandle     region; //!< associated region handle
    struct acc_region_node* next;   //!< bucket for collision
    char                    file[]; //!< name of the source file
} acc_region_node;

#define REGION_HASHTABLE_SIZE 1024 // default size of OpenACC region hash table
static acc_region_node* hashtab_acc_region[ REGION_HASHTABLE_SIZE ];

/**
 * Insert an OpenACC region into the OpenACC region hash table. Line number,
 * event type, and source file identify the region.
 *
 * @param lineNo line number
 * @param regionType OpenACC event/region type
 * @param srcFile source file
 * @param region Score-P region handle
 */
static void
hash_add_acc_region( int                 lineNo,
                     acc_event_t         regionType,
                     const char*         srcFile,
                     SCOREP_RegionHandle region );

/**
 * Retrieves an OpenACC region from the OpenACC region hash table.
 *
 * @param lineNo line number
 * @param regionType OpenACC event/region type
 * @param srcFile source file
 * @return Score-P region handle
 */
static SCOREP_RegionHandle
hash_get_acc_region( int         lineNo,
                     acc_event_t regionType,
                     const char* srcFile );

/********************** END: Hashing of OpenACC regions ***********************/

/*
 * Score-P OpenACC device structure
 * (Currently, only used for GPU memory allocation tracking.)
 */
typedef struct scorep_openacc_device
{
    int                           device_id;
    acc_device_t                  device_type;
    struct SCOREP_AllocMetric*    allocMetric;
    struct scorep_openacc_device* next;
} scorep_openacc_device;

/* Global device list. Should be very short in most cases. Needs locking. */
static scorep_openacc_device* device_list;

/* User-specified OpenACC recording features */
bool scorep_openacc_features_initialized = false;
bool scorep_openacc_record_regions       = false;
bool scorep_openacc_record_enqueue       = false;
bool scorep_openacc_record_wait          = false;
bool scorep_openacc_record_device_alloc  = false;
bool scorep_openacc_record_kernel_props  = false;
bool scorep_openacc_record_varnames      = false;

/* File handle for OpenACC regions */
static SCOREP_SourceFileHandle acc_region_file_handle = SCOREP_INVALID_SOURCE_FILE;

/* Attributes for additional information on OpenACC regions */
SCOREP_AttributeHandle scorep_openacc_attribute_implicit       = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_openacc_attribute_transfer_bytes = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_openacc_attribute_variable_name  = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_openacc_attribute_kernel_name    = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_openacc_attribute_kernel_gangs   = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_openacc_attribute_kernel_workers = SCOREP_INVALID_ATTRIBUTE;
SCOREP_AttributeHandle scorep_openacc_attribute_kernel_veclen  = SCOREP_INVALID_ATTRIBUTE;

/**
 * Evaluate the user-specified features and setup adapter accordingly.
 * (We assume that this function cannot be executed concurrently by multiple
 * threads.)
 */
void
scorep_openacc_setup_features( void )
{
    UTILS_DEBUG( "[OpenACC] Setup features" );

    if ( !scorep_openacc_features_initialized && scorep_openacc_features )
    {
        // create attribute handle for implicit OpenACC regions
        scorep_openacc_attribute_implicit = SCOREP_Definitions_NewAttribute(
            "acc_implicit",
            "Compiler-generated OpenACC activity",
            SCOREP_ATTRIBUTE_TYPE_INT8 );

        // check for OpenACC regions
        if ( scorep_openacc_features & SCOREP_OPENACC_FEATURE_REGIONS )
        {
            scorep_openacc_record_regions = true;
        }

        // check for OpenACC enqueue operations
        if ( scorep_openacc_features & SCOREP_OPENACC_FEATURE_ENQUEUE )
        {
            scorep_openacc_attribute_transfer_bytes = SCOREP_Definitions_NewAttribute(
                "acc_transfer_bytes",
                "Bytes to transfer between host and accelerator",
                SCOREP_ATTRIBUTE_TYPE_UINT64 );

            scorep_openacc_record_enqueue = true;
        }

        // write kernel properties, if enabled and also enqueue operations tracked
        if ( scorep_openacc_features & SCOREP_OPENACC_FEATURE_KERNEL_PROPERTIES &&
             scorep_openacc_record_enqueue )
        {
            scorep_openacc_attribute_kernel_name = SCOREP_Definitions_NewAttribute(
                "acc_kernel_name",
                "Name of the launched kernel",
                SCOREP_ATTRIBUTE_TYPE_STRING );

            scorep_openacc_attribute_kernel_gangs = SCOREP_Definitions_NewAttribute(
                "acc_kernel_gangs",
                "Kernel gang size",
                SCOREP_ATTRIBUTE_TYPE_UINT64 );

            scorep_openacc_attribute_kernel_workers = SCOREP_Definitions_NewAttribute(
                "acc_kernel_workers",
                "Kernel worker size",
                SCOREP_ATTRIBUTE_TYPE_UINT64 );

            scorep_openacc_attribute_kernel_veclen = SCOREP_Definitions_NewAttribute(
                "acc_kernel_vector_length",
                "Kernel vector length",
                SCOREP_ATTRIBUTE_TYPE_UINT64 );

            scorep_openacc_record_kernel_props = true;
        }

        // check for OpenACC synchronization
        if ( scorep_openacc_features & SCOREP_OPENACC_FEATURE_WAIT )
        {
            scorep_openacc_record_wait = true;
        }

        // check for OpenACC device memory usage
        if ( scorep_openacc_features & SCOREP_OPENACC_FEATURE_DEVICE_ALLOC )
        {
            scorep_openacc_record_device_alloc = true;
        }

        // check if variable name recording is enabled
        // triggered only in upload/download or allocation events
        if ( scorep_openacc_features & SCOREP_OPENACC_FEATURE_VARNAMES &&
             ( scorep_openacc_record_enqueue /*|| scorep_openacc_record_device_alloc*/ ) )
        {
            scorep_openacc_attribute_variable_name = SCOREP_Definitions_NewAttribute(
                "acc_variable_name",
                "Name of the allocated or transferred variable",
                SCOREP_ATTRIBUTE_TYPE_STRING );

            scorep_openacc_record_varnames = true;
        }

        scorep_openacc_features_initialized = true;
    }
}

/**
 * Create an OpenACC region name string.
 *
 * @param srcFile       Source file
 * @param lineNo        Line number
 * @param regionType    OpenACC event/region type
 *
 * @return OpenACC region name
 */
static char*
create_acc_region_string( const char* srcFile,
                          int         lineNo,
                          acc_event_t regionType )
{
    char*  region_name        = NULL;
    size_t region_name_length = 0;
    char*  region_type_string = NULL;

    // set region type names according to the event type
    switch ( regionType )
    {
        case acc_ev_device_init_start:
            region_type_string = "init";
            region_name_length = 4;
            break;
        case acc_ev_device_shutdown_start:
            region_type_string = "shutdown";
            region_name_length = 8;
            break;
        case acc_ev_compute_construct_start:
            region_type_string = "compute";
            region_name_length = 7;
            break;
        case acc_ev_update_start:
            region_type_string = "update";
            region_name_length = 6;
            break;
        case acc_ev_enter_data_start:
            region_type_string = "data_enter";
            region_name_length = 10;
            break;
        case acc_ev_exit_data_start:
            region_type_string = "data_exit";
            region_name_length = 9;
            break;
        case acc_ev_enqueue_launch_start:
            region_type_string = "launch_kernel";
            region_name_length = 13;
            break;
        case acc_ev_enqueue_upload_start:
            region_type_string = "upload";
            region_name_length = 6;
            break;
        case acc_ev_enqueue_download_start:
            region_type_string = "download";
            region_name_length = 8;
            break;
        case acc_ev_wait_start:
            region_type_string = "wait";
            region_name_length = 4;
            break;
        default:
            region_type_string = "unknown";
            region_name_length = 7;
            break;
    }

    // if the source code information are available
    if ( srcFile && lineNo > 0 )
    {
        // store the file name only, not the full path
        // find the last '/' ->
        const char* file_name =  strrchr( srcFile, '/' );

        // if only the file name is given
        if ( NULL == file_name )
        {
            file_name = srcFile;
        }
        else // if the full path is given
        {
            // start with character after the last '/'
            file_name += 1;
        }

        region_name_length += strlen( file_name );

        // add line number length, max. 10 digits with 32bit integer possible
        // add 'acc_', '@', ':', and null termination
        region_name_length += 17;

        region_name = ( char* )SCOREP_Memory_AllocForMisc(
            region_name_length * sizeof( char ) );

        // compose OpenACC region name
        if ( -1 == snprintf( region_name, region_name_length, "acc_%s@%s:%i",
                             region_type_string, file_name, lineNo ) )
        {
            UTILS_WARNING( "[OpenACC] Could not create region name for %s!", regionType );
            region_name = region_type_string;
        }
    }
    else
    {
        // add 'acc_' and null termination
        region_name_length += 5;

        region_name = ( char* )SCOREP_Memory_AllocForMisc(
            region_name_length * sizeof( char ) );

        // compose OpenACC region name
        if ( -1 == snprintf( region_name, region_name_length, "acc_%s",
                             region_type_string ) )
        {
            UTILS_WARNING( "[OpenACC] Could not create region name for %s!", regionType );
            region_name = region_type_string;
        }
    }

    return region_name;
}

/**
 * Get handle of an OpenACC region.
 *
 * @param lineNo        Line number
 * @param accEvent    OpenACC event/region type
 * @param srcFile       Source file
 *
 * @return Score-P handle of an OpenACC region
 */
SCOREP_RegionHandle
scorep_openacc_get_region_handle( int         lineNo,
                                  acc_event_t accEvent,
                                  const char* srcFile )
{
    // check for line number first
    SCOREP_RegionHandle regionHandle = SCOREP_INVALID_REGION;

    regionHandle = hash_get_acc_region( lineNo, accEvent, srcFile );

    // we expect this to be the infrequent execution path
    if ( SCOREP_INVALID_REGION == regionHandle )
    {
        ACC_LOCK();
        regionHandle = hash_get_acc_region( lineNo, accEvent, srcFile );

        if ( SCOREP_INVALID_REGION == regionHandle )
        {
            char* region_name = create_acc_region_string( srcFile, lineNo, accEvent );

            regionHandle = SCOREP_Definitions_NewRegion(
                region_name, NULL,
                acc_region_file_handle, 0, 0,
                SCOREP_PARADIGM_OPENACC, SCOREP_REGION_FUNCTION );

            hash_add_acc_region( lineNo, accEvent, srcFile, regionHandle );
        }

        ACC_UNLOCK();
    }

    return regionHandle;
}

/******************************************************************************/
/********************** GPU memory allocation tracking ************************/
/******************************************************************************/

/**
 * Get a Score-P allocation metric handle for a given device.
 * (Make sure to lock a call to this function!)
 *
 * @param deviceType type of the OpenACC device
 * @param deviceNumber device number
 *
 * @return Score-P allocation metric handle
 */
static struct SCOREP_AllocMetric*
get_alloc_metric_handle( acc_device_t deviceType,
                         int          deviceNumber )
{
    scorep_openacc_device* dev = device_list;
    while ( dev != NULL )
    {
        if ( dev->device_id == deviceNumber && dev->device_type == deviceType )
        {
            return dev->allocMetric;
        }

        dev = dev->next;
    }

    return NULL;
}

/**
 * Create a Score-P OpenACC device that also generates a metric handle for
 * device allocations.
 *
 * @param deviceType    OpenACC device type
 * @param deviceNumber  Device number
 *
 * @return Score-P OpenACC device with metric handle
 */
static scorep_openacc_device*
create_device( acc_device_t deviceType,
               int          deviceNumber )
{
    scorep_openacc_device* dev =
        ( scorep_openacc_device* )SCOREP_Memory_AllocForMisc(
            sizeof( scorep_openacc_device ) );

    // "acc_mem_usage " + 2 digits for deviceType + ":"
    // + 2 digits for device number + null termination
    const size_t metric_length = 14 + 2 + 1 + 2 + 1;

    // create a new metric handle
    char* acc_metric_name =
        ( char* )SCOREP_Memory_AllocForMisc( metric_length * sizeof( char ) );

    if ( -1 == snprintf( acc_metric_name, metric_length,
                         "acc_mem_usage %d:%i",
                         deviceType, deviceNumber ) )
    {
        UTILS_WARNING( "[OpenACC] Could not create metric name for device type %d!",
                       deviceType );
        acc_metric_name = "acc_mem_usage";
    }

    struct SCOREP_AllocMetric* allocMetric = NULL;

    SCOREP_AllocMetric_New( acc_metric_name, &allocMetric );

    dev->allocMetric = allocMetric;

    return dev;
}

struct SCOREP_AllocMetric*
scorep_openacc_get_alloc_metric_handle( acc_device_t deviceType,
                                        int          deviceNumber )
{
    struct SCOREP_AllocMetric* allocHandle = get_alloc_metric_handle( deviceType, deviceNumber );

    if ( allocHandle == NULL )
    {
        ACC_LOCK();
        allocHandle = get_alloc_metric_handle( deviceType, deviceNumber );

        if ( allocHandle == NULL )
        {
            scorep_openacc_device* dev = create_device( deviceType, deviceNumber );
            allocHandle = dev->allocMetric;

            // prepend device to global device list
            dev->next   = device_list;
            device_list = dev;
        }
        ACC_UNLOCK();
    }

    return allocHandle;
}
/******************************************************************************/
/******************** END: GPU memory allocation tracking *********************/
/******************************************************************************/

/******************************************************************************/
/*************** Implementation of OpenACC region hashing. ********************/
/******************************************************************************/

/**
 * Hash function for OpenACC regions. Uses line number and event type.
 * Including the event type avoids a single bucket list in hash table entry 0
 * if no source code information are available. However, a compiler should
 * always be able to provide OpenACC source code information, even without '-g'.
 *
 * @param lineNo     Line number
 * @param regionType OpenACC event/region type
 *
 * @return hash index for the respective OpenACC region hash table
 */
static inline uint16_t
hash_acc_region( int lineNo,  acc_event_t regionType )
{
    return ( uint16_t )( ( ( uint64_t )lineNo + regionType ) & ( REGION_HASHTABLE_SIZE - 1 ) );
}

/**
 * Adds an OpenACC region to the variables hash table.
 *
 * @param lineNo        Line number
 * @param regionType    OpenACC event/region type
 * @param srcFile       Source file
 * @param region        Score-P region handle
 */
static void
hash_add_acc_region( int                 lineNo,
                     acc_event_t         regionType,
                     const char*         srcFile,
                     SCOREP_RegionHandle region )
{
    //uint32_t id = ( uint32_t )lineNo % REGION_HASHTABLE_SIZE;
    uint16_t id = hash_acc_region( lineNo, regionType );

    size_t len = 0;

    if ( srcFile )
    {
        len += strlen( srcFile );
    }

    // allocate memory for node + srcFile string + one byte for null termination
    acc_region_node* add =
        ( acc_region_node* )SCOREP_Memory_AllocForMisc(
            sizeof( acc_region_node ) + ( len + 1 ) * sizeof( char ) );

    // add the source file name (as flexible array member)
    // note: 'srcFile' may be NULL, len may be 0
    memcpy( add->file, srcFile, len );
    ( add->file )[ len ] = '\0';

    add->line                = lineNo;
    add->type                = regionType;
    add->region              = region;
    add->next                = hashtab_acc_region[ id ];
    hashtab_acc_region[ id ] = add;
}

/**
 * Get a region from the variables hash table.
 *
 * @param lineNo        Line number
 * @param regionType    OpenACC event/region type
 * @param srcFile       Source file
 * @param region        Score-P region handle
 *
 * @return Score-P region handle for the given OpenACC region
 */
static SCOREP_RegionHandle
hash_get_acc_region( int         lineNo,
                     acc_event_t regionType,
                     const char* srcFile )
{
    //uint32_t id = ( uint32_t )lineNo & ( REGION_HASHTABLE_SIZE - 1 );
    uint16_t id = hash_acc_region( lineNo, regionType );

    acc_region_node* curr = hashtab_acc_region[ id ];

    while ( curr )
    {
        if ( curr->line == lineNo && curr->type == regionType &&
             strcmp( curr->file, srcFile ) == 0 )
        {
            return curr->region;
        }

        curr = curr->next;
    }

    return SCOREP_INVALID_REGION;
}
/******************************************************************************/
/************ END: Implementation of OpenACC region hashing. ******************/
/******************************************************************************/
