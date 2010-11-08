/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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

#ifndef SCOREP_INTERNAL_TRACE_TYPES_H
#define SCOREP_INTERNAL_TRACE_TYPES_H



/**
 * @file       scorep_trace_types.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <SCOREP_DefinitionHandles.h>

#include <otf2/OTF2_EvtWriter.h>


typedef struct SCOREP_Trace_LocationData SCOREP_Trace_LocationData;


struct SCOREP_Trace_LocationData
{
    OTF2_EvtWriter* otf_writer;
    uint64_t        otf_location;
};

#endif /* SCOREP_INTERNAL_TRACE_TYPES_H */
