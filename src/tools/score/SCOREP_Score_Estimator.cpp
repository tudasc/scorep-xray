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
 * @file       SCOREP_Score_Estimator.cxx
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Implements a class which performs calculations for trace
 *             size estimation.
 */

#include <config.h>
#include <math.h>
#include <SCOREP_Score_Estimator.hpp>
#include <SCOREP_Score_Types.hpp>

using namespace std;

SCOREP_Score_Estimator::SCOREP_Score_Estimator( SCOREP_Score_Profile* profile )
{
    m_profile = profile;
    m_regions = profile->GetNumberOfRegions();
    m_threads = profile->GetNumberOfThreads();

    m_timestamp = 8;
    m_dense     = calculate_dense_metric();

    m_enter = calculate_enter();
    m_exit  = calculate_exit();

    m_groups = ( SCOREP_Score_Group** )malloc( SCOREP_SCORE_GROUP_NUM * sizeof( SCOREP_Score_Group* ) );
    for ( int i = 0; i < SCOREP_SCORE_GROUP_NUM; i++ )
    {
        m_groups[ i ] = new SCOREP_Score_Group( ( SCOREP_Score_GroupId )i, m_threads );
    }
}

SCOREP_Score_Estimator::~SCOREP_Score_Estimator()
{
    for ( int i = 0; i < SCOREP_SCORE_GROUP_NUM; i++ )
    {
        delete ( m_groups[ i ] );
    }
    free( m_groups );
}

void
SCOREP_Score_Estimator::CalculateGroups()
{
    for ( uint64_t region = 0; region < m_regions; region++ )
    {
        SCOREP_Score_GroupId group = m_profile->GetGroup( region );

        for ( uint64_t thread = 0; thread < m_threads; thread++ )
        {
            uint64_t tbc    = 0;
            uint64_t visits = m_profile->GetVisits( region, thread );

            if ( visits == 0 )
            {
                continue;
            }

            if ( m_profile->HasEnterExit( region ) )
            {
                tbc += visits * ( m_enter + m_exit );
            }

            m_groups[ group ]->AddRegion( tbc, m_profile->GetTime( region, thread ), thread );
            m_groups[ SCOREP_SCORE_GROUP_ALL ]->AddRegion( tbc, m_profile->GetTime( region, thread ), thread );
        }
    }
}

void
SCOREP_Score_Estimator::Print()
{
    double total_time = m_groups[ SCOREP_SCORE_GROUP_ALL ]->GetTotalTime();

    cout << "flt \t type \t max_tbc \t time \t \% \t region" << endl;
    for ( int i = 0; i < SCOREP_SCORE_GROUP_NUM; i++ )
    {
        m_groups[ i ]->Print( total_time );
    }
}

void
SCOREP_Score_Estimator::DumpEventSizes()
{
    cout << "Dense metrics: " << m_dense << endl;
    cout << "Enter:         " << m_enter << endl;
    cout << "Exit:          " << m_exit  << endl;
}

uint32_t
SCOREP_Score_Estimator::calculate_enter()
{
    return m_timestamp +                      // timestamp
           get_compressed_size( m_regions ) + // region handle
           m_dense;                           // metrics
}

uint32_t
SCOREP_Score_Estimator::calculate_exit()
{
    return m_timestamp +                      // timestamp
           get_compressed_size( m_regions ) + // region handle
           m_dense;                           // metrics
}

uint32_t
SCOREP_Score_Estimator::calculate_dense_metric()
{
    // How to determine the number of dense metricss ?
    uint32_t number = 0;

    if ( number == 0 )
    {
        return 0;
    }
    return get_compressed_size( m_profile->GetNumberOfMetrics() ) + // handle
           1 +                                                      // number
           number +                                                 // type ids
           number * 8;                                              // values
}

uint32_t
SCOREP_Score_Estimator::get_compressed_size( uint64_t max_value )
{
    return ( uint32_t )1 +                  // Number of leading zeros
           ceil( log10( max_value ) );      // Number
}
