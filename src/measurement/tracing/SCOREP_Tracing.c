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
 * @file       SCOREP_Tracing.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "SCOREP_Tracing.h"
#include <scorep_environment.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */

OTF2_FileSubstrate
SCOREP_Tracing_GetFileSubstrate()
{
    OTF2_FileSubstrate substrate = OTF2_SUBSTRATE_POSIX;
    if ( SCOREP_Env_UseSionSubstrate() )
    {
        #if HAVE( SION )
        substrate = OTF2_SUBSTRATE_SION;
        #endif
    }
    return substrate;
}


uint64_t
SCOREP_Tracing_GetOTF2ChunkSize()
{
    return 1024 * 1024; // 1MB
}
