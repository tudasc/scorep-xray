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
 * @file       scorep_error_test.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <SCOREP_Error.h>


int
main()
{
    /* Check that the error codes do not overlap with static constants.
       Should avoid that someone who adds error codes creates such conflicts. */
    extern uint64_t
    MANGLE_NAME( error_reverse_lookup_size );

    int64_t last_code = MANGLE_NAME( ERROR_CODE_OFFSET ) + MANGLE_NAME( error_reverse_lookup_size );
    if ( last_code >= MANGLE_NAME( SUCCESS ) )
    {
        printf( "Error code conflicts with static constant MANGLE_NAME( SUCCESS ). The offset\n"
                " is %d. The number of error codes is %" PRIu64 ".\n",
                MANGLE_NAME( ERROR_CODE_OFFSET ), MANGLE_NAME( error_reverse_lookup_size ) );
        return EXIT_FAILURE;
    }

    printf( "================================================================\n"
            "The following error test will print error messages in order\n"
            "to test whether the error system works correctly. If we suppress\n"
            "this messages we would no longer see, if a segmentation fault\n"
            "occures.\n"
            "================================================================\n"
            );
    errno = 0;
    MANGLE_NAME( ERROR_POSIX ) ( "This error message is no real error: Success" );
    errno = ENOMEM;
    MANGLE_NAME( ERROR_POSIX ) ( "This error message is no real error: Valid errno" );
    errno = 1024;
    MANGLE_NAME( ERROR_POSIX ) ( "This error message is no real error: Unknown POSIX" );
    errno = -1024;
    MANGLE_NAME( ERROR_POSIX ) ( "This error message is no real error: Unknown POSIX II" );
    OTF2_ERROR( MANGLE_NAME( SUCCESS ),
                "This error message is no real error: MANGLE_NAME( SUCCESS )" );
    OTF2_ERROR( MANGLE_NAME( ERROR_END_OF_FUNCTION ),
                "This error message is no real error: MANGLE_NAME( ERROR_END_OF_FUNCTION )" );
    OTF2_ERROR( 1024,
                "This error message is no real error: error above MANGLE_NAME( SUCCESS )" );
    OTF2_ERROR( -1024,
                "This error message is no real error: error below valid errors" );

    printf( "================================================================\n"
            "End of error tests.\n"
            "================================================================\n"
            );
    return 0;
}
