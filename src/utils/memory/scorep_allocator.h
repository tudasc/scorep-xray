/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
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


#ifndef SCOREP_INTERNAL_ALLOCATOR_H
#define SCOREP_INTERNAL_ALLOCATOR_H


/**
 * @file
 *
 *
 */


#include <SCOREP_Allocator.h>

#include <stdbool.h>
#include <stdint.h>


typedef union SCOREP_Allocator_Object SCOREP_Allocator_Object;


struct SCOREP_Allocator_Page
{
    SCOREP_Allocator_Allocator* allocator;

    char*                       memory_start_address;
    char*                       memory_end_address;     // use until end address
                                                        // but not beyond
    char*                       memory_current_address; // internal use only!

    // SCOREP_Allocator_PageManager and SCOREP_Allocator_Allocator store pages in lists.
    // For internal use only
    SCOREP_Allocator_Page* next;
};


struct SCOREP_Allocator_Allocator
{
    uint32_t page_shift;
    uint32_t n_pages_capacity;
    uint32_t n_pages_maintenance;
    uint32_t n_pages_high_watermark;
    uint32_t n_pages_allocated;
    //uint32_t union_size;
    //uint32_t reserved;

    /** free objects */
    SCOREP_Allocator_Object*     free_objects;

    SCOREP_Allocator_Guard       lock;
    SCOREP_Allocator_Guard       unlock;
    SCOREP_Allocator_GuardObject lock_object;
};


struct SCOREP_Allocator_PageManager
{
    SCOREP_Allocator_Allocator* allocator;         // fetch new pages from this one
    SCOREP_Allocator_Page*      pages_in_use_list;

    uint32_t*                   moved_page_id_mapping;

    /* sentinel which allocation could be rolled back */
    /* only movable allocations currently */
    SCOREP_Allocator_MovableMemory last_allocation;
};


struct SCOREP_Allocator_ObjectManager
{
    SCOREP_Allocator_Allocator* allocator;         // fetch new pages from this one
    SCOREP_Allocator_Page*      pages_in_use_list;
    /* list of free objects */
    void*                       free_list;
    /* size of each object, was rounded-up to meet the required alignment */
    uint32_t                    object_size;
};


union SCOREP_Allocator_Object
{
    union SCOREP_Allocator_Object*        next;
    /* 32: 24, 64: 40 */
    struct SCOREP_Allocator_Page          page;
    /* 32: 16, 64: 32 */
    struct SCOREP_Allocator_PageManager   page_manager;
    /* 32: 16, 64: 32 */
    struct SCOREP_Allocator_ObjectManager object_manager;
};


#endif /* SCOREP_INTERNAL_ALLOCATOR_H */
