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
 * static variable to control initialize status of GNU
 */
static init routineInit = 1;

/**
 * data structure to map function name and region identifier
 */
typedef struct HashNode HN;


/**
 * called during program initialization
 */
void
__rouinit
(
)
{
    printf( " PGI routine init \n " );


    if ( rou_init )
    {
        SILC_InitMeasurement();

        routineInit = 0;

        silc_comp_finalize = &__rouexit;
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
    printf( " begin of a profiled routine 2 \n " );

    if ( routineInit )
    {
        __rouinit();
        routineInit = 0;
    }

    if ( !p->isseen )
    {
        /* get the file name from instrumentation */

        /* get file id beloning to file name */

        if ( ( HN = hash_get( ( long )func ) ) )
        {
            p->fid = 1;
        }
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
