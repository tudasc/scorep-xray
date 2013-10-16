/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file       SCOREP_Score_Estimator.cpp
 *
 * @brief      Implements a class which performs calculations for trace
 *             size estimation.
 */

#include <config.h>
#include "SCOREP_Score_Estimator.hpp"
#include "SCOREP_Score_Types.hpp"
#include <SCOREP_Filter.h>
#include <otf2/OTF2_EventSizeEstimator.h>
#include <math.h>

using namespace std;

/* **************************************************************************************
                                                                       internal functions
****************************************************************************************/

static void
swap( SCOREP_Score_Group** items, uint64_t pos1, uint64_t pos2 )
{
    SCOREP_Score_Group* helper = items[ pos1 ];
    items[ pos1 ] = items[ pos2 ];
    items[ pos2 ] = helper;
}

static void
quicksort( SCOREP_Score_Group** items, uint64_t size )
{
    if ( size < 2 )
    {
        return;
    }
    if ( size == 2 )
    {
        if ( items[ 0 ]->GetMaxTBC() < items[ 1 ]->GetMaxTBC() )
        {
            swap( items, 0, 1 );
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
            swap( items, beg, end );

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
        swap( items, end, pos );
    }
    while ( ( end < size ) &&
            ( items[ end ]->GetMaxTBC() == threshold ) )
    {
        end++;
    }

    quicksort( items, beg );
    quicksort( &items[ end ], size - end );
}

/* **************************************************************************************
                                                             class SCOREP_Score_Estimator
****************************************************************************************/

SCOREP_Score_Estimator::SCOREP_Score_Estimator( SCOREP_Score_Profile* profile,
                                                uint32_t              denseNum )
{
    m_dense_num   = denseNum;
    m_profile     = profile;
    m_region_num  = profile->GetNumberOfRegions();
    m_process_num = profile->GetNumberOfProcesses();

    m_has_filter = false;
    calculate_event_sizes();

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
SCOREP_Score_Estimator::initialize_regions( void )
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
SCOREP_Score_Estimator::InitializeFilter( string filterFile )
{
    /* Initialize filter component */
    SCOREP_ErrorCode err = SCOREP_SUCCESS;
    err = SCOREP_Filter_ParseFile( filterFile.c_str() );

    if ( err != SCOREP_SUCCESS )
    {
        cerr << "ERROR:Failed to open '" << filterFile << "'." << endl;
        exit( EXIT_FAILURE );
    }

    /* Initialize filter groups */
    m_filtered = ( SCOREP_Score_Group** )
                 malloc( SCOREP_SCORE_TYPE_NUM * sizeof( SCOREP_Score_Group* ) );

    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        string name = SCOREP_Score_GetTypeName( i );
        if ( i != SCOREP_SCORE_TYPE_FLT )
        {
            name += "-FLT";
        }

        m_filtered[ i ] = new SCOREP_Score_Group( i, m_process_num, name );
        m_filtered[ i ]->DoFilter( SCOREP_Score_GetFilterState( i ) );
    }

    /* Update regular groups */
    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ]->DoFilter( SCOREP_SCORE_FILTER_NO );
    }

    m_has_filter = true;
}

bool
SCOREP_Score_Estimator::match_filter( uint64_t region )
{
    bool do_filter = SCOREP_Filter_Match( m_profile->GetFileName( region ).c_str(),
                                          m_profile->GetRegionName( region ).c_str(),
                                          NULL );
    return do_filter &&
           SCOREP_Score_GetFilterState( m_profile->GetGroup( region ) ) != SCOREP_SCORE_FILTER_NO;
}

