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


/* For the SCOREP_Tracing_GetFileSubstrate() see paradigm/scorep_sion_*.c */


OTF2_Compression
SCOREP_Tracing_GetCompression()
{
    if ( SCOREP_Env_CompressTraces() )
    {
        return OTF2_COMPRESSION_ZLIB;
    }
    else
    {
        return OTF2_COMPRESSION_NONE;
    }
}


uint64_t
SCOREP_Tracing_GetChunkSize()
{
    /** @todo croessel in OTF2_Archive_New we need to specify an event
        chunk-size and a definition chunk size. the chnunk size need to be
        larger than the largest item that is written. events are relatively
        small whereas some definition record grow with the number of
        processes. We nee two environment variable here. does the event chunk
        size equal the memory page size (scorep_env_page_size)? */
    return 1024 * 1024; // 1MB
}
