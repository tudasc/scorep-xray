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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TEST_FILE_NAME   "__my_output_file__"
#define TEST_STRING      "my string to test file output\n"

int
main( int argc, char* argv[] )
{
    int    output_fd; /* Output file descriptors */
    size_t ret_out;   /* Number of bytes returned by write() operation */

    /* Open output file */
    output_fd = open( TEST_FILE_NAME, O_WRONLY | O_CREAT, 0644 );
    if ( output_fd == -1 )
    {
        return -1;
    }

    /* Write data to file */
    ret_out = write( output_fd, TEST_STRING, strlen( TEST_STRING ) );
    if ( ret_out != strlen( TEST_STRING ) )
    {
        /* Write error */
        return -2;
    }

    /* Close file descriptors */
    close( output_fd );

    /* Remove file */
    unlink( TEST_FILE_NAME );

    return 0;
}
