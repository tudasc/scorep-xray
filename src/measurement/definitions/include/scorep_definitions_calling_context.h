/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_CALLING_CONTEXT_H
#define SCOREP_PRIVATE_DEFINITIONS_CALLING_CONTEXT_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( CallingContext )
{
    SCOREP_DEFINE_DEFINITION_HEADER( CallingContext );

    uint64_t            ip;
    uint64_t            ip_offset;
    SCOREP_StringHandle file_handle;

    SCOREP_RegionHandle             region_handle;
    SCOREP_SourceCodeLocationHandle scl_handle;
    SCOREP_CallingContextHandle     parent_handle;
};


SCOREP_CallingContextHandle
SCOREP_Definitions_NewCallingContext( uint64_t                        ip,
                                      SCOREP_RegionHandle             region,
                                      SCOREP_SourceCodeLocationHandle scl,
                                      SCOREP_CallingContextHandle     parent );

SCOREP_RegionHandle
SCOREP_CallingContextHandle_GetRegion( SCOREP_CallingContextHandle handle );

SCOREP_CallingContextHandle
SCOREP_CallingContextHandle_GetParent( SCOREP_CallingContextHandle handle );

void
scorep_definitions_unify_calling_context( SCOREP_CallingContextDef*            definition,
                                          struct SCOREP_Allocator_PageManager* handlesPageManager );


SCOREP_DEFINE_DEFINITION_TYPE( InterruptGenerator )
{
    SCOREP_DEFINE_DEFINITION_HEADER( InterruptGenerator );

    SCOREP_StringHandle           name_handle;
    SCOREP_InterruptGeneratorMode mode;
    SCOREP_MetricBase             base;
    int64_t                       exponent;
    uint64_t                      period;
};


SCOREP_InterruptGeneratorHandle
SCOREP_Definitions_NewInterruptGenerator( const char*                   name,
                                          SCOREP_InterruptGeneratorMode mode,
                                          SCOREP_MetricBase             base,
                                          int64_t                       exponent,
                                          uint64_t                      period );


void
scorep_definitions_unify_interrupt_generator( SCOREP_InterruptGeneratorDef*        definition,
                                              struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_CALLING_CONTEXT_H */
