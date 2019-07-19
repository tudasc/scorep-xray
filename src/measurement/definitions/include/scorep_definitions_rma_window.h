/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013, 2019
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_RMA_WINDOW_H
#define SCOREP_PRIVATE_DEFINITIONS_RMA_WINDOW_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( RmaWindow )
{
    SCOREP_DEFINE_DEFINITION_HEADER( RmaWindow );

    SCOREP_StringHandle       name_handle;
    SCOREP_CommunicatorHandle communicator_handle;
    /* Number identifying this to be the i-th window on this comm */
    uint32_t creation_id;
    /* Flag indicating whether default name is still set */
    bool has_default_name;
};


/**
 * Associate the parameter tuple with a process unique RMA window handle.
 *
 * @param name A meaningful name for the RMA window, e.g. 'MPI window'
 *             or 'Gfx Card 1'. The string will be copied. Or NULL, if 'SetName'
 *             may be used. Will be set to "", if not done so.
 *
 * @param communicatorHandle Underlying communicator. At creation time, this must
 *                           be a InterimCommunicator handle, but this will
 *                           automatically resolved to the unified Communicator
 *                           handle in the unification.
 *
 * @return A process unique RMA window handle to be used in calls to other
 * SCOREP_Rma* functions.
 */
SCOREP_RmaWindowHandle
SCOREP_Definitions_NewRmaWindow( const char*                      name,
                                 SCOREP_InterimCommunicatorHandle communicatorHandle );


void
SCOREP_RmaWindowHandle_SetName( SCOREP_RmaWindowHandle rmaWindowHandle,
                                const char*            name );

void
scorep_definitions_unify_rma_window( SCOREP_RmaWindowDef*                 definition,
                                     struct SCOREP_Allocator_PageManager* handlesPageManager );

void
scorep_definitions_rehash_rma_window( SCOREP_RmaWindowDef* definition );


#endif /* SCOREP_PRIVATE_DEFINITIONS_RMA_WINDOW_H */
