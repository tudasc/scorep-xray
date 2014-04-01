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
 * Copyright (c) 2009-2014,
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
 * @file
 *
 * @brief      Implements a class which performs calculations for trace
 *             size estimation.
 */

#include <config.h>
#include "SCOREP_Score_Estimator.hpp"
#include "SCOREP_Score_EventList.hpp"
#include "SCOREP_Score_Types.hpp"
#include <SCOREP_Filter.h>
#include <math.h>
#include <fstream>

using namespace std;

#define SCOREP_SCORE_BUFFER_SIZE 128

/* **************************************************************************************
                                                                       internal functions
****************************************************************************************/

/**
 * Swaps the items on @a pos1 and @a pos2 from the group list.
 * Needed for the quicksort.
 * @param item  List of groups that is sorted.
 * @param pos1  Position of an elemant that is swapped with the element at @a pos2.
 * @param pos2  Position of an elemant that is swapped with the element at @a pos1.
 */
static void
swap( SCOREP_Score_Group** items, uint64_t pos1, uint64_t pos2 )
{
    SCOREP_Score_Group* helper = items[ pos1 ];
    items[ pos1 ] = items[ pos2 ];
    items[ pos2 ] = helper;
}

/**
 * Sorts an array of pointers to groups using the quicksort algorithm.
 * @param items  Array of groups that are sorted.
 * @param size   Number of groups in @a items.
 */
