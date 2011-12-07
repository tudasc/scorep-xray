/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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


/**
 * @file       src/trunk/src/measurement/paradigm/scorep_sion_not_supported.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include <SCOREP_Tracing.h>


void
SCOREP_Tracing_RegisterSionCallbacks( OTF2_Archive* archive )
{
    // don't register anything if sion is not supported.
}


OTF2_FileSubstrate
SCOREP_Tracing_GetFileSubstrate()
{
    return OTF2_SUBSTRATE_POSIX;
}