void
SCOREP_Score_Estimator::Calculate( bool showRegions )
{
    if ( showRegions )
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

            // Consider effects of enter/exit events
            if ( m_profile->HasEnterExit( region ) )
            {
                tbc += m_enter + m_exit;
            }

            // Calculate sizes of additional events
            if ( m_profile->HasParameter( region ) )
            {
                tbc += m_parameter;
            }

            if ( m_profile->HasSend( region ) )
            {
                tbc += m_send;
            }

            if ( m_profile->HasIsend( region ) )
            {
                tbc += m_isend;
            }

            if ( m_profile->HasIsendComplete( region ) )
            {
                tbc += m_isend_complete;
            }

            if ( m_profile->HasIrecvRequest( region ) )
            {
                tbc += m_irecv_request;
            }

            if ( m_profile->HasRecv( region ) )
            {
                tbc += m_recv;
            }

            if ( m_profile->HasIrecv( region ) )
            {
                tbc += m_irecv;
            }

            if ( m_profile->HasCollective( region ) )
            {
                tbc += m_collective;
            }

            if ( m_profile->HasFork( region ) )
            {
                tbc += m_fork;
            }

            if ( m_profile->HasJoin( region ) )
            {
                tbc += m_join;
            }

            if ( m_profile->HasThreadTeam( region ) )
            {
                tbc += m_thread_team;
            }

            if ( m_profile->HasAcquireLock( region ) )
            {
                tbc += m_acquire_lock;
            }

            if ( m_profile->HasReleaseLock( region ) )
            {
                tbc += m_release_lock;
            }

            if ( m_profile->HasTaskCreateComplete( region ) )
            {
                tbc += m_task_create + m_task_complete;
            }

            if ( m_profile->HasTaskSwitch( region ) )
            {
                tbc += m_task_switch;
            }

            if ( m_profile->HasCudaStreamCreate( region ) )
            {
                /* CUDA streams use a global RMA window,
                 * each RMA window is created and destroyed,
                 * once on the CPU and additionally on each stream,
                 * furthermore there is a win_create/win_destroy on
                 * the implicit GPU location */
                tbc    += m_rma_win_create + m_rma_win_destroy;
                visits += 2;
            }

            if ( m_profile->HasCudaMemcpy( region ) )
            {
                /* RMA put/get operation + completion record */
                tbc += m_rma_put + m_rma_op_complete_blocking;
            }

            // Consider number of visits
            tbc *= visits;

            m_groups[ group ]->AddRegion( tbc, time, process );
            m_groups[ SCOREP_SCORE_TYPE_ALL ]->AddRegion( tbc, time, process );

            if ( showRegions )
            {
                m_regions[ region ]->AddRegion( tbc, time, process );
            }

            if ( m_has_filter )
            {
                bool do_filter = match_filter( region );
                if ( showRegions )
                {
                    m_regions[ region ]->DoFilter( do_filter ?
                                                   SCOREP_SCORE_FILTER_YES :
                                                   SCOREP_SCORE_FILTER_NO );
                }
                if ( !do_filter )
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
SCOREP_Score_Estimator::PrintGroups( void )
{
    double   total_time = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetTotalTime();
    uint64_t max_tbc;
    uint64_t total_tbc;

    if ( m_has_filter )
    {
        max_tbc   = m_filtered[ SCOREP_SCORE_TYPE_ALL ]->GetMaxTBC();
        total_tbc = m_filtered[ SCOREP_SCORE_TYPE_ALL ]->GetTotalTBC();
    }
    else
    {
        max_tbc   = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetMaxTBC();
        total_tbc = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetTotalTBC();
    }

    cout << endl;
    cout << "Estimated aggregate size of event trace:                   "
         << total_tbc << " bytes" << endl;
    cout << "Estimated requirements for largest trace buffer (max_tbc): "
         << max_tbc << " bytes" << endl;
    cout << "(hint: When tracing set SCOREP_TOTAL_MEMORY > max_tbc to avoid intermediate flushes\n"
         << " or reduce requirements using file listing names of USR regions to be filtered.)"
         << endl << endl;

    quicksort( m_groups, SCOREP_SCORE_TYPE_NUM );

    cout << "flt type         max_tbc         time      % region" << endl;
    for ( int i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ]->Print( total_time );
    }

    if ( m_has_filter )
    {
        quicksort( &m_filtered[ 1 ], SCOREP_SCORE_TYPE_NUM - 1 );

        cout << endl;
        for ( int i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
        {
            m_filtered[ i ]->Print( total_time );
        }
    }
}

void
SCOREP_Score_Estimator::PrintRegions( void )
{
    quicksort( m_regions, m_region_num );

    double total_time = m_groups[ SCOREP_SCORE_TYPE_ALL ]->GetTotalTime();
    cout << endl;
    for ( int i = 0; i < m_region_num; i++ )
    {
        m_regions[ i ]->Print( total_time );
    }
}

void
SCOREP_Score_Estimator::DumpEventSizes( void )
{
    cout << "Dense metrics: " << m_dense << endl;
    cout << "Enter:         " << m_enter << endl;
    cout << "Exit:          " << m_exit  << endl;
}

uint32_t
SCOREP_Score_Estimator::get_compressed_size( uint64_t max_value )
{
    return ( uint32_t )1 +                                    // Number of leading zeros
           ceil( log( ( double )max_value ) / log( 256.0 ) ); // Number
}

void
SCOREP_Score_Estimator::add_header_size( uint32_t* size )
{
    *size += 1 + get_compressed_size( *size );
}

void
SCOREP_Score_Estimator::calculate_event_sizes( void )
{
    OTF2_EventSizeEstimator* estimator = OTF2_EventSizeEstimator_New();
    OTF2_EventSizeEstimator_SetNumberOfRegionDefinitions( estimator, m_region_num );

    size_t timestamp_size = OTF2_EventSizeEstimator_GetSizeOfTimestamp( estimator );
    size_t dense          = OTF2_EventSizeEstimator_GetSizeOfMetricEvent( estimator, m_dense_num );

    m_enter = timestamp_size
              + OTF2_EventSizeEstimator_GetSizeOfEnterEvent( estimator )
              + dense;

    m_exit = timestamp_size
             + OTF2_EventSizeEstimator_GetSizeOfLeaveEvent( estimator )
             + m_dense;

    m_send = OTF2_EventSizeEstimator_GetSizeOfMpiSendEvent( estimator );

    m_isend = OTF2_EventSizeEstimator_GetSizeOfMpiIsendEvent( estimator );

    m_isend_complete = OTF2_EventSizeEstimator_GetSizeOfMpiIsendCompleteEvent( estimator );
    m_irecv_request  = OTF2_EventSizeEstimator_GetSizeOfMpiIrecvRequestEvent( estimator );

    m_recv = OTF2_EventSizeEstimator_GetSizeOfMpiRecvEvent( estimator );

    m_irecv = OTF2_EventSizeEstimator_GetSizeOfMpiIrecvEvent( estimator );

    m_request_test = OTF2_EventSizeEstimator_GetSizeOfMpiRequestTestEvent( estimator );

    m_request_cancel = OTF2_EventSizeEstimator_GetSizeOfMpiRequestCancelledEvent( estimator );

    m_collective = OTF2_EventSizeEstimator_GetSizeOfMpiCollectiveBeginEvent( estimator )
                   + OTF2_EventSizeEstimator_GetSizeOfMpiCollectiveEndEvent( estimator );

    m_fork = OTF2_EventSizeEstimator_GetSizeOfThreadForkEvent( estimator );

    m_join = OTF2_EventSizeEstimator_GetSizeOfThreadJoinEvent( estimator );

    m_thread_team = OTF2_EventSizeEstimator_GetSizeOfThreadTeamBeginEvent( estimator )
                    + OTF2_EventSizeEstimator_GetSizeOfThreadTeamEndEvent( estimator );

    m_acquire_lock = OTF2_EventSizeEstimator_GetSizeOfThreadAcquireLockEvent( estimator );

    m_release_lock = OTF2_EventSizeEstimator_GetSizeOfThreadReleaseLockEvent( estimator );

    m_task_create = OTF2_EventSizeEstimator_GetSizeOfThreadTaskCreateEvent( estimator );

    m_task_switch = OTF2_EventSizeEstimator_GetSizeOfThreadTaskSwitchEvent( estimator );

    m_task_complete = OTF2_EventSizeEstimator_GetSizeOfThreadTaskCompleteEvent( estimator );

    m_parameter = OTF2_EventSizeEstimator_GetSizeOfParameterIntEvent( estimator );

    m_rma_win_create = OTF2_EventSizeEstimator_GetSizeOfRmaWinCreateEvent( estimator );

    m_rma_win_destroy = OTF2_EventSizeEstimator_GetSizeOfRmaWinDestroyEvent( estimator );

    m_rma_collective = OTF2_EventSizeEstimator_GetSizeOfRmaCollectiveBeginEvent( estimator )
                       + OTF2_EventSizeEstimator_GetSizeOfRmaCollectiveEndEvent( estimator );

    m_rma_group_sync = OTF2_EventSizeEstimator_GetSizeOfRmaGroupSyncEvent( estimator );

    m_rma_request_lock = OTF2_EventSizeEstimator_GetSizeOfRmaRequestLockEvent( estimator );

    m_rma_acquire_lock = OTF2_EventSizeEstimator_GetSizeOfRmaAcquireLockEvent( estimator );

    m_rma_try_lock = OTF2_EventSizeEstimator_GetSizeOfRmaTryLockEvent( estimator );

    m_rma_release_lock = OTF2_EventSizeEstimator_GetSizeOfRmaReleaseLockEvent( estimator );

    m_rma_sync = OTF2_EventSizeEstimator_GetSizeOfRmaSyncEvent( estimator );

    m_rma_wait_change = OTF2_EventSizeEstimator_GetSizeOfRmaWaitChangeEvent( estimator );

    m_rma_get = OTF2_EventSizeEstimator_GetSizeOfRmaGetEvent( estimator );

    m_rma_put = OTF2_EventSizeEstimator_GetSizeOfRmaPutEvent( estimator );

    m_rma_atomic = OTF2_EventSizeEstimator_GetSizeOfRmaAtomicEvent( estimator );

    m_rma_op_complete_blocking = OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteBlockingEvent( estimator );

    m_rma_complete_blocking = OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteBlockingEvent( estimator );

    m_rma_op_complete_non_blocking = OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteNonBlockingEvent( estimator );

    m_rma_op_test = OTF2_EventSizeEstimator_GetSizeOfRmaOpTestEvent( estimator );

    m_rma_op_complete_remote = OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteRemoteEvent( estimator );

    m_thread = OTF2_EventSizeEstimator_GetSizeOfThreadBeginEvent( estimator )
               + OTF2_EventSizeEstimator_GetSizeOfThreadEndEvent( estimator );

    m_thread_wait = OTF2_EventSizeEstimator_GetSizeOfThreadWaitEvent( estimator );

    OTF2_EventSizeEstimator_Delete( estimator );
}
