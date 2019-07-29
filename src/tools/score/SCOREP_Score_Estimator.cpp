/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2016, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
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
#include "SCOREP_Score_Event.hpp"
#include "SCOREP_Score_Profile.hpp"
#include "SCOREP_Score_EventList.hpp"
#include "SCOREP_Score_Types.hpp"
#include <SCOREP_Filter.h>
#include <math.h>
#include <fstream>
#include <iomanip>
#include <deque>
#include <limits>

using namespace std;

#define SCOREP_SCORE_BUFFER_SIZE 128

/* **************************************************************************************
                                                                       internal functions
****************************************************************************************/

/**
 * Swaps the items on @a pos1 and @a pos2 from the group list.
 * Needed for the quicksort.
 * @param item  List of groups that is sorted.
 * @param pos1  Position of an element that is swapped with the element at @a pos2.
 * @param pos2  Position of an element that is swapped with the element at @a pos1.
 */
static void
swap( SCOREP_Score_Group** items,
      uint64_t             pos1,
      uint64_t             pos2 )
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
quicksort( SCOREP_Score_Group** items,
           uint64_t             size )
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

        cerr << "ERROR: Failed to find writable directory for temporary files" << endl;
        exit( EXIT_FAILURE );
    }

    return temp_dir + filename.str();
}

static string
get_user_readable_byte_no( uint64_t bytes )
{
    const uint32_t base = 1024;
    uint32_t       exp;
    for ( exp = 0; bytes > 4 * base; exp++ )
    {
        bytes /= base;
    }
    // round up;
    bytes++;
    stringstream result;
    result << bytes;
    switch ( exp )
    {
        case 0:
            result << " bytes";
            break;
        case 1:
            result << "kB";
            break;
        case 2:
            result << "MB";
            break;
        case 3:
            result << "GB";
            break;
        case 4:
            result << "TB";
            break;
        case 5:
            result << "PB";
            break;
        case 6:
            result << "EB";
            break;
        case 7:
            result << "ZB";
            break;
        case 8:
            result << "YB";
            break;
    }
    return result.str();
}


/* **************************************************************************************
                                                             class SCOREP_Score_Estimator
****************************************************************************************/

