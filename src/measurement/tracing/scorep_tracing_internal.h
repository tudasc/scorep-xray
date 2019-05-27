/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_INTERNAL_TRACING_H
#define SCOREP_INTERNAL_TRACING_H


/**
 * @file
 *
 */


#include <otf2/otf2.h>


#include <UTILS_Error.h>

#include <SCOREP_Definitions.h>

#include "scorep_rewind_stack_management.h"
#include <scorep_location_management.h>

extern bool     scorep_tracing_use_sion;
extern uint64_t scorep_tracing_max_procs_per_sion_file;
extern bool     scorep_tracing_convert_calling_context;

/**
 * Stores the substrate id of the tracing substrate. Set during initialization.
 */
extern size_t scorep_tracing_substrate_id;

typedef struct SCOREP_TracingData SCOREP_TracingData;
struct SCOREP_TracingData
{
    OTF2_EvtWriter*      otf_writer;
    scorep_rewind_stack* rewind_stack;
    scorep_rewind_stack* rewind_free_list;
    OTF2_AttributeList*  otf_attribute_list;
};


struct SCOREP_Location;


static inline SCOREP_TracingData*
scorep_tracing_get_trace_data( struct SCOREP_Location* location )
{
    return ( SCOREP_TracingData* )
           SCOREP_Location_GetSubstrateData( location,
                                             scorep_tracing_substrate_id );
}


OTF2_EvtWriter*
SCOREP_Tracing_GetEventWriter( void );


SCOREP_ErrorCode
scorep_tracing_set_collective_callbacks( OTF2_Archive* archive );


SCOREP_ErrorCode
scorep_tracing_set_locking_callbacks( OTF2_Archive* archive );


#endif /* SCOREP_INTERNAL_TRACING_H */
