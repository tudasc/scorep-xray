/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @status     alpha
 * @file       score.cxx
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Implements the main function of the scorep-score tool.
 */


#include <config.h>
#include <SCOREP_Score_Profile.hpp>
#include <SCOREP_Score_Estimator.hpp>

using namespace std;

void
print_help()
{
    cout << "Usage: scorep-score <profile> [options]" << endl;
    cout << "Options:\n"
         << " -r          Show all regions.\n"
         << " -h          Show this help and exit.\n"
         << " -f <filter> Shows the result with the filter applied.\n"
         << endl;
}

int
main( int argc, char** argv )
{
    string file_name;
    string filter_file;
    bool   show_regions = false;

    //--------------------------------------- Parameter options parsing

    for ( int i = 1; i < argc; i++ )
    {
        // Options start with a dash
        if ( argv[ i ][ 0 ] == '-' )
        {
            if ( argv[ i ][ 1 ] == 'r' )
            {
                show_regions = true;
            }
            else if ( argv[ i ][ 1 ] == 'h' )
            {
                print_help();
                exit( EXIT_SUCCESS );
            }
            else if ( argv[ i ][ 1 ] == 'f' )
            {
                if ( i + 1 < argc )
                {
                    filter_file = argv[ i + 1 ];
                    i++;
                }
                else
                {
                    cerr << "ERROR: No filter file specified." << endl;
                    print_help();
                    exit( EXIT_FAILURE );
                }
            }
            else
            {
                cerr << "ERROR: Unknown argment -" << argv[ i ][ 1 ] << "." << endl;
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
        cerr << "ERROR: No input profile." << endl;
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
        cerr << "ERROR: Can not open Cube report '" << file_name << "'" << endl;
        exit( EXIT_FAILURE );
    }

    SCOREP_Score_Estimator estimator( profile );

    if ( filter_file != "" )
    {
        estimator.InitializeFilter( filter_file );
    }
    estimator.Calculate( show_regions );
    estimator.PrintGroups();

    if ( show_regions )
    {
        estimator.PrintRegions();
    }

    delete ( profile );
    return 0;
}