SCOREP_Score_Estimator::SCOREP_Score_Estimator( SCOREP_Score_Profile* profile,
                                                uint64_t              denseNum )
    : m_filter( SCOREP_Filter_New() )
    , m_has_filter( false )
    , m_profile( profile )
    , m_groups( 0 )
    , m_regions( 0 )
    , m_filtered( 0 )
    , m_region_num( profile->getNumberOfRegions() )
    , m_process_num( profile->getNumberOfProcesses() )
    , m_dense_num( denseNum )
    , m_show_regions( false ) /* will only be used while in calculate() */
    , m_bytes_per_num_parameter( 0 )
    , m_bytes_per_str_parameter( 0 )
    , m_bytes_per_hit( 0 )
{
    SCOREP_Score_Event* timestamp_event = new SCOREP_Score_TimestampEvent();
    registerEvent( timestamp_event );
    registerEvent( new SCOREP_Score_EnterEvent() );
    registerEvent( new SCOREP_Score_LeaveEvent() );
    if ( m_profile->hasHits() )
    {
        registerEvent( new SCOREP_Score_CallingContextEnterEvent() );
        registerEvent( new SCOREP_Score_CallingContextLeaveEvent() );
        registerEvent( new SCOREP_Score_CallingContextSampleEvent() );
    }
    if ( m_dense_num > 0 )
    {
        registerEvent( new SCOREP_Score_MetricEvent( m_dense_num ) );
    }
    registerEvent( new SCOREP_Score_ParameterIntEvent() );
    registerEvent( new SCOREP_Score_ParameterStringEvent() );

    if ( profile->getNumberOfProgramArguments() >= 0 )
    {
        registerEvent( new SCOREP_Score_ProgramBeginEvent( profile->getNumberOfProgramArguments() ) );
        registerEvent( new SCOREP_Score_ProgramEndEvent() );
    }

#define SCOREP_SCORE_EVENT( name ) region_set.insert( name );
    set<string> region_set;
    SCOREP_SCORE_EVENT_MPI_SEND;
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiSend", region_set, true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_MPI_ISEND;
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiIsend", region_set, true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_MPI_ISENDCOMPLETE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiIsendComplete",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_MPI_IRECVREQUEST;
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiIrecvRequest",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_MPI_RECV;
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiRecv", region_set, true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_MPI_IRECV;
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiIrecv", region_set, true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_MPI_COLLECTIVE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiCollectiveBegin",
                                                    region_set,
                                                    false ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "MpiCollectiveEnd",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_THREAD_ACQUIRELOCK;
    registerEvent( new SCOREP_Score_NameMatchEvent( "ThreadAcquireLock",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_THREAD_RELEASELOCK;
    registerEvent( new SCOREP_Score_NameMatchEvent( "ThreadReleaseLock",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_RMA_OP;
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaPut",
                                                    region_set,
                                                    true ) );

    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaOpCompleteBlocking",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_RMA_OP_COMPLETE_REMOTE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaOpCompleteRemote",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_RMA_ATOMIC;
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaAtomic",
                                                    region_set,
                                                    false ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaOpCompleteBlocking",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_RMA_COLLECTIVE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaCollectiveBegin",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaCollectiveEnd",
                                                    region_set,
                                                    true ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_RMA_WAIT_CHANGE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaWaitChange",
                                                    region_set,
                                                    false ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_RMA_LOCK;
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaRequestLock",
                                                    region_set,
                                                    false ) );

    region_set.clear();
    SCOREP_SCORE_EVENT_RMA_RELEASE_LOCK;
    registerEvent( new SCOREP_Score_NameMatchEvent( "RmaReleaseLock",
                                                    region_set,
                                                    false ) );

#undef SCOREP_SCORE_EVENT
#define SCOREP_SCORE_EVENT( name ) region_list.push_back( name );
    deque<string> region_list;
    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_FORK;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadFork",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_JOIN;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadJoin",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_TEAM;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadTeamBegin",
                                                      region_list,
                                                      true ) );
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadTeamEnd",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_TASK_CREATE;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadTaskCreate",
                                                      region_list,
                                                      false ) );
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadTaskComplete",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_TASK_SWITCH;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadTaskSwitch",
                                                      region_list,
                                                      true ) );
    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_CREATE;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadCreate",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_BEGIN;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadBegin",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_WAIT;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadWait",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_THREAD_CREATE_WAIT_END;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "ThreadEnd",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_MEASUREMENT_BUFFER_FLUSH;
    registerEvent( new SCOREP_Score_PrefixMatchEvent( "BufferFlush",
                                                      region_list,
                                                      true ) );

    region_list.clear();
    SCOREP_SCORE_EVENT_MEASUREMENT_ON_OFF;
    SCOREP_Score_Event* measurement_on_off_event = new SCOREP_Score_PrefixMatchEvent( "MeasurementOnOff",
                                                                                      region_list,
                                                                                      false );
    registerEvent( measurement_on_off_event );

#undef SCOREP_SCORE_EVENT
#define SCOREP_SCORE_EVENT( name ) region_set.insert( name );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_CREATE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoDestroyHandle",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoCreateHandle",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_DELETE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoDestroyHandle",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoDeleteFile",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_SEEK;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoSeek",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_BLOCKING_TRANSFER;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoOperationComplete",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoOperationBegin",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_NONBLOCKING_TRANSFER_BEGIN;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoOperationIssued",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoOperationBegin",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_NONBLOCKING_TRANSFER_END;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoOperationComplete",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoOperationTest",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_CLOSE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoDestroyHandle",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_DUPLICATE;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoDuplicateHandle",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoDestroyHandle",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_ACQUIRE_LOCK;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoTryLock",
                                                    region_set,
                                                    true ) );
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoAcquireLock",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_OPERATION_CANCELLED;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoOperationCancelled",
                                                    region_set,
                                                    true ) );
    region_set.clear();
    SCOREP_SCORE_EVENT_IO_RELEASE_LOCK;
    registerEvent( new SCOREP_Score_NameMatchEvent( "IoReleaseLock",
                                                    region_set,
                                                    true ) );
    region_set.clear();
