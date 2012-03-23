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
#include <SCOREP_Filter.h>

using namespace std;

/* **************************************************************************************
                                                                       internal functions
****************************************************************************************/

static void
scorep_score_swap( SCOREP_Score_Group** items, uint64_t pos1, uint64_t pos2 )
{
    SCOREP_Score_Group* helper = items[ pos1 ];
    items[ pos1 ] = items[ pos2 ];
    items[ pos2 ] = helper;
}

static void
scorep_score_quicksort( SCOREP_Score_Group** items, uint64_t size )
{
    if ( size < 2 )
    {
        return;
    }
    if ( size == 2 )
    {
        if ( items[ 0 ]->GetMaxTBC() < items[ 1 ]->GetMaxTBC() )
        {
            scorep_score_swap( items, 0, 1 );
        }
        return;
    }

    uint64_t beg       = 0;
    uint64_t end       = size - 1;
    uint64_t pos       = size / 2;
    uint64_t threshold = items[ pos ]->GetMaxTBC();

    while ( beg < end )
    {
        while ( ( beg < end ) &&
                ( items[ beg ]->GetMaxTBC() > threshold ) )
        {
            beg++;
        }

        while ( ( beg < end ) &&
                ( items[ end ]->GetMaxTBC() <= threshold ) )
        {
            end--;
        }
        if ( beg < end )
        {
            scorep_score_swap( items, beg, end );

            // Maintain position of our threshold item
            // Needed for special handling of equal values
            if ( beg == pos )
            {
                pos = end;
            }
            else if ( end == pos )
            {
                pos = beg;
            }
        }
    }

    // Special handling if we have many entries with the same value
    // Otherwise,lots of equal values might lead to infinite recursion.
    if ( items[ end ]->GetMaxTBC() < threshold )
    {
        scorep_score_swap( items, end, pos );
    }
    while ( ( end < size ) &&
            ( items[ end ]->GetMaxTBC() == threshold ) )
    {
        end++;
    }

    scorep_score_quicksort( items, beg );
    scorep_score_quicksort( &items[ end ], size - end );
}

/* **************************************************************************************
                                                             class SCOREP_Score_Estimator
****************************************************************************************/

SCOREP_Score_Estimator::SCOREP_Score_Estimator( SCOREP_Score_Profile* profile )
{
    m_profile     = profile;
    m_region_num  = profile->GetNumberOfRegions();
    m_process_num = profile->GetNumberOfProcesses();

    m_has_filter = false;
    m_timestamp  = 8;
    m_dense      = calculate_dense_metric();

    m_enter = calculate_enter();
    m_exit  = calculate_exit();

    m_filtered = NULL;
    m_regions  = NULL;
    m_groups   = ( SCOREP_Score_Group** )malloc( SCOREP_SCORE_TYPE_NUM * sizeof( SCOREP_Score_Group* ) );
    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ] = new SCOREP_Score_Group( i, m_process_num,
                                                SCOREP_Score_GetTypeName( i ) );
    }
}

SCOREP_Score_Estimator::~SCOREP_Score_Estimator()
{
    delete_groups( m_groups, SCOREP_SCORE_TYPE_NUM );
    delete_groups( m_regions, m_region_num );
    delete_groups( m_filtered, SCOREP_SCORE_TYPE_NUM );

    if ( m_has_filter )
    {
        SCOREP_Filter_FreeRules();
    }
}

void
SCOREP_Score_Estimator::delete_groups( SCOREP_Score_Group** groups, uint64_t num )
{
    if ( groups != NULL )
    {
        for ( int i = 0; i < num; i++ )
        {
            delete ( groups[ i ] );
        }
        free( groups );
    }
}

void
SCOREP_Score_Estimator::initialize_regions()
{
    m_regions = ( SCOREP_Score_Group** )malloc( m_region_num * sizeof( SCOREP_Score_Group* ) );
    for ( uint64_t region = 0; region < m_region_num; region++ )
    {
        m_regions[ region ] = new SCOREP_Score_Group( m_profile->GetGroup( region ),
                                                      m_process_num,
                                                      m_profile->GetRegionName( region ) );
    }
}

