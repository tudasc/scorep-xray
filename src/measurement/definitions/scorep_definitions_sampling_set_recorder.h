/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_RECORDER_H
#define SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_RECORDER_H


/**
 * @file       src/measurement/definitions/scorep_definitions_sampling_set_recorder.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( SamplingSetRecorder )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SamplingSetRecorder );

    SCOREP_SamplingSetHandle sampling_set_handle;
    SCOREP_LocationHandle    recorder_handle;

    /* Chain of recorders of one sampling set. */
    SCOREP_SamplingSetRecorderHandle recorders_next;
};


void
scorep_definitions_unify_sampling_set_recorder( SCOREP_SamplingSetRecorderDef* definition,
                                                SCOREP_Allocator_PageManager*  handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_RECORDER_H */
