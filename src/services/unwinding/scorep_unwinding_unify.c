/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2021-2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_unwinding_mgmt.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_Location.h>
#include <SCOREP_Addr2line.h>
#include <SCOREP_Demangle.h>

#include "scorep_unwinding_region.h"


/* *INDENT-OFF* */
static void finalize_region( scorep_unwinding_region* region, void* data );
static bool finalize_region_definitions( SCOREP_Location* location, void* data );
/* *INDENT-ON* */

SCOREP_ErrorCode
scorep_unwinding_unify( void )
{
    /*
     * Iterate over all calling context definitions and resolve instruction
     * address to SCL
     */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN(
        &scorep_local_definition_manager,
        CallingContext,
        calling_context )
    {
        if ( !definition->ip )
        {
            continue;
        }

        definition->ip_offset = definition->ip;

#if !HAVE( SCOREP_ADDR2LINE )
        continue;
#else   /* HAVE( SCOREP_ADDR2LINE ) */
        void*       so_handle;
        const char* so_file_name;
        uintptr_t   so_base_addr;
        uint16_t    so_token_unused;

        bool        scl_found;
        const char* scl_file_name = NULL;
        const char* scl_function_name_unused;
        unsigned    scl_line_no = SCOREP_INVALID_LINE_NO;

        SCOREP_Addr2line_LookupAddr( ( uintptr_t )definition->ip,
                                     &so_handle,
                                     &so_file_name,
                                     &so_base_addr,
                                     &so_token_unused,
                                     &scl_found,
                                     &scl_file_name,
                                     &scl_function_name_unused,
                                     &scl_line_no );
        if ( so_handle == NULL )
        {
            continue;
        }

        definition->ip_offset  -= ( uint64_t )so_base_addr;
        definition->file_handle = SCOREP_Definitions_NewString( so_file_name );

        if ( scl_found && scl_file_name && *scl_file_name && scl_line_no != SCOREP_INVALID_LINE_NO )
        {
            definition->scl_handle = SCOREP_Definitions_NewSourceCodeLocation( scl_file_name, scl_line_no );
        }
#endif  /* HAVE( SCOREP_ADDR2LINE ) */
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    SCOREP_Location_ForAll( finalize_region_definitions, NULL );

    return SCOREP_SUCCESS;
}

static bool
finalize_region_definitions( SCOREP_Location* location,
                             void*            dataUnused )
{
    SCOREP_LocationType location_type = SCOREP_Location_GetType( location );

    if ( location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }

    SCOREP_Unwinding_CpuLocationData* unwind_data =
        SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    /* we clear the known regions and apply the post processing while at it */
    scorep_unwinding_region_clear( unwind_data, finalize_region, dataUnused );

    return false;
}

static void
finalize_region( scorep_unwinding_region* region,
                 void*                    dataUnused )
{
    if ( region->handle == SCOREP_INVALID_REGION )
    {
        return;
    }

    SCOREP_RegionDef* definition =
        SCOREP_LOCAL_HANDLE_DEREF( region->handle, Region );

    const char* region_name    = SCOREP_RegionHandle_GetName( region->handle );
    char*       demangled_name = SCOREP_Demangle( region_name,
                                                  SCOREP_DEMANGLE_DEFAULT );
    if ( demangled_name )
    {
        definition->name_handle = SCOREP_Definitions_NewString( demangled_name );
        free( demangled_name );
    }

#if !HAVE( SCOREP_ADDR2LINE )
    return;
#else /* HAVE( SCOREP_ADDR2LINE ) */
    void*       so_handle;
    const char* so_file_name_unused;
    uintptr_t   so_base_addr_unused;
    uint16_t    so_token_unused;

    bool        scl_found_begin_addr;
    bool        scl_found_end_addr;
    const char* scl_file_name = NULL;
    const char* scl_function_name_unused;
    unsigned    scl_begin_line_no = SCOREP_INVALID_LINE_NO;
    unsigned    scl_end_line_no   = SCOREP_INVALID_LINE_NO;

    SCOREP_Addr2line_LookupAddrRange( ( uintptr_t )region->start,
                                      ( uintptr_t )region->end,
                                      &so_handle,
                                      &so_file_name_unused,
                                      &so_base_addr_unused,
                                      &so_token_unused,
                                      &scl_found_begin_addr,
                                      &scl_found_end_addr,
                                      &scl_file_name,
                                      &scl_function_name_unused,
                                      &scl_begin_line_no,
                                      &scl_end_line_no );
    if ( so_handle == NULL )
    {
        return;
    }

    if ( scl_found_begin_addr && scl_file_name && *scl_file_name && scl_begin_line_no != SCOREP_INVALID_LINE_NO )
    {
        SCOREP_SourceFileHandle source_file_handle = SCOREP_Definitions_NewSourceFile( scl_file_name );
        definition->file_name_handle = SCOREP_LOCAL_HANDLE_DEREF( source_file_handle, SourceFile )->name_handle;
        definition->begin_line       = scl_begin_line_no;
        if ( scl_found_end_addr && scl_end_line_no != SCOREP_INVALID_LINE_NO )
        {
            definition->end_line = scl_end_line_no;
        }
    }
#endif /* HAVE( SCOREP_ADDR2LINE ) */
}
