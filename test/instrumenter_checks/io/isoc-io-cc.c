/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <stdio.h>

#define TEST_FILE_NAME   "__my_output_file__"
#define TEST_STRING      "my string to test file output\n"

int
main( int argc, char* argv[] )
{
    FILE* output_file;                    /* Output file */
    char  buffer[] = { 'f', 'o', 'o' };   /* Test data */

    /* Open output file */
    output_file = fopen( TEST_FILE_NAME, "wb" );
    if ( output_file == NULL )
    {
        return -1;
    }

    /* Write data to file */
    fwrite( buffer, sizeof( char ), sizeof( buffer ), output_file );

    /* Close file descriptors */
    fclose( output_file );

    remove( TEST_FILE_NAME );

    return 0;
}
