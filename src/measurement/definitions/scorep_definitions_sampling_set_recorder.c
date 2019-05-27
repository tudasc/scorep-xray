/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>




static void
scorep_sampling_set_add_recorder( SCOREP_DefinitionManager* definition_manager,
                                  SCOREP_SamplingSetDef*    samplingSet,
                                  SCOREP_SamplingSetHandle  samplingSetHandle,
                                  SCOREP_LocationHandle     recorderHandle );


void
SCOREP_SamplingSet_AddRecorder( SCOREP_SamplingSetHandle samplingSetHandle,
                                SCOREP_LocationHandle    recorderHandle )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_SamplingSetDef* sampling_set = SCOREP_LOCAL_HANDLE_DEREF(
        samplingSetHandle,
        SamplingSet );
    UTILS_BUG_ON( sampling_set->is_scoped,
                  "Using a scoped sampling set as the reference sampling set recorder." );
    UTILS_BUG_ON( sampling_set->klass == SCOREP_SAMPLING_SET_ABSTRACT,
                  "Referenced sampling set should not be of class abstract." );

    SCOREP_LocationDef* recorder = SCOREP_LOCAL_HANDLE_DEREF(
        recorderHandle,
        Location );
    UTILS_BUG_ON( !( ( recorder->location_type == SCOREP_LOCATION_TYPE_CPU_THREAD && sampling_set->klass == SCOREP_SAMPLING_SET_CPU )
                     || ( recorder->location_type == SCOREP_LOCATION_TYPE_GPU && sampling_set->klass == SCOREP_SAMPLING_SET_GPU ) ),
                  "The sampling set class must match the location type." );

    SCOREP_Definitions_Lock();

    scorep_sampling_set_add_recorder( &scorep_local_definition_manager,
                                      sampling_set,
                                      samplingSetHandle,
                                      recorderHandle );

    SCOREP_Definitions_Unlock();
}


static void
scorep_sampling_set_add_recorder( SCOREP_DefinitionManager* definition_manager,
                                  SCOREP_SamplingSetDef*    samplingSet,
                                  SCOREP_SamplingSetHandle  samplingSetHandle,
                                  SCOREP_LocationHandle     recorderHandle )
{
    SCOREP_SamplingSetRecorderDef*   new_definition = NULL;
    SCOREP_SamplingSetRecorderHandle new_handle     = SCOREP_INVALID_SAMPLING_SET_RECORDER;

    SCOREP_DEFINITION_ALLOC( SamplingSetRecorder );

    /* They won't be check of duplicates, because the referenced locations won't too. */

    new_definition->sampling_set_handle = samplingSetHandle;
    new_definition->recorder_handle     = recorderHandle;

    /* Chain this into the sampling set. */
    new_definition->next         = SCOREP_INVALID_SAMPLING_SET_RECORDER;
    *samplingSet->recorders_tail = new_handle;
    samplingSet->recorders_tail  = &new_definition->recorders_next;

    /* Chain into definition manager */
    *definition_manager->sampling_set_recorder.tail = new_handle;
    definition_manager->sampling_set_recorder.tail  = &new_definition->next;
    new_definition->sequence_number                 = definition_manager->sampling_set_recorder.counter++;

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( recorderHandle, SCOREP_HANDLE_TYPE_SAMPLING_SET_RECORDER ) );
    }
}


void
scorep_definitions_unify_sampling_set_recorder( SCOREP_SamplingSetRecorderDef* definition,
                                                SCOREP_Allocator_PageManager*  handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_SamplingSetHandle unified_sampling_set_handle = SCOREP_HANDLE_GET_UNIFIED(
        definition->sampling_set_handle,
        SamplingSet,
        handlesPageManager );

    SCOREP_SamplingSetDef* unified_sampling_set = SCOREP_HANDLE_DEREF(
        unified_sampling_set_handle,
        SamplingSet,
        scorep_unified_definition_manager->page_manager );

    scorep_sampling_set_add_recorder(
        scorep_unified_definition_manager,
        unified_sampling_set,
        unified_sampling_set_handle,
        SCOREP_HANDLE_GET_UNIFIED( definition->recorder_handle,
                                   Location,
                                   handlesPageManager ) );
}
