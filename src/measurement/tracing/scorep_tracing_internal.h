/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
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


#include "scorep_rewind_stack_management.h"


extern bool     scorep_tracing_use_sion;
extern uint64_t scorep_tracing_max_procs_per_sion_file;


struct SCOREP_TracingData
{
    OTF2_EvtWriter*      otf_writer;
    scorep_rewind_stack* rewind_stack;
    scorep_rewind_stack* rewind_free_list;
    OTF2_AttributeList*  otf_attribute_list;
};


OTF2_EvtWriter*
SCOREP_Tracing_GetEventWriter( void );


void
SCOREP_Tracing_LockArchive( void );


void
SCOREP_Tracing_UnlockArchive( void );


SCOREP_ErrorCode
scorep_tracing_set_collective_callbacks( OTF2_Archive* archive );


#endif /* SCOREP_INTERNAL_TRACING_H */