void
SCOREP_Score_Estimator::InitializeFilter( string filter_file )
{
    SCOREP_Error_Code err = SCOREP_SUCCESS;
    err = SCOREP_Filter_ParseFile( filter_file.c_str() );

    if ( err != SCOREP_SUCCESS )
    {
        cerr << "ERROR:Failed to open '" << filter_file << "'." << endl;
        exit( EXIT_FAILURE );
    }

    m_filtered = ( SCOREP_Score_Group** )
                 malloc( SCOREP_SCORE_TYPE_NUM * sizeof( SCOREP_Score_Group* ) );

    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        string name = SCOREP_Score_GetTypeName( i );
        if ( i != SCOREP_SCORE_TYPE_FLT )
        {
            name += " - FLT";
        }

        m_filtered[ i ] = new SCOREP_Score_Group( i, m_process_num, name );
    }

    m_has_filter = true;
}

void
SCOREP_Score_Estimator::Calculate( bool show_regions )
{
    if ( show_regions )
    {
        initialize_regions();
    }

    for ( uint64_t region = 0; region < m_region_num; region++ )
    {
        uint64_t group = m_profile->GetGroup( region );

        for ( uint64_t process = 0; process < m_process_num; process++ )
        {
            uint64_t tbc    = 0;
            uint64_t visits = m_profile->GetVisits( region, process );
            double   time   = m_profile->GetTime( region, process );

            if ( visits == 0 )
            {
                continue;
            }

            if ( m_profile->HasEnterExit( region ) )
            {
                tbc += visits * ( m_enter + m_exit );
            }

            m_groups[ group ]->AddRegion( tbc, time, process );
            m_groups[ SCOREP_SCORE_TYPE_ALL ]->AddRegion( tbc, time, process );

            if ( show_regions )
            {
                m_regions[ region ]->AddRegion( tbc, time, process );
            }

            if ( m_has_filter )
            {
                bool do_filter
                    = SCOREP_Filter_Match( m_profile->GetFileName( region ).c_str(),
                                           m_profile->GetRegionName( region ).c_str(),
                                           false );
                m_regions[ region ]->DoFilter( do_filter );
                if ( do_filter )
                {
                    m_filtered[ group ]->AddRegion( tbc, time, process );
                    m_filtered[ SCOREP_SCORE_TYPE_ALL ]->AddRegion( tbc, time, process );
                }
                else
                {
                    m_filtered[ SCOREP_SCORE_TYPE_FLT ]->AddRegion( tbc, time, process );
                }
            }
        }
    }
}

void
SCOREP_Score_Estimator::PrintGroups()
{
    double   total_time = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetTotalTime();
    uint64_t max_tbc    = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetMaxTBC();
    uint64_t total_tbc  = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetTotalTBC();

    cout << endl;
    cout << "Estimated aggregate size of event trace (total_tbc):       "
         << total_tbc << " bytes" << endl;
    cout << "Estimated requirements for largest trace buffer (max_tbc): "
         << max_tbc << " bytes" << endl;
    cout << "(hint: When tracing set SCOREP_TOTAL_MEMORY > max_tbc to avoid intermediate flushes\n"
         << " or reduce requirements using file listing names of USR regions to be filtered.)"
         << endl << endl;

    scorep_score_quicksort( m_groups, SCOREP_SCORE_TYPE_NUM );

    cout << "flt type         max_tbc         time      \% region" << endl;
    for ( int i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ]->Print( total_time );
    }
}

void
SCOREP_Score_Estimator::PrintRegions()
{
    scorep_score_quicksort( m_regions, m_region_num );

    double total_time = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetTotalTime();
    cout << endl;
    for ( int i = 0; i < m_region_num; i++ )
    {
        m_regions[ i ]->Print( total_time );
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
    return m_timestamp +                         // timestamp
           get_compressed_size( m_region_num ) + // region handle
           m_dense;                              // metrics
}

uint32_t
SCOREP_Score_Estimator::calculate_exit()
{
    return m_timestamp +                         // timestamp
           get_compressed_size( m_region_num ) + // region handle
           m_dense;                              // metrics
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
