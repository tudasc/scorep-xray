#ifndef SILC_INTERNAL_TRACE_TYPES_H
#define SILC_INTERNAL_TRACE_TYPES_H

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_trace_types.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */

#include <SILC_DefinitionHandles.h>

#include <otf2/OTF2_EvtWriter.h>


typedef struct SILC_Trace_LocationData SILC_Trace_LocationData;


struct SILC_Trace_LocationData
{
    OTF2_EvtWriter* otf_writer;
    uint64_t        otf_location;
};

#endif /* SILC_INTERNAL_TRACE_TYPES_H */
