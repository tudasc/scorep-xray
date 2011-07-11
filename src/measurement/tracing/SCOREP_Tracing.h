#ifndef SCOREP_TRACING_H
#define SCOREP_TRACING_H

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


/**
 * @file       SCOREP_Tracing.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <otf2/OTF2_GeneralDefinitions.h>
#include <stdint.h>


OTF2_FileSubstrate
SCOREP_Tracing_GetFileSubstrate();


uint64_t
SCOREP_Tracing_GetChunkSize();


void
SCOREP_Tracing_RegisterSionCallbacks();


#endif /* SCOREP_TRACING_H */
