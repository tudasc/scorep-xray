/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_PAGE_H
#define SCOREP_INTERNAL_PAGE_H



/**
 * @file
 *
 *
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "scorep_allocator.h"

static inline uint32_t
page_size( const SCOREP_Allocator_Allocator* allocator )
{
    uint32_t o = 1;
    return o << allocator->page_shift;
}


static inline uint32_t
total_memory( const SCOREP_Allocator_Allocator* allocator )
{
    return allocator->n_pages_capacity << allocator->page_shift;
}


static inline uint32_t
page_mask( const SCOREP_Allocator_Allocator* allocator )
{
    return page_size( allocator ) - 1;
}


static inline uint32_t
get_page_id( const SCOREP_Allocator_Page* page )
{
    intptr_t offset  = ( char* )page->memory_start_address - ( char* )page->allocator;
    uint32_t page_id = offset >> page->allocator->page_shift;

    return page_id;
}

static inline uint32_t
get_order( const SCOREP_Allocator_Allocator* allocator, uint32_t length )
{
    uint32_t order = ( length >> allocator->page_shift )
                     + !!( length & page_mask( allocator ) );

    return order;
}

static inline void
set_page_order( SCOREP_Allocator_Page* page, uint32_t order )
{
    uint32_t length = order << page->allocator->page_shift;
    page->memory_end_address = page->memory_start_address + length;
}

static inline void
set_page_usage( SCOREP_Allocator_Page* page, uint32_t usage )
{
    page->memory_current_address = page->memory_start_address + usage;
}

static inline void
init_page( SCOREP_Allocator_Allocator* allocator,
           SCOREP_Allocator_Page*      page,
           uint32_t                    id,
           uint32_t                    order )
{
    page->allocator            = allocator;
    page->memory_start_address = ( char* )allocator + ( id << allocator->page_shift );
    set_page_usage( page, 0 );
    set_page_order( page, order );
    page->next = NULL;
}

static inline void
clear_page( SCOREP_Allocator_Page* page )
{
    set_page_usage( page, 0 );
    set_page_order( page, 1 );
    page->next = NULL;
}

static inline uint32_t
get_page_length( const SCOREP_Allocator_Page* page )
{
    ptrdiff_t length = page->memory_end_address - page->memory_start_address;

    return length;
}

static inline uint32_t
get_page_order( const SCOREP_Allocator_Page* page )
{
    ptrdiff_t length = get_page_length( page );
    uint32_t  order  = length >> page->allocator->page_shift;

    return order;
}

static inline uint32_t
get_page_usage( const SCOREP_Allocator_Page* page )
{
    ptrdiff_t usage = page->memory_current_address - page->memory_start_address;

    return usage;
}

static inline uint32_t
get_page_avail( const SCOREP_Allocator_Page* page )
{
    ptrdiff_t avail = page->memory_end_address - page->memory_current_address;

    return avail;
}

#endif /* SCOREP_INTERNAL_PAGE_H */
