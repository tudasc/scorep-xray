/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief      Implements the main function of the scorep-score tool.
 */


#include <config.h>
#include "SCOREP_Score_Profile.hpp"
#include "SCOREP_Score_Estimator.hpp"

using namespace std;

void
print_help()
{
    cout << "Usage: scorep-score <profile> [options]" << endl;
    cout << "Options:\n"
         << " -r          Show all regions.\n"
         << " -h, --help  Show this help and exit.\n"
         << " -f <filter> Shows the result with the filter applied.\n"
         << " -c <num>    Specifes the number of hardware counters that shall be measured.\n"
         << "             By default, this value is 0, which means that only a timestamp\n"
         << "             is measured on each event. If you plan to record hardware counters\n"
         << "             specify the number of hardware counters. Otherwise, scorep-score\n"
         << "             may underestimate the required space.\n"
         << " -m          Prints mangled region names instead of demangled names."
         << endl;
}

int
main( int argc, char** argv )
{
    string   file_name;
    string   filter_file;
    uint64_t dense_num    = 0;
    bool     show_regions = false;
    bool     use_mangled  = false;

    //--------------------------------------- Parameter options parsing

    for ( int i = 1; i < argc; i++ )
    {
        string arg = argv[ i ];

        // Options start with a dash
        if ( argv[ i ][ 0 ] == '-' )
        {
            if ( arg == "-r" )
            {
                show_regions = true;
            }
            else if ( ( arg == "-h" ) ||
                      ( arg == "--help" ) )
            {
                print_help();
                exit( EXIT_SUCCESS );
            }
            else if ( arg == "-f" )
            {
                if ( i + 1 < argc )
                {
                    filter_file = argv[ i + 1 ];
                    i++;
                }
                else
                {
                    cerr << "ERROR: No filter file specified" << endl;
                    print_help();
                    exit( EXIT_FAILURE );
                }
            }
            else if ( arg == "-c" )
            {
                if ( i + 1 < argc )
                {
                    dense_num = atoi( argv[ i + 1 ] );
                    i++;
                }
                else
                {
                    cerr << "ERROR: Missing number of hardware counters" << endl;
                    print_help();
                    exit( EXIT_FAILURE );
                }
            }
            else if ( arg == "-m" )
            {
                use_mangled = true;
            }
            else
            {
                cerr << "ERROR: Unknown argment: '" << arg << "'" << endl;
                print_help();
                exit( EXIT_FAILURE );
            }
        }

        // Everything without a dash is a input file name.
        else
        {
            file_name = argv[ i ];
        }
    }

    if ( file_name == "" )
    {
        cerr << "ERROR: No input profile" << endl;
        print_help();
        exit( EXIT_FAILURE );
    }

    //-------------------------------------- Sanity checks
    if ( dense_num < 0 )
    {
        cerr << "ERROR: The number of hardware counters cannot be less than zero: "
             << "'" << dense_num << "'" << endl;
        print_help();
        exit( EXIT_FAILURE );
    }

    //-------------------------------------- Scoreing

    SCOREP_Score_Profile* profile;

    try
    {
        profile = new SCOREP_Score_Profile( file_name );
    }
    catch ( ... )
    {
        cerr << "ERROR: Cannot open Cube report '" << file_name << "'" << endl;
        exit( EXIT_FAILURE );
    }

    SCOREP_Score_Estimator estimator( profile, dense_num );

    if ( filter_file != "" )
    {
        estimator.initializeFilter( filter_file );
    }
    estimator.calculate( show_regions, use_mangled );
    estimator.printGroups();

    if ( show_regions )
    {
        estimator.printRegions();
    }

    delete ( profile );
    return 0;
}