static void
quicksort( SCOREP_Score_Group** items, uint64_t size )
{
    if ( size < 2 )
    {
        return;
    }
    if ( size == 2 )
    {
        if ( items[ 0 ]->getMaxTraceBufferSize() <
             items[ 1 ]->getMaxTraceBufferSize() )
        {
            swap( items, 0, 1 );
        }
        return;
    }

    uint64_t beg       = 0;
    uint64_t end       = size - 1;
    uint64_t pos       = size / 2;
    uint64_t threshold = items[ pos ]->getMaxTraceBufferSize();

    while ( beg < end )
    {
        while ( ( beg < end ) &&
                ( items[ beg ]->getMaxTraceBufferSize() > threshold ) )
        {
            beg++;
        }

        while ( ( beg < end ) &&
                ( items[ end ]->getMaxTraceBufferSize() <= threshold ) )
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
    if ( items[ end ]->getMaxTraceBufferSize() < threshold )
    {
        swap( items, end, pos );
    }
    while ( ( end < size ) &&
            ( items[ end ]->getMaxTraceBufferSize() == threshold ) )
    {
        end++;
    }

    quicksort( items, beg );
    quicksort( &items[ end ], size - end );
}

static bool
is_writable_file_creatable( string filename )
{
    fstream temp_file( filename.c_str(), ios_base::out );
    if ( temp_file.good() )
    {
        temp_file.close();
        remove( filename.c_str() );
        return true;
    }
    return false;
}

static string
get_temp_filename( void )
{
    static string      temp_dir;
    static bool        is_initialized = false;
    static const char* username       = getenv( "USER" );
    static uint32_t    sequence_no    = 0;

    stringstream filename;
    filename << "scorep-score_"; // Avoid clashes with other applications
    if ( username != NULL )
    {
        filename << username << "_"; // Avoid interference with other users
    }
    filename << getpid() << "_"      // Avoid interference with other process of the same user
             << sequence_no;         // Avoid interference of multiple calls in the one tool

    sequence_no++;

    if ( !is_initialized )
    {
        static string slash = "/";
        const char*   temp;
        is_initialized = true;

        temp = getenv( "TMPDIR" );
        if ( ( temp != NULL ) &&
             is_writable_file_creatable( temp + slash + filename.str() ) )
        {
            temp_dir = temp + slash;
            return temp_dir + filename.str();
        }

        temp = getenv( "TMP" );
        if ( ( temp != NULL ) &&
             is_writable_file_creatable( temp + slash + filename.str() ) )
        {
            temp_dir = temp + slash;
            return temp_dir + filename.str();
        }

        temp = getenv( "TEMP" );
        if ( ( temp != NULL ) &&
             is_writable_file_creatable( temp + slash + filename.str() ) )
        {
            temp_dir = temp + slash;
            return temp_dir + filename.str();
        }

        temp_dir = "/tmp/";
        if ( is_writable_file_creatable( temp_dir + filename.str() ) )
        {
            return temp_dir + filename.str();
        }

        temp_dir = "";
        if ( is_writable_file_creatable( filename.str() ) )
        {
            return filename.str();
        }

        cerr << "ERROR: Failed to find writable directory for temporary files." << endl;
        exit( EXIT_FAILURE );
    }

    return temp_dir + filename.str();
}


/* **************************************************************************************
                                                             class SCOREP_Score_Estimator
****************************************************************************************/

SCOREP_Score_Estimator::SCOREP_Score_Estimator( SCOREP_Score_Profile* profile,
                                                uint64_t              denseNum )
{
    m_dense_num   = denseNum;
    m_profile     = profile;
    m_region_num  = profile->getNumberOfRegions();
    m_process_num = profile->getNumberOfProcesses();

    m_has_filter = false;
    m_event_list.push_back( new SCOREP_Score_TimestampEvent() );
    m_event_list.push_back( new SCOREP_Score_EnterEvent() );
    m_event_list.push_back( new SCOREP_Score_LeaveEvent() );
    if ( denseNum > 0 )
    {
        m_event_list.push_back( new SCOREP_Score_MetricEvent() );
    }
    m_event_list.push_back( new SCOREP_Score_ParameterEvent() );

#define SCOREP_SCORE_EVENT( name ) region_list.push_back( name );
    std::deque<std::string> region_list;
    SCOREP_SCORE_EVENT_SEND;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiSend", region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_ISEND;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiIsend", region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_ISENDCOMPLETE;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiIsendComplete",
                                                             region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_IRECVREQUEST;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiIrecvRequest",
                                                             region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_RECV;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiRecv", region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_IRECV;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiIrecv", region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_COLLECTIVE;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiCollectiveBegin",
                                                             region_list ) );
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "MpiCollectiveEnd",
                                                             region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_ACQUIRELOCK;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "ThreadAcquireLock",
                                                             region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_RELEASELOCK;
    m_event_list.push_back( new SCOREP_Score_NameMatchEvent( "ThreadReleaseLock",
                                                             region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_FORK;
    m_event_list.push_back( new SCOREP_Score_PrefixMatchEvent( "ThreadFork",
                                                               region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_JOIN;
    m_event_list.push_back( new SCOREP_Score_PrefixMatchEvent( "ThreadJoin",
                                                               region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_TEAM;
    m_event_list.push_back( new SCOREP_Score_PrefixMatchEvent( "ThreadTeamBegin",
                                                               region_list ) );
    m_event_list.push_back( new SCOREP_Score_PrefixMatchEvent( "ThreadTeamEnd",
                                                               region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_TASK_CREATE;
    m_event_list.push_back( new SCOREP_Score_PrefixMatchEvent( "ThreadTaskCreate",
                                                               region_list ) );
    m_event_list.push_back( new SCOREP_Score_PrefixMatchEvent( "ThreadTaskComplete",
                                                               region_list ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_TASK_SWITCH;
    m_event_list.push_back( new SCOREP_Score_PrefixMatchEvent( "ThreadTaskSwitch",
                                                               region_list ) );
#undef SCOREP_SCORE_EVENT

    calculate_event_sizes();

    m_filtered = NULL;
    m_regions  = NULL;
    m_groups   = ( SCOREP_Score_Group** )malloc( SCOREP_SCORE_TYPE_NUM * sizeof( SCOREP_Score_Group* ) );
    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ] = new SCOREP_Score_Group( i, m_process_num,
                                                SCOREP_Score_getTypeName( i ) );
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

    while ( !m_event_list.empty() )
    {
        delete ( m_event_list.front() );
        m_event_list.pop_front();
    }
}

void
SCOREP_Score_Estimator::initializeFilter( string filterFile )
{
    /* Initialize filter component */
    SCOREP_ErrorCode err = SCOREP_SUCCESS;
    err = SCOREP_Filter_ParseFile( filterFile.c_str() );

    if ( err != SCOREP_SUCCESS )
    {
        cerr << "ERROR: Failed to open '" << filterFile << "'." << endl;
        exit( EXIT_FAILURE );
    }

    /* Initialize filter groups */
    m_filtered = ( SCOREP_Score_Group** )
                 malloc( SCOREP_SCORE_TYPE_NUM * sizeof( SCOREP_Score_Group* ) );

    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        string name = SCOREP_Score_getTypeName( i );
        if ( i != SCOREP_SCORE_TYPE_FLT )
        {
            name += "-FLT";
        }

        m_filtered[ i ] = new SCOREP_Score_Group( i, m_process_num, name );
        m_filtered[ i ]->doFilter( SCOREP_Score_getFilterState( i ) );
    }

    /* Update regular groups */
    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ]->doFilter( SCOREP_SCORE_FILTER_NO );
    }

    m_has_filter = true;
}

void
SCOREP_Score_Estimator::calculate( bool showRegions )
{
    if ( showRegions )
    {
        initialize_regions();
    }

    for ( uint64_t region = 0; region < m_region_num; region++ )
    {
        uint64_t group           = m_profile->getGroup( region );
        uint64_t bytes_per_visit = 0;

        /* Calculate bytes per visit */
        for ( event_list_t::iterator i = m_event_list.begin();
              i != m_event_list.end(); i++ )
        {
            if ( ( *i )->occursInRegion( m_profile, region ) )
            {
                bytes_per_visit += ( *i )->getEventSize();
            }
        }

        /* Apply region data for each process */
        for ( uint64_t process = 0; process < m_process_num; process++ )
        {
            uint64_t visits = m_profile->getVisits( region, process );
            double   time   = m_profile->getTime( region, process );

            if ( visits == 0 )
            {
                continue;
            }

            m_groups[ group ]->addRegion( visits, bytes_per_visit, time, process );
            m_groups[ SCOREP_SCORE_TYPE_ALL ]->addRegion( visits, bytes_per_visit,
                                                          time, process );

            if ( showRegions )
            {
                m_regions[ region ]->addRegion( visits, bytes_per_visit, time, process );
            }

            if ( m_has_filter )
            {
                bool do_filter = match_filter( region );
                if ( showRegions )
                {
                    m_regions[ region ]->doFilter( do_filter ?
                                                   SCOREP_SCORE_FILTER_YES :
                                                   SCOREP_SCORE_FILTER_NO );
                }
                if ( !do_filter )
                {
                    m_filtered[ group ]->addRegion( visits, bytes_per_visit,
                                                    time, process );
                    m_filtered[ SCOREP_SCORE_TYPE_ALL ]->addRegion( visits,
                                                                    bytes_per_visit,
                                                                    time, process );
                }
                else
                {
                    m_filtered[ SCOREP_SCORE_TYPE_FLT ]->addRegion( visits,
                                                                    bytes_per_visit,
                                                                    time, process );
                }
            }
        }
    }
}

void
SCOREP_Score_Estimator::printGroups( void )
{
    double   total_time = m_groups[ SCOREP_SCORE_TYPE_ALL ]->getTotalTime();
    uint64_t max_buf;
    uint64_t total_buf;
    uint64_t memory_req;

    if ( m_has_filter )
    {
        max_buf   = m_filtered[ SCOREP_SCORE_TYPE_ALL ]->getMaxTraceBufferSize();
        total_buf = m_filtered[ SCOREP_SCORE_TYPE_ALL ]->getTotalTraceBufferSize();
    }
    else
    {
        max_buf   = m_groups[ SCOREP_SCORE_TYPE_ALL ]->getMaxTraceBufferSize();
        total_buf = m_groups[ SCOREP_SCORE_TYPE_ALL ]->getTotalTraceBufferSize();
    }

    /* Estimate definition size by profile size and round up to the next
       multiple of 2 MB */
    memory_req = m_profile->getFileSize() /
                 ( m_profile->getNumberOfProcesses() * m_profile->getNumberOfMetrics() );
    memory_req = ( max_buf + memory_req + 4 * 1024 * 1024 ) /
                 ( 2 * 1024 * 1024 );

    cout << endl;
    cout << "Estimated aggregate size of event trace:                   "
         << total_buf << " bytes" << endl;
    cout << "Estimated requirements for largest trace buffer (max_buf): "
         << max_buf << " bytes" << endl;
    cout << "Estimated memory requirements (SCOREP_TOTAL_MEMORY):       "
         << memory_req * 2 << "MB" << endl;
    cout << "(hint: When tracing set SCOREP_TOTAL_MEMORY="
         << memory_req * 2 << "MB to avoid intermediate flushes\n"
         << " or reduce requirements using USR regions filters.)"
         << endl << endl;

    quicksort( m_groups, SCOREP_SCORE_TYPE_NUM );

    cout << "flt type         max_buf        visits         time  %time  region" << endl;
    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ]->print( total_time );
    }

    if ( m_has_filter )
    {
        quicksort( &m_filtered[ 1 ], SCOREP_SCORE_TYPE_NUM - 1 );

        cout << endl;
        for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
        {
            m_filtered[ i ]->print( total_time );
        }
    }
}

void
SCOREP_Score_Estimator::printRegions( void )
{
    quicksort( m_regions, m_region_num );

    double total_time = m_groups[ SCOREP_SCORE_TYPE_ALL ]->getTotalTime();
    cout << endl;
    for ( uint64_t i = 0; i < m_region_num; i++ )
    {
        m_regions[ i ]->print( total_time );
    }
}

void
SCOREP_Score_Estimator::dumpEventSizes( void )
{
    for ( event_list_t::iterator i = m_event_list.begin(); i != m_event_list.end(); i++ )
    {
        std::string name   = ( *i )->getName();
        std::string blanks = "                         ";
        cout << name << ":" << blanks.substr( 0, 20 - name.length() )
             << ( *i )->getEventSize() << std::endl;
    }
}

void
SCOREP_Score_Estimator::calculate_event_sizes( void )
{
    /* Write otf2-estimator input */
    string in_filename  = get_temp_filename();
    string out_filename = get_temp_filename();

    fstream estimator_in( in_filename.c_str(), ios_base::out );
    if ( !estimator_in.good() )
    {
        cerr << "ERROR: Failed to open temorary file for otf2-estimator input."
             << endl;
        exit( EXIT_FAILURE );
    }

    estimator_in << "set Region " << m_region_num << "\n";
    estimator_in << "set Metric " << m_profile->getNumberOfMetrics() << "\n";

    for ( event_list_t::iterator i = m_event_list.begin(); i != m_event_list.end(); i++ )
    {
        if ( ( *i )->getName() == "Metric" )
        {
            estimator_in << "get " << ( *i )->getName()
                         << " " << m_dense_num << "\n";
        }
        else
        {
            estimator_in << "get " << ( *i )->getName() << "\n";
        }
    }
    estimator_in << "exit" << std::endl;
    estimator_in.close();

    string command = OTF2_ESTIMATOR_INSTALL " > \"" +
                     out_filename + "\" < \"" + in_filename + "\"";
    if ( system( command.c_str() ) != EXIT_SUCCESS )
    {
        cerr << "ERROR: Failed to call otf2-estimator." << endl;
        exit( EXIT_FAILURE );
    }

    /* Read output of otf2-estimator */
    fstream estimator_out( out_filename.c_str(), ios_base::in );
    if ( !estimator_out.good() )
    {
        cerr << "ERROR: Failed to open temorary file for otf2-estimator input."
             << endl;
        exit( EXIT_FAILURE );
    }

    while ( !estimator_out.eof() )
    {
        /* Decode next line. Has format <name><space><number of bytes> */
        char buf[ SCOREP_SCORE_BUFFER_SIZE ];
        estimator_out.getline( buf, SCOREP_SCORE_BUFFER_SIZE );
        char* number = buf;
        while ( *number != ' ' && *number != '\0' )
        {
            number++;
        }
        *number = '\0';
        number++;
        uint64_t value = strtol( number, NULL, 0 );

        /* Apply to event sizes */
        for ( event_list_t::iterator i = m_event_list.begin();
              i != m_event_list.end(); i++ )
        {
            ( *i )->setEventSize( buf, value );
        }
    }

    /* Clean up */
    estimator_out.close();
    remove( in_filename.c_str() );
    remove( out_filename.c_str() );

    //dumpEventSizes();
}

/* ****************************************************** private methods */

void
SCOREP_Score_Estimator::delete_groups( SCOREP_Score_Group** groups, uint64_t num )
{
    if ( groups != NULL )
    {
        for ( uint64_t i = 0; i < num; i++ )
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
        m_regions[ region ] = new SCOREP_Score_Group( m_profile->getGroup( region ),
                                                      m_process_num,
                                                      m_profile->getRegionName( region ) );
    }
}

bool
SCOREP_Score_Estimator::match_filter( uint64_t region )
{
    bool do_filter = SCOREP_Filter_Match( m_profile->getFileName( region ).c_str(),
                                          m_profile->getRegionName( region ).c_str(),
                                          NULL );
    return do_filter &&
           SCOREP_Score_getFilterState( m_profile->getGroup( region ) ) != SCOREP_SCORE_FILTER_NO;
}
