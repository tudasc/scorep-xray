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
 * @ file SILC_compiler_pgi.c
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @brief provided support for PGI-compiler profiling for 7.x and 8.x compilers.
 * Note: The PGI9.x compiler version uses GNU instrument function methods to provide
 * function enter and exit events.
 */

#include "stdio.h"
#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

/**
 * @brief Container structure to map profiling informations, like function names
 * and region handles
 */


/**
 * @brief Data structure to be used by the PGI compiler
 */
struct s1
{
    uint64_t          l1;
    uint64_t          l2;
    double            d1;
    double            d2;
    uint32_t          isseen;
    char*             c;
    void*             p1;
    SILC_LineNo       lineno;
    void*             p2;
    struct s1*        p3;
    uint32_t          fid;  /* function id */
    SILC_RegionHandle rid;  /* routine id */
    char*             file; /* file name */
    char*             rout; /* routine name */
};

uint64_t count = 0;

/**
 * static variable to control initialize status of adapter
 */
static int silc_compiler_initialize = 1;

/**
 * data structure to map function name and region identifier
 */
typedef struct HashNode HashNode;


/**
 * called during program initialization
 */
void
__rouinit
(
)
{
    printf( " PGI routine init \n " );


    if ( silc_compiler_initialize )
    {
        SILC_InitMeasurement();
    }
}


/**
 * called during program termination
 */

void
__rouexit
(
)
{
    printf( " PGI routine init exit event \n " );
}





/**
 * called at the beginning of each profiled routine
 */
void
___rouent2
(
    struct s1* p
)
{
    HashNode hn;

    printf( " begin of a profiled routine 2 \n " );

    if ( silc_compiler_initialize )
    {
        SILC_InitMeasurement();
    }

    if ( !p->isseen )
    {
        /* get the file name from instrumentation */

        /* get file id beloning to file name */

        p->rid = SILC_DefineRegion( p->rout,
                                    SILC_INVALID_SOURCE_FILE,
                                    SILC_INVALID_LINE_NO,
                                    SILC_INVALID_LINE_NO,
                                    SILC_ADAPTER_COMPILER,
                                    SILC_REGION_FUNCTION
                                    );
        p->isseen = 1;
    }
}

/**
 * called at the beginning of each profiled routine
 */
void
___rouent64
(
    struct s1* p
)
{
    printf( " begin of a profiled routine for 64bit systems \n " );
}

/**
 * called at the end of each profiled routine
 */
void
___rouret
(
    void
)
{
    printf( " end of a profiled routine \n " );
}

/**
 * called at the end of each profiled routine
 */
void
___rouret2
(
    void
)
{
    printf( " end of a profiled routine 2 \n " );
}


void
___linent2
(
    void
)
{
}

SILC_Error_Code
silc_compiler_init_adapter()
{
    if ( silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " inititialize PGI compiler adapter!" );

        /* Sez flag */
        silc_compiler_initialize = 0;
    }

    return SILC_SUCCESS;
}

void
silc_compiler_finalize()
{
    /* call only, if previously initialized */
    if ( !silc_compiler_initialize )
    {
        silc_compiler_initialize = 1;
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize PGI compiler adapter!" );
    }
}
