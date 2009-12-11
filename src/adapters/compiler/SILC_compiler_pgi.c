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


typedef struct HashNode
{
    long             id;
    const char*      name;
    const char*      fname;
    int              lnobegin;
    int              lnoend;
    int              reghandle;
    struct HashNode* next;
} HashNode;

#define HASH_MAX 1024
static HashNode* htab[ HASH_MAX ];

unsigned int     count = 0;


/**
 * static variable to control initialize status of GNU
 */
static init routineInit = 1;

static void
hash_put
(
    long        id,
    const char* functionName,
    const char* routineName,
    int         lineNumber
)
{
    long identifier = id % HASH_MAX;
    printf( " PUT hash id %ld \n", identifier );

    HashNode* add = ( HashNode* )malloc( sizeof( HashNode ) );
    add->id            = id;
    add->name          = routineName;
    add->fname         = functionName ? ( const char* )strdup( functionName ) : functionName;
    add->lnobegin      = lineNumber;
    add->lnoend        = 0;
    add->reghandle     = 0; /*     add->reghandle = SILC_INVALID_REGION; */
    add->next          = htab[ identifier ];
    htab[ identifier ] = add;
}

static HashNode*
hash_get( long h )
{
    long identifier = h % HASH_MAX;
    printf( " GET  hash id %ld \n", identifier );

    HashNode* curr = htab[ identifier ];
    while ( curr )
    {
        if ( curr->id == h )
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}



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

        if ( ( hn = hash_get( ( long )func ) ) )
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
