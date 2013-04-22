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

#ifndef SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_H
#define SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_H


/**
 * @file       src/measurement/definitions/scorep_definitions_sampling_set.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( SamplingSet )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SamplingSet );

    // order is important
    bool is_scoped;

    /* offset after this struct to the caching for tracing */
    size_t tracing_cache_offset;

    SCOREP_MetricOccurrence occurrence;
    uint8_t                 number_of_metrics;
    SCOREP_MetricHandle     metric_handles[];
};


/*
 * this is not a real definition
 * but an overload of the SamplingSet definition
 * because they share the same id namespace in OTF2
 */
SCOREP_DEFINE_DEFINITION_TYPE( ScopedSamplingSet )
{
    // Don't use ScopedSamplingSet here, there is no handle for this definition
    SCOREP_DEFINE_DEFINITION_HEADER( SamplingSet );

    // order is important
    bool is_scoped;

    SCOREP_SamplingSetHandle sampling_set_handle;
    SCOREP_LocationHandle    recorder_handle;
    SCOREP_MetricScope       scope_type;
    /* all types are handles */
    SCOREP_AnyHandle scope_handle;
};


void
SCOREP_CopySamplingSetDefinitionToUnified( SCOREP_SamplingSet_Definition* definition,
                                           SCOREP_Allocator_PageManager*  handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_SAMPLING_SET_H */