#undef SCOREP_SCORE_EVENT
    calculate_event_sizes();
    /* One visit to MeasurementOnOff corresponds to two MeasurementOnOff trace events. */
    measurement_on_off_event->setEventSize( 2 * ( measurement_on_off_event->getEventSize() + timestamp_event->getEventSize() ) );

    m_bytes_per_num_parameter = getEventSize( "ParameterInt" ) +
                                getEventSize( "Timestamp" );

    m_bytes_per_str_parameter = getEventSize( "ParameterString" ) +
                                getEventSize( "Timestamp" );

    if ( m_profile->hasHits() )
    {
        m_bytes_per_hit = getEventSize( "CallingContextSample" ) +
                          getEventSize( "Timestamp" );
    }

    m_bytes_per_visits.resize( m_region_num, 0 );
    for ( uint64_t region = 0; region < m_region_num; region++ )
    {
        m_bytes_per_visits[ region ] = 0;

        /* Calculate bytes per visit, though visits into sampling regions wont
           be recorded in the trace */
        if ( m_profile->getRegionParadigm( region ) != "sampling" )
        {
            m_bytes_per_visits[ region ] = bytesPerVisit( region );
        }
    }

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

    if ( m_filter )
    {
        SCOREP_Filter_Delete( m_filter );
        m_filter     = 0;
        m_has_filter = false;
    }

    for ( map<string, SCOREP_Score_Event*>::iterator i = m_events.begin();
          i != m_events.end(); i++ )
    {
        delete i->second;
    }
}

void
SCOREP_Score_Estimator::registerEvent( SCOREP_Score_Event* event )
{
    m_events.insert( pair< string, SCOREP_Score_Event* >( event->getName(), event ) );
}

uint32_t
SCOREP_Score_Estimator::getEventSize( const string& name )
{
    map<string, SCOREP_Score_Event*>::const_iterator it = m_events.find( name );
    if ( it == m_events.end() )
    {
        return 0;
    }

    return it->second->getEventSize();
}

void
SCOREP_Score_Estimator::setEventSize( const std::string& name,
                                      uint32_t           size )
{
    map<string, SCOREP_Score_Event*>::iterator it = m_events.find( name );
    if ( it == m_events.end() )
    {
        return;
    }

    it->second->setEventSize( size );
}

