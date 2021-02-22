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
 * Copyright (c) 2009-2013, 2017, 2019-2021,
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
#include <scorep_tools_utils.hpp>

using namespace std;

void
exit_fail()
{
    cout <<     "\nUsage:\nscorep-score [options] <profile>\n\n"
         <<     "To print out more detailed help information on available parameters, type\n"
         <<     "scorep-score --help\n" << endl;
    exit( EXIT_FAILURE );
}

void
print_help()
{
    string usage =
        #include "scorep_score_usage.h"
    ;
    cout << usage.c_str() << endl;
    cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">" << endl;
    exit( EXIT_SUCCESS );
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
    // default options for automatic selection
    double min_percentage_from_max_buf = 1;
    double max_time_per_visit          = 1;
    bool   filter_usr                  = true;
    // optional manual option for more fine control
    uint64_t min_visits           = 0;
    double   min_max_buf_absolute = 0;
    bool     filter_com           = false;

    //--------------------------------------- Parameter options parsing

    // if 'help' is part of the parameters or there are no parameters, print
    // usage info and exit
    bool get_help = false;
    for ( int i = 1; i < argc; i++ )
    {
        string arg = argv[ i ];
        if ( ( arg == "-h" ) || ( arg == "--help" ) )
        {
            get_help = true;
            break;
        }
    }
    if ( argc == 1 || get_help )
    {
        print_help();
    }

    // all other cases require a cube file and that it is the last parameter
    file_name = argv[ argc - 1 ];
    if ( !exists_file( file_name ) )
    {
        cerr << "ERROR: Input file '" << file_name << "' does not exist!" << endl;
        if ( file_name[ 0 ] == '-' )
        {
            cerr << "       Note, '-' parameters other than '-h' require a cube file as input\n"
                 << "       and the file has to be the last parameter of the command line." << endl;
        }
        exit_fail();
    }
    argc--;

    for ( int i = 1; i < argc; i++ )
    {
        string arg = argv[ i ];

        if ( arg == "-r" )
        {
            show_regions = true;
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
                exit_fail();
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
                exit_fail();
            }
        }
        else if ( arg == "-m" )
        {
            use_mangled = true;
        }
        else if ( arg == "-g" )
        {
            produce_initial_filter_file = true;

            // check if there is optional parameter for this option
            if ( ( i + 1 < argc ) && ( argv[ i + 1 ][ 0 ] != '-' ) )
            {
                // parse optional control parameters for filter generation
                vector <string> arguments;
                stringstream    tokenize( argv[ i + 1 ] );
                i++;
                string token;
                while ( getline( tokenize, token, ',' ) )
                {
                    // simplify token string to allow for common typing errors
                    string original_token = token;
                    token = scorep_tolower( token );
                    token = replace_all( " ", "", token );
                    if ( token.length() == 0 )
                    {
                        continue;
                    }
                    std::string::size_type pos = token.find( "=" );
                    if ( pos == std::string::npos )
                    {
                        cerr << "ERROR: Invalid filter generation option:\"" << original_token << "\"" << endl;
                        exit_fail();
                    }
                    else
                    {
                        string key   = token.substr( 0, pos );
                        string value = token.substr( pos + 1, token.length() - pos - 1 );
                        if ( key == "bufferpercent" )
                        {
                            char* p;
                            min_percentage_from_max_buf = strtod( value.c_str(), &p );
                            if ( *p )
                            {
                                cerr << "ERROR: Parameter value for buffer percentage is not a number!" << endl;
                                exit_fail();
                            }
                            if ( min_percentage_from_max_buf < 0 || min_percentage_from_max_buf > 100 )
                            {
                                cerr << "ERROR: The buffer percentage has to be in the range 0-100!" << endl;
                                exit_fail();
                            }
                        }
                        else if (  key == "timepervisit" )
                        {
                            char* p;
                            max_time_per_visit = strtod( value.c_str(), &p );
                            if ( *p )
                            {
                                cerr << "ERROR: Parameter value for max time per visit is not a number!" << endl;
                                exit_fail();
                            }
                            if ( max_time_per_visit < 0 )
                            {
                                cerr << "ERROR: The max time per visit has to be positive!" << endl;
                                exit_fail();
                            }
                        }
                        else if (  key == "visits" )
                        {
                            char* p;
                            min_visits = strtoul( value.c_str(), &p, 10 );
                            if ( *p )
                            {
                                cerr << "ERROR: Parameter value for visits is not a number!" << endl;
                                exit_fail();
                            }
                            if ( min_visits < 0 )
                            {
                                cerr << "ERROR: The visits count has to be positive!" << endl;
                                exit_fail();
                            }
                        }
                        else if ( key == "bufferabsolute" )
                        {
                            char* p;
                            min_max_buf_absolute = strtod( value.c_str(), &p );
                            if ( *p )
                            {
                                cerr << "ERROR: Parameter value for absolute minimum buffer is not a number!" << endl;
                                exit_fail();
                            }
                            if ( min_max_buf_absolute < 0 )
                            {
                                cerr << "ERROR: The buffer value has to be larger than 0!" << endl;
                                exit_fail();
                            }
                        }
                        else if ( key == "type" )
                        {
                            if ( value == "usr" )
                            {
                                // the default case
                                filter_usr = true;
                                filter_com = false;
                            }
                            else if ( value == "com" )
                            {
                                filter_usr = false;
                                filter_com = true;
                            }
                            else if ( value == "both" )
                            {
                                filter_usr = true;
                                filter_com = true;
                            }
                            else
                            {
                                cerr << "ERROR: Invalid filter generation option:\"" << original_token << "\"" << endl;
                                exit_fail();
                            }
                        }
                        else
                        {
                            cerr << "ERROR: Invalid filter generation option:\"" << original_token << "\"" << endl;
                            exit_fail();
                        }
                    }
                }
            }
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
                    exit_fail();
                }
                i++;
            }
            else
            {
                cerr << "ERROR: No sorting mode specified" << endl;
                exit_fail();
            }
        }
        else
        {
            cerr << "ERROR: Unknown argument: '" << arg << "'" << endl;
            if ( exists_file( arg ) && ( get_extension( arg ) == ".cubex" ) )
            {
                cerr << "       Note, the input file has to be the last parameter of the command line." << endl;
            }
            exit_fail();
        }
    }

    //-------------------------------------- Sanity checks
    if ( dense_num < 0 )
    {
        cerr << "ERROR: The number of hardware counters cannot be less than zero: "
             << "'" << dense_num << "'" << endl;
        exit_fail();
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
        exit_fail();
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
                                      max_time_per_visit,
                                      min_visits,
                                      min_max_buf_absolute,
                                      filter_usr,
                                      filter_com );
    }

    delete ( profile );
    delete ( cube );
    return 0;
}
