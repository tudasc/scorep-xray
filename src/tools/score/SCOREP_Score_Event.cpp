/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013, 2016, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief      Implements class SCOREP_Score_Event.
 */

#include <config.h>
#include "SCOREP_Score_Event.hpp"
#include "SCOREP_Score_Profile.hpp"

#include <sstream>
#include <utility>

using namespace std;

/* **************************************************************************************
 * class SCOREP_Score_Event
 ***************************************************************************************/

/*------------------------------------------------ public functions */
SCOREP_Score_Event::SCOREP_Score_Event( const string& name )
{
    m_name = name;
    m_size = 0;
}

SCOREP_Score_Event::~SCOREP_Score_Event()
{
}

const string&
SCOREP_Score_Event::getName( void )
{
    return m_name;
}

uint32_t
SCOREP_Score_Event::getEventSize( void ) const
{
    return m_size;
}

void
SCOREP_Score_Event::setEventSize( uint32_t size )
{
    m_size = size;
}

bool
SCOREP_Score_Event::contributes( const SCOREP_Score_Profile& /*profile*/,
                                 uint64_t /*region*/ )
{
    return false;
}

bool
SCOREP_Score_Event::hasTimestamp( void ) const
{
    return true;
}


/* **************************************************************************************
 * class SCOREP_Score_ProgramBeginEvent
 ***************************************************************************************/
SCOREP_Score_ProgramBeginEvent::SCOREP_Score_ProgramBeginEvent( uint64_t numberOfArguments )
    : SCOREP_Score_Event( "ProgramBegin" )
    , m_number_of_arguments( numberOfArguments )
{
    stringstream name;
    name << m_name << " " << m_number_of_arguments;
    m_name = name.str();
}

bool
SCOREP_Score_ProgramBeginEvent::contributes( const SCOREP_Score_Profile& profile,
                                             uint64_t                    region )
{
    if ( profile.isRootRegion( region )
         // The first CUDA or OpenCL event on a GPU location is currently a
         // root region. This will change with #1159. Till then, ignore.
         && profile.getRegionParadigm( region ) != "cuda"
         && profile.getRegionParadigm( region ) != "opencl" )
    {
        return true;
    }
    return false;
}

/* **************************************************************************************
 * class SCOREP_Score_ProgramEndEvent
 ***************************************************************************************/
SCOREP_Score_ProgramEndEvent::SCOREP_Score_ProgramEndEvent( void )
    : SCOREP_Score_Event( "ProgramEnd" )
{
}

bool
SCOREP_Score_ProgramEndEvent::contributes( const SCOREP_Score_Profile& profile,
                                           uint64_t                    region )
{
    if ( profile.isRootRegion( region )
         // The first CUDA or OpenCL event on a GPU location is currently a
         // root region. This will change with #1159. Till then, ignore.
         && profile.getRegionParadigm( region ) != "cuda"
         && profile.getRegionParadigm( region ) != "opencl" )
    {
        return true;
    }
    return false;
}

/* **************************************************************************************
 * class SCOREP_Score_EnterEvent
 ***************************************************************************************/
SCOREP_Score_EnterEvent::SCOREP_Score_EnterEvent( void ) : SCOREP_Score_Event( "Enter" )
{
}

bool
SCOREP_Score_EnterEvent::contributes( const SCOREP_Score_Profile& profile,
                                      uint64_t                    region )
{
    if ( profile.hasHits()
         || profile.omitInTraceEnterLeaveEvents( region ) )
    {
        return false;
    }
    return true;
}

/* **************************************************************************************
 * class SCOREP_Score_LeaveEvent
 ***************************************************************************************/
SCOREP_Score_LeaveEvent::SCOREP_Score_LeaveEvent( void ) : SCOREP_Score_Event( "Leave" )
{
}

bool
SCOREP_Score_LeaveEvent::contributes( const SCOREP_Score_Profile& profile,
                                      uint64_t                    region )
{
    if ( profile.hasHits()
         || profile.omitInTraceEnterLeaveEvents( region ) )
    {
        return false;
    }
    return true;
}

/* **************************************************************************************
 * class SCOREP_Score_CallingContextEnterEvent
 ***************************************************************************************/
SCOREP_Score_CallingContextEnterEvent::SCOREP_Score_CallingContextEnterEvent( void ) : SCOREP_Score_Event( "CallingContextEnter" )
{
}

bool
SCOREP_Score_CallingContextEnterEvent::contributes( const SCOREP_Score_Profile& profile,
                                                    uint64_t                    region )
{
    if ( !profile.hasHits()
         || profile.isDynamicRegion( region ) )
    {
        return false;
    }
    return true;
}

/* **************************************************************************************
 * class SCOREP_Score_CallingContextLeaveEvent
 ***************************************************************************************/
SCOREP_Score_CallingContextLeaveEvent::SCOREP_Score_CallingContextLeaveEvent( void ) : SCOREP_Score_Event( "CallingContextLeave" )
{
}

