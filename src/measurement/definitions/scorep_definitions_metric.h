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

#ifndef SCOREP_PRIVATE_DEFINITIONS_METRIC_H
#define SCOREP_PRIVATE_DEFINITIONS_METRIC_H


/**
 * @file
 *
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( Metric )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Metric );

    // Add SCOREP_Metric stuff from here on.
    SCOREP_StringHandle        name_handle;
    SCOREP_StringHandle        description_handle;
    SCOREP_MetricSourceType    source_type;
    SCOREP_MetricMode          mode;
    SCOREP_MetricValueType     value_type;
    SCOREP_MetricBase          base;
    int64_t                    exponent;
    SCOREP_StringHandle        unit_handle;
    SCOREP_MetricProfilingType profiling_type;
};


void
scorep_definitions_unify_metric( SCOREP_MetricDef*             definition,
                                 SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_METRIC_H */
