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
 *
 * @brief provided support for PGI-profiling
 */

#include "stdio.h"
#include <SILC_Utils.h>

struct s1
{
    uint64_t   l1;
    uint64_t   l2;
    double     d1;
    double     d2;
    uint32_t   isseen;
    char*      c;
    void*      p1;
    uint32_t   lineno;
    void*      p2;
    struct s1* p3;
    uint32_t   fid;
    uint32_t   rid;
    char*      file;
    char*      rout;
};


/**
 * called during program initialization
 */

void
__rouinit
(
)
{
    printf( " PGI routine init enter event \n " );
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