void
SCOREP_Score_Estimator::initializeFilter( const string& filterFile )
{
    /* Initialize filter component */
    SCOREP_ErrorCode err = SCOREP_Filter_ParseFile( m_filter, filterFile.c_str() );
    if ( err != SCOREP_SUCCESS )
    {
        cerr << "ERROR: Failed to open '" << filterFile << "'" << endl;
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


uint64_t
SCOREP_Score_Estimator::bytesPerVisit( uint64_t region )
{
    uint64_t bytes_per_visit = 0;
    /* Calculate bytes per visit, though visits into sampling regions wont
        be recorded in the trace */
    for ( map<string, SCOREP_Score_Event*>::iterator i = m_events.begin();
          i != m_events.end(); i++ )
    {
        SCOREP_Score_Event* event = i->second;
        if ( event->contributes( *m_profile, region ) )
        {
            bytes_per_visit += event->getEventSize();
            if ( event->hasTimestamp() )
            {
                bytes_per_visit += getEventSize( "Timestamp" );
            }
        }
    }
    return bytes_per_visit;
}




void
SCOREP_Score_Estimator::calculate( bool showRegions,
                                   bool useMangled )
{
    /* we need access to this predicate in our operator() */
    m_show_regions = showRegions;
    if ( m_show_regions )
    {
        initialize_regions( useMangled );
    }

    /* Apply region data for each process */
    for ( uint64_t process = 0; process < m_process_num; process++ )
    {
        m_profile->iterateCalltree( process, *this );
    }
}

void
SCOREP_Score_Estimator::printGroups( void )
{
    double   total_time = m_groups[ SCOREP_SCORE_TYPE_ALL ]->getTotalTime();
    uint64_t max_buf;
    uint64_t total_buf;
    uint64_t memory_req;
    uint64_t value = 2 * 1024 * 1024;
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

    memory_req = max_buf;
    memory_req = value > memory_req ? value : memory_req;
    memory_req = memory_req + value *  m_profile->getMaxNumberOfLocationsPerProcess();

    cout << endl;
    cout << "Estimated aggregate size of event trace:                   "
         << get_user_readable_byte_no( total_buf ) << endl;
    cout << "Estimated requirements for largest trace buffer (max_buf): "
         << get_user_readable_byte_no( max_buf ) << endl;
    cout << "Estimated memory requirements (SCOREP_TOTAL_MEMORY):       "
         << get_user_readable_byte_no( memory_req ) << endl;
    if ( memory_req > numeric_limits<uint32_t>::max() )
    {
        cout << "(warning: The memory requirements cannot be satisfied by Score-P to avoid\n"
             << " intermediate flushes when tracing. Set SCOREP_TOTAL_MEMORY=4G to get the\n"
             << " maximum supported memory or reduce requirements using USR regions filters.)"
             << endl << endl;
    }
    else
    {
        cout << "(hint: When tracing set SCOREP_TOTAL_MEMORY="
             << get_user_readable_byte_no( memory_req ) << " to avoid intermediate flushes\n"
             << " or reduce requirements using USR regions filters.)"
             << endl << endl;
    }

    quicksort( m_groups, SCOREP_SCORE_TYPE_NUM );

    // The first group is "ALL"
    m_groups[ 0 ]->updateWidths( m_widths );
    cout << "flt"
         << " " << setw( m_widths.m_type ) << "type"
         << " " << setw( m_widths.m_bytes ) << "max_buf[B]"
         << " " << setw( m_widths.m_visits ) << "visits";
    if ( m_profile->hasHits() )
    {
        cout << " " << setw( m_widths.m_hits ) << "hits";
    }
    cout << " " << setw( m_widths.m_time ) << "time[s]"
         << " time[%]"
         << " " << setw( m_widths.m_time_per_visit ) << "time/visit[us]"
         << "  region"
         << endl;
    for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
    {
        m_groups[ i ]->print( total_time, m_widths, m_profile->hasHits() );
    }

    if ( m_has_filter )
    {
        quicksort( &m_filtered[ 1 ], SCOREP_SCORE_TYPE_NUM - 1 );

        cout << endl;
        for ( uint64_t i = 0; i < SCOREP_SCORE_TYPE_NUM; i++ )
        {
            m_filtered[ i ]->print( total_time, m_widths, m_profile->hasHits() );
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
        m_regions[ i ]->print( total_time, m_widths, m_profile->hasHits() );
    }
}

void
SCOREP_Score_Estimator::dumpEventSizes( void )
{
    for ( map<string, SCOREP_Score_Event*>::iterator i = m_events.begin(); i != m_events.end(); i++ )
    {
        const string& name   = i->second->getName();
        string        blanks = "                         ";
        cerr << name << ":" << blanks.substr( 0, 20 - name.length() )
             << i->second->getEventSize() << endl;
    }
}

void
SCOREP_Score_Estimator::calculate_event_sizes( void )
{
    /* Write otf2-estimator input */
    string in_filename  = get_temp_filename();
    string out_filename = get_temp_filename();

    fstream estimator_in( in_filename.c_str(), ios_base::out );
    if ( !estimator_in )
    {
        cerr << "ERROR: Failed to open temporary file for 'otf2-estimator' input" << endl;
        exit( EXIT_FAILURE );
    }

    estimator_in << "set Region " << m_region_num << "\n";
    estimator_in << "set Metric " << m_profile->getNumberOfMetrics() << "\n";
    const map<string, uint64_t>& definition_counters =
        m_profile->getDefinitionCounters();
    for ( map<string, uint64_t>::const_iterator it = definition_counters.begin();
          it != definition_counters.end(); ++it )
    {
        estimator_in << "set " << it->first << " " << it->second << "\n";
    }

    for ( map<string, SCOREP_Score_Event*>::iterator i = m_events.begin(); i != m_events.end(); i++ )
    {
        estimator_in << "get " << i->second->getName() << "\n";
    }
    estimator_in << "exit" << endl;
    estimator_in.close();

    string command = OTF2_ESTIMATOR_INSTALL " > \"" +
                     out_filename + "\" < \"" + in_filename + "\"";
    if ( system( command.c_str() ) != EXIT_SUCCESS )
    {
        cerr << "ERROR: Failed to call 'otf2-estimator'" << endl;
        exit( EXIT_FAILURE );
    }

    /* Read output of otf2-estimator */
    fstream estimator_out( out_filename.c_str(), ios_base::in );
    if ( !estimator_out )
    {
        cerr << "ERROR: Failed to open temporary file for 'otf2-estimator' output" << endl;
        exit( EXIT_FAILURE );
    }

    while ( estimator_out )
    {
        /* Decode next line. Has format <name><space><number of bytes>
         * where <name> can have spaces too. */
        string line;
        getline( estimator_out, line );
        size_t size_pos = line.find_last_of( " " );
        if ( size_pos == string::npos )
        {
            continue;
        }
        string   event = line.substr( 0, size_pos );
        string   number( line.substr( size_pos + 1 ) );
        char*    end_pos;
        uint64_t value = strtoul( number.c_str(), &end_pos, 0 );
        if ( *end_pos )
        {
            continue;
        }

        /* Apply to event sizes */
        setEventSize( event, value );
    }

    /* Clean up */
    estimator_out.close();
    remove( in_filename.c_str() );
    remove( out_filename.c_str() );

    //dumpEventSizes();
}

void
SCOREP_Score_Estimator::operator()( uint64_t process,
                                    uint64_t region,
                                    uint64_t parentRegion,
                                    uint64_t visits,
                                    double   time,
                                    uint64_t hits,
                                    uint32_t numParameters,
                                    uint32_t strParameters )
{
    if ( visits == 0 && hits == 0 )
    {
        return;
    }

    SCOREP_Score_Type group = m_profile->getGroup( region );

    const string& region_name = m_profile->getRegionName( region );
    if ( m_profile->isDynamicRegion( region ) && parentRegion != -1 )
    {
        /* Attribute bytes, visits, and time to parent node. Only parent contributes and
           can be filtered. Note that parent itself has a visit count of 0 if it has only
           iteration/instance childs. Note that dynamic regions wont trigger parameters in
           the trace */
        uint64_t bytes = visits * ( m_bytes_per_visits[ parentRegion ] );
        m_groups[ group ]->updateProcess( process, bytes, visits, 0, time );
        m_groups[ SCOREP_SCORE_TYPE_ALL ]->updateProcess( process, bytes, visits, 0, time );
        if ( m_show_regions )
        {
            m_regions[ parentRegion ]->updateProcess( process, bytes, visits, 0, time );
        }
        return;
    }

    /* Calculate total bytes for this call node */
    uint64_t bytes = visits * ( m_bytes_per_visits[ region ]
                                + numParameters * m_bytes_per_num_parameter
                                + strParameters * m_bytes_per_str_parameter )
                     + hits * m_bytes_per_hit;

    m_groups[ group ]->updateProcess( process, bytes, visits, hits, time );
    m_groups[ SCOREP_SCORE_TYPE_ALL ]->updateProcess( process, bytes, visits, hits, time );

    if ( m_show_regions )
    {
        m_regions[ region ]->updateProcess( process, bytes, visits, hits, time );
    }

    if ( m_has_filter )
    {
        bool do_filter = match_filter( region );
        if ( m_show_regions )
        {
            m_regions[ region ]->doFilter( do_filter ?
                                           SCOREP_SCORE_FILTER_YES :
                                           SCOREP_SCORE_FILTER_NO );
        }
        if ( !do_filter )
        {
            m_filtered[ group ]->updateProcess( process, bytes, visits, hits, time );
            m_filtered[ SCOREP_SCORE_TYPE_ALL ]->updateProcess( process, bytes, visits, hits, time );
        }
        else
        {
            m_filtered[ SCOREP_SCORE_TYPE_FLT ]->updateProcess( process, bytes, visits, hits, time );
        }
    }
}

/* ****************************************************** private methods */

void
SCOREP_Score_Estimator::delete_groups( SCOREP_Score_Group** groups,
                                       uint64_t             num )
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
SCOREP_Score_Estimator::initialize_regions( bool useMangled )
{
    m_regions = ( SCOREP_Score_Group** )malloc( m_region_num * sizeof( SCOREP_Score_Group* ) );
    for ( uint64_t region = 0; region < m_region_num; region++ )
    {
        string name = ( useMangled ?
                        m_profile->getMangledName( region ) :
                        m_profile->getRegionName( region ) );
        m_regions[ region ] = new SCOREP_Score_Group( m_profile->getGroup( region ),
                                                      m_process_num,
                                                      name );
    }
}

bool
SCOREP_Score_Estimator::match_filter( uint64_t region )
{
    /* sampled regions wont get filtered */
    int do_filter = 0;
    SCOREP_Filter_Match( m_filter,
                         m_profile->getFileName( region ).c_str(),
                         m_profile->getRegionName( region ).c_str(),
                         m_profile->getMangledName( region ).c_str(),
                         &do_filter );
    return m_profile->getRegionParadigm( region ) != "sampling" && do_filter &&
           SCOREP_Score_getFilterState( m_profile->getGroup( region ) ) != SCOREP_SCORE_FILTER_NO;
}
