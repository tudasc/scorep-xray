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
 * @ file      SILC_compiler_ftrace.c
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @brief Compiler adapter version for NEC SX compiler
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <SILC_Compiler_Data.h>


extern void*
silc_ftrace_getname( void );
extern int
silc_ftrace_getname_len( void );


static uint32_t initSX = 1;

/**
 * data structure to map function name and region identifier
 */
typedef struct HashNode HNsx;


void
ftrace_finalize( void );
void
_ftrace_enter2_( void );
void
_ftrace_exit2_( void );
void
_ftrace_stop2_( void );


void
ftrace_finalize()
{
    printf( " ftrace finalize \n" );

    hash_free();

    initSX = 1;
}




void
_ftrace_enter2_()
{
    char* func = silc_ftrace_getname();
    int   len  = silc_ftrace_getname_len();

    HNsx* hn = hash_get( ( long )func );

    if ( initSX )
    {
        /* not initialized so far */
        SILC_InitMeasurement();

        ftrace_finalize();
        initSX = 0;     /* is initialized */
    }



    printf( " function name: %s \n", func );
    printf( " function length: %i \n", len );

    if ( !hn )
    {
        hash_put( ( long )func, func, "", len );
    }

    if ( ( hn = hash_get( ( long )func ) ) )
    {
        if ( hn->reghandle == SILC_INVALID_REGION )
        {
            /* -- region entered the first time, register region -- */
            silc_compiler_register_region( hn );
        }
        fprintf( stderr, "enter the region with handle %i \n", hn->reghandle );
        SILC_EnterRegion( hn->reghandle );
    }
}


void
_ftrace_exit2_()
{
    char* func = silc_ftrace_getname();
    long  id   = ( long )func;
    HNsx* hn;

    frintf( stderr, "call function exit!!!\n" );
    frintf( stderr, " ftrace exit 2 \t %i \n", ( long )func );
    if ( hn = hash_get( ( long )func ) )
    {
        SILC_ExitRegion( hn->reghandle );
    }
}

void
_ftrace_stop2_()
{
    printf( " ftrace stop 2 \n" );
}
