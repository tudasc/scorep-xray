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
 * Copyright (c) 2009-2013, 2017, 2019-2020,
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
    string usage =
        #include "scorep_score_usage.h"
    ;
    cout << usage.c_str() << endl;
    cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">" << endl;
}

int
main( int    argc,
      char** argv )
{
    string                   file_name;
    string                   filter_file;
    int64_t                  dense_num                   = 0;
    bool                     show_regions                = false;
    bool                     use_mangled                 = false;
    SCOREP_Score_SortingType sortingby                   = SCOREP_SCORE_SORTING_TYPE_MAXBUFFER;
    bool                     produce_initial_filter_file = false;
    bool                     filter_file_options_set     = false;
    double                   min_percentage_from_max_buf = 1;
    double                   max_time_per_visit          = 1;
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
            else if ( arg == "-g" )
            {
                produce_initial_filter_file = true;
                // force mangled names when generation is active
                // use_mangled = true;
            }
            else if ( arg == "-b" )
            {
                if ( i + 1 < argc )
                {
                    char* p;
                    min_percentage_from_max_buf = strtod( argv[ i + 1 ], &p );
                    if ( *p )
                    {
                        cerr << "ERROR: Parameter value for buffer percentage is not a number!" << endl;
                        exit( EXIT_FAILURE );
                    }
                    if ( min_percentage_from_max_buf < 0 || min_percentage_from_max_buf > 100 )
                    {
                        cerr << "ERROR: The buffer percentage has to be in the range 0-100!" << endl;
                        exit( EXIT_FAILURE );
                    }
                    i++;
                }
                else
                {
                    cerr << "ERROR: Missing parameter value for the buffer percent!" << endl;
                    print_help();
                    exit( EXIT_FAILURE );
                }
                filter_file_options_set = true;
            }
            else if ( arg == "-t" )
            {
                if ( i + 1 < argc )
                {
                    char* p;
                    max_time_per_visit = strtod( argv[ i + 1 ], &p );
                    if ( *p )
                    {
                        cerr << "ERROR: Parameter value for max time per visit is not a number!" << endl;
                        exit( EXIT_FAILURE );
                    }
                    if ( max_time_per_visit < 0 )
                    {
                        cerr << "ERROR: The max time per visit has to be positive!" << endl;
                        exit( EXIT_FAILURE );
                    }
                    i++;
                }
                else
                {
                    cerr << "ERROR: Missing parameter value for max time per visit!" << endl;
                    print_help();
                    exit( EXIT_FAILURE );
                }
                filter_file_options_set = true;
            }
            else if ( arg == "-s" )
            {
                if ( i + 1 < argc )
                {
                    std::string choice = argv[ i + 1 ];
                    if ( choice == "totaltime" )
                    {
                        sortingby = SCOREP_SCORE_SORTING_TYPE_TOTALTIME;
                    }
                    else if ( choice == "timepervisit" )
                    {
                        sortingby = SCOREP_SCORE_SORTING_TYPE_TIMEPERVISIT;
                    }
                    else if ( choice == "maxbuffer" )
                    {
                        sortingby = SCOREP_SCORE_SORTING_TYPE_MAXBUFFER;
                    }
                    else if ( choice == "visits" )
                    {
                        sortingby = SCOREP_SCORE_SORTING_TYPE_VISITS;
                    }
                    else if ( choice == "name" )
                    {
                        sortingby = SCOREP_SCORE_SORTING_TYPE_NAME;
                    }
                    else
                    {
                        cerr << "ERROR: Unknown sorting choice" << endl;
                        print_help();
                        exit( EXIT_FAILURE );
                    }
                    i++;
                }
                else
                {
                    cerr << "ERROR: No sorting mode specified" << endl;
                    print_help();
                    exit( EXIT_FAILURE );
                }
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
    cube::Cube*           cube;
    try
    {
        cube = new cube::Cube();
        cube->openCubeReport( file_name );
        profile = new SCOREP_Score_Profile( cube );
    }
    catch ( ... )
    {
        cerr << "ERROR: Cannot open Cube report '" << file_name << "'" << endl;
        exit( EXIT_FAILURE );
    }

    if ( filter_file_options_set && !produce_initial_filter_file )
    {
        cerr << "WARNING: Options to configure the filter file generation (-t or -b) are set.\n"
             << "         Without enabling the generation (-g) these will have no effect!" << endl;
    }

    SCOREP_Score_Estimator estimator( profile, dense_num, sortingby );

    if ( filter_file != "" )
    {
        estimator.initializeFilter( filter_file );
    }
    estimator.calculate( show_regions || produce_initial_filter_file, use_mangled );
    estimator.printGroups();

    if ( show_regions )
    {
        estimator.printRegions();
    }

    if ( produce_initial_filter_file )
    {
        estimator.generateFilterFile( min_percentage_from_max_buf,
                                      max_time_per_visit );
    }

    delete ( profile );
    delete ( cube );
    return 0;
}
