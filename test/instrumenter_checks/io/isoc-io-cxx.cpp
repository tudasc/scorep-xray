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

#include <iostream>
#include <fstream>

#include <stdio.h>

using namespace std;

#define TEST_FILE_NAME   "__my_output_file__"

int
main( int argc, char* argv[] )
{
    ofstream output_file;                 /* Output file */

    output_file.open( TEST_FILE_NAME );
    output_file << "my string to test file output\n";
    output_file.close();

    remove( TEST_FILE_NAME );

    return 0;
}
