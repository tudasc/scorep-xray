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
 * @status     alpha
 * @file       scorep_compiler_gnu.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief SUN f90 compiler PHAT interface.
 * Compiler adapter for the SUN F90 compiler.
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>

#include <SCOREP_Types.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Events.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Compiler_Init.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Filter.h>

/* *INDENT-OFF* */
int on_scorep_finalize();
/* *INDENT-ON* */

#define SCOREP_FILTERED_REGION UINT64_MAX

#if SCOREP_FILTERED_REGION == SCOREP_INVALID_REGION
#error "SCOREP_FILTERED_REGION must not equal SCOREP_INVALID_REGION"
#endif

/*
 * -----------------------------------------------------------------------------
 * Simple hash table to map function names to region handles
 * -----------------------------------------------------------------------------
 */
typedef struct hash_node_struct
{
    long                     hash_code;     /* hash code (address of function name) */
    SCOREP_RegionHandle      region_handle; /* associated Score-P region identifier  */
    struct hash_node_struct* next;
} scorep_compiler_hash_node_t;

#define HASH_MAX 1021

static scorep_compiler_hash_node_t* htab[ HASH_MAX ];

static SCOREP_RegionHandle scorep_compiler_main_handle = SCOREP_INVALID_REGION;

static SCOREP_Mutex scorep_compiler_hash_lock;

/**
 * Flag that indicates that the compiler is finalized.
 */
static int scorep_compiler_finalized = 0;

/*
 * Stores Score-P region handle 'region' under hash code 'hash'
 */
static void
scorep_compiler_hash_put( long                hash,
                          SCOREP_RegionHandle region )
{
    long                         id  = hash % HASH_MAX;
    scorep_compiler_hash_node_t* add = malloc( sizeof( scorep_compiler_hash_node_t ) );
    add->hash_code     = hash;
    add->region_handle = region;
    add->next          = htab[ id ];
    htab[ id ]         = add;
}

/*
 * Lookup hash code "hash" Returns Score-P region identifier if already
 * stored, otherwise SCOREP_INVALID_REGION
 */
static SCOREP_RegionHandle
scorep_compiler_hash_get( long hash )
{
    long                         id   = hash % HASH_MAX;
    scorep_compiler_hash_node_t* curr = htab[ id ];
    while ( curr )
    {
        if ( curr->hash_code == hash )
        {
            return curr->region_handle;
        }
        curr = curr->next;
    }
    return SCOREP_INVALID_REGION;
}

/*
 * Register new region with Score-P system
 * 'str' is passed in from SUN compiler
 */
static SCOREP_RegionHandle
scorep_compiler_register_region( char* region_name )
{
    SCOREP_RegionHandle handle = SCOREP_FILTERED_REGION;

    /* register region with Score-P and store region identifier */
    if ( !SCOREP_Filter_Match( NULL, region_name, true ) )
    {
        handle = SCOREP_DefineRegion( region_name,
                                      SCOREP_INVALID_SOURCE_FILE,
                                      SCOREP_INVALID_LINE_NO,
                                      SCOREP_INVALID_LINE_NO,
                                      SCOREP_ADAPTER_COMPILER,
                                      SCOREP_REGION_FUNCTION );
    }
    scorep_compiler_hash_put( ( long )region_name, handle );
    return handle;
}

/*
 * This function is called at the entry of each function
 * The call is generated by the SUN f90 compilers
 */
void
phat_enter( char* name,
            int*  id )
{
    if ( !SCOREP_IsInitialized() )
    {
        if ( scorep_compiler_finalized )
        {
            return;
        }
        SCOREP_InitMeasurement();
    }

    /* ignore SUN OMP runtime functions */
    if ( strchr( name, '$' ) != NULL )
    {
        return;
    }

    SCOREP_RegionHandle handle = scorep_compiler_hash_get( ( long )name );

    if ( handle == SCOREP_INVALID_REGION )
    {
        /* region entered the first time, register region */
        SCOREP_MutexLock( scorep_compiler_hash_lock );
        handle = scorep_compiler_hash_get( ( long )name );
        if ( handle == SCOREP_INVALID_REGION )
        {
            handle = scorep_compiler_register_region( name );
        }
        SCOREP_MutexUnlock( scorep_compiler_hash_lock );
    }

    if ( handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_EnterRegion( handle );
    }
}


/*
 * This function is called at the exit of each function
 * The call is generated by the SUN F90 compilers
 */
void
phat_exit( char* name,
           int*  id )
{
    if ( scorep_compiler_finalized )
    {
        return;
    }

    SCOREP_RegionHandle handle = scorep_compiler_hash_get( ( long )name );

    if ( ( handle != SCOREP_INVALID_REGION ) && ( handle != SCOREP_FILTERED_REGION ) )
    {
        SCOREP_ExitRegion( handle );
    }
}

SCOREP_Error_Code
scorep_compiler_init_adapter()
{
    SCOREP_MutexCreate( &scorep_compiler_hash_lock  );
    /* The studio compiler does not instrument "main" but we want to have a
       main. Note that this main is triggered by the first event that arrives
       at the measurement system. See also on_scorep_finalize(). */
    scorep_compiler_main_handle = scorep_compiler_register_region( "main" );
    SCOREP_EnterRegion( scorep_compiler_main_handle );
    SCOREP_RegisterExitCallback( &on_scorep_finalize );
    return SCOREP_SUCCESS;
}

SCOREP_Error_Code
scorep_compiler_init_location( SCOREP_Location* location )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "studio compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

/* Location finalization */
void
scorep_compiler_final_location( void* locationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "studio compiler adapter final location!" );
}

int
on_scorep_finalize()
{
    /* We manually entered the artificial "main" region. We also need to exit
       it manually. See also scorep_compiler_init_adapter(). */
    SCOREP_ExitRegion( scorep_compiler_main_handle );
    return 0;
}

void
scorep_compiler_finalize()
{
    SCOREP_MutexDestroy( &scorep_compiler_hash_lock );
    scorep_compiler_finalized = 1;
}
