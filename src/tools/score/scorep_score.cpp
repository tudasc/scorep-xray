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
         << " -r   Show all regions.\n"
         << " -h   Show this help and exit.\n"
         << endl;
}

int
main( int argc, char** argv )
{
    string file_name;
    bool   show_regions = false;

    //--------------------------------------- Parameter options parsing

    for ( int i = 1; i < argc; i++ )
    {
        // Options start with a dash
        if ( argv[ i ][ 0 ] == '-' )
        {
            for ( int j = 1; argv[ i ][ j ] != '\0'; j++ )
            {
                if ( argv[ i ][ j ] == 'r' )
                {
                    show_regions = true;
                }
                else if ( argv[ i ][ j ] == 'h' )
                {
                    print_help();
                    exit( EXIT_SUCCESS );
                }
                else
                {
                    cerr << "ERROR: Unknown argment -" << argv[ i ][ j ] << "." << endl;
                    print_help();
                    exit( EXIT_FAILURE );
                }
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

    estimator.Calculate( show_regions );
    estimator.PrintGroups();

    if ( show_regions )
    {
        estimator.PrintRegions();
    }

    delete ( profile );
    return 0;
}
