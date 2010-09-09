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
 * @status alpha
 *
 * @maintainer Johannes Spazier <johannes.spazier@tu-dresden.de>
 * @autors     Johannes Spazier <johannes.spazier@tu-dresden.de>
 *
 */

#include <config.h>

#include <stdio.h>
#include <string.h>

#define MODE_SEQ 0
#define MODE_OMP 1
#define MODE_MPI 2
#define MODE_HYB 3

#define ACTION_LIBS 1
#define ACTION_CFLAGS 2
#define ACTION_CC 3
#define ACTION_CXX 4
#define ACTION_PREFIX 5

#define HELPTEXT "\nUsage: silc_config [--seq|--omp|--mpi|--hyb] (--cflags|--libs|--cc|--cxx|--prefix)\n"

int
main( int    argc,
      char** argv )
{
    int   i;
    /* set default mode to mpi */
    int   mode   = MODE_MPI;
    int   action = 0;

    char* silc_libs[ 4 ] = { "silc_serial", "silc_omp", "silc_mpi", "silc_mpi_omp" };

    /* parsing the command line */
    for ( i = 1; i < argc; i++ )
    {
        if ( strcmp( argv[ i ], "--help" ) == 0 || strcmp( argv[ i ], "-h" ) == 0 )
        {
            printf( "%s\n", HELPTEXT );
        }
        else if ( strcmp( argv[ i ], "--seq" ) == 0 )
        {
            mode = MODE_SEQ;
        }
        else if ( strcmp( argv[ i ], "--omp" ) == 0 )
        {
            mode = MODE_OMP;
        }
        else if ( strcmp( argv[ i ], "--mpi" ) == 0 )
        {
            mode = MODE_MPI;
        }
        else if ( strcmp( argv[ i ], "--hyb" ) == 0 )
        {
            mode = MODE_HYB;
        }
        else if ( strcmp( argv[ i ], "--libs" ) == 0 )
        {
            action = ACTION_LIBS;
        }
        else if ( strcmp( argv[ i ], "--cflags" ) == 0 )
        {
            action = ACTION_CFLAGS;
        }
        else if ( strcmp( argv[ i ], "--cc" ) == 0 )
        {
            action = ACTION_CC;
        }
        else if ( strcmp( argv[ i ], "--cxx" ) == 0 )
        {
            action = ACTION_CXX;
        }
        else if ( strcmp( argv[ i ], "--prefix" ) == 0 )
        {
            action = ACTION_PREFIX;
        }
        else
        {
            printf( "Unknown option %s. Ignore.\n", argv[ i ] );
        }
    }

    switch ( action )
    {
        case ACTION_LIBS:
            printf( "-L%s -l%s %s -lotf2 -lsilc_utilities\n", LIBDIR, silc_libs[ mode ], LIBBFD );

            break;

        case ACTION_CFLAGS:
            printf( "%s -I%s\n", INST_CPPFLAGS, INCDIR );

            break;

        case ACTION_CC:
            printf( "%s\n", CC );

            break;

        case ACTION_CXX:
            printf( "%s\n", CXX );

            break;

        case ACTION_PREFIX:
            printf( "%s\n", PREFIX );

            break;

        default:
            printf( "%s\n", HELPTEXT );

            break;
    }

    return 0;
}