bool
SCOREP_Score_CallingContextLeaveEvent::contributes( const SCOREP_Score_Profile& profile,
                                                    uint64_t                    region )
{
    if ( !profile.hasHits()
         || profile.isDynamicRegion( region ) )
    {
        return false;
    }
    return true;
}

/* **************************************************************************************
 * class SCOREP_Score_CallingContextLeaveEvent
 ***************************************************************************************/
SCOREP_Score_CallingContextSampleEvent::SCOREP_Score_CallingContextSampleEvent( void ) : SCOREP_Score_Event( "CallingContextSample" )
{
}

/* **************************************************************************************
 * class SCOREP_Score_MetricEvent
 ***************************************************************************************/
SCOREP_Score_MetricEvent::SCOREP_Score_MetricEvent( uint64_t numDense )
    : SCOREP_Score_Event( "Metric" ),
    m_num_dense( numDense )
{
    stringstream name;
    name << m_name << " " << m_num_dense;
    m_name = name.str();
}

bool
SCOREP_Score_MetricEvent::contributes( const SCOREP_Score_Profile& profile,
                                       uint64_t                    region )
{
    if ( profile.isDynamicRegion( region ) )
    {
        return false;
    }
    return true;
}

void
SCOREP_Score_MetricEvent::setEventSize( uint32_t size )
{
    m_size = 2 * size;            /* Regions have two metric sets (enter & exit) */
}

/* **************************************************************************************
 * class SCOREP_Score_TimestampEvent
 ***************************************************************************************/
SCOREP_Score_TimestampEvent::SCOREP_Score_TimestampEvent( void )
    : SCOREP_Score_Event( "Timestamp" )
{
}

bool
SCOREP_Score_TimestampEvent::contributes( const SCOREP_Score_Profile& profile,
                                          uint64_t                    region )
{
    return false; // a timestamp never matches
}

bool
SCOREP_Score_TimestampEvent::hasTimestamp( void ) const
{
    return false; // Number of timestamps are added separately and not during
                  // Event iteration
}

/* **************************************************************************************
 * class SCOREP_Score_ParameterIntEvent
 ***************************************************************************************/
SCOREP_Score_ParameterIntEvent::SCOREP_Score_ParameterIntEvent( void )
    : SCOREP_Score_Event( "ParameterInt" )
{
}

bool
SCOREP_Score_ParameterIntEvent::contributes( const SCOREP_Score_Profile& /* profile */,
                                             uint64_t /* region */ )
{
    return false; // parameters are handled separately
}

/* **************************************************************************************
 * class SCOREP_Score_ParameterStringEvent
 ***************************************************************************************/
SCOREP_Score_ParameterStringEvent::SCOREP_Score_ParameterStringEvent( void )
    : SCOREP_Score_Event( "ParameterString" )
{
}

bool
SCOREP_Score_ParameterStringEvent::contributes( const SCOREP_Score_Profile& /* profile */,
                                                uint64_t /* region */ )
{
    return false; // parameters are handled separately
}

/* **************************************************************************************
 * class SCOREP_Score_NameMatchEvent
 ***************************************************************************************/
SCOREP_Score_NameMatchEvent::SCOREP_Score_NameMatchEvent( const string&      eventName,
                                                          const set<string>& regionNames,
                                                          bool               hasTimestamp )
    : SCOREP_Score_Event( eventName )
{
    m_region_names  = regionNames;
    m_has_timestamp = hasTimestamp;
}

bool
SCOREP_Score_NameMatchEvent::contributes( const SCOREP_Score_Profile& profile,
                                          uint64_t                    region )
{
    return m_region_names.count( profile.getRegionName( region ) ) == 1;
}

bool
SCOREP_Score_NameMatchEvent::hasTimestamp( void ) const
{
    return m_has_timestamp;
}

/* **************************************************************************************
 * class SCOREP_Score_PrefixMatchEvent
 ***************************************************************************************/
SCOREP_Score_PrefixMatchEvent::SCOREP_Score_PrefixMatchEvent(
    const string&        eventName,
    const deque<string>& regionPrefix,
    bool                 hasTimestamp )
    : SCOREP_Score_Event( eventName )
{
    m_region_prefix = regionPrefix;
    m_has_timestamp = hasTimestamp;
}

bool
SCOREP_Score_PrefixMatchEvent::contributes( const SCOREP_Score_Profile& profile,
                                            uint64_t                    region )
{
    for ( deque<string>::iterator i = m_region_prefix.begin();
          i != m_region_prefix.end(); i++ )
    {
        if ( *i == profile.getRegionName( region ).substr( 0, i->length() ) )
        {
            return true;
        }
    }
    return false;
}

bool
SCOREP_Score_PrefixMatchEvent::hasTimestamp( void ) const
{
    return m_has_timestamp;
}
