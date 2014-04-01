/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief      Inmplements class SCOREP_Score_Event.
 */

#include <config.h>
#include "SCOREP_Score_Event.hpp"
#include "SCOREP_Score_Profile.hpp"

/* **************************************************************************************
 * class SCOREP_Score_Event
 ***************************************************************************************/

/*------------------------------------------------ public functions */
SCOREP_Score_Event::SCOREP_Score_Event( std::string name )
{
    m_name = name;
    m_size = 0;
}

SCOREP_Score_Event::~SCOREP_Score_Event()
{
}

std::string
SCOREP_Score_Event::getName( void )
{
    return m_name;
}

uint32_t
SCOREP_Score_Event::getEventSize( void )
{
    return m_size;
}

void
SCOREP_Score_Event::setEventSize( std::string name, uint32_t size )
{
    if ( name == m_name )
    {
        m_size = size;
    }
}

bool
SCOREP_Score_Event::occursInRegion( SCOREP_Score_Profile* profile, uint64_t regionId )
{
    return false;
}

/* **************************************************************************************
 * class SCOREP_Score_EnterEvent
 ***************************************************************************************/
SCOREP_Score_EnterEvent::SCOREP_Score_EnterEvent( void ) : SCOREP_Score_Event( "Enter" )
{
}

bool
SCOREP_Score_EnterEvent::occursInRegion( SCOREP_Score_Profile* profile,
                                         uint64_t              regionId )
{
    std::string region_name = profile->getRegionName( regionId );
    if ( region_name.find( '=', 0 ) == string::npos )
    {
        return true;
    }
    return false; // Is a parameter region which has no enter/exit
}

/* **************************************************************************************
 * class SCOREP_Score_LeaveEvent
 ***************************************************************************************/
SCOREP_Score_LeaveEvent::SCOREP_Score_LeaveEvent( void ) : SCOREP_Score_Event( "Leave" )
{
}

bool
SCOREP_Score_LeaveEvent::occursInRegion( SCOREP_Score_Profile* profile,
                                         uint64_t              regionId )
{
    string region_name = profile->getRegionName( regionId );
    if ( region_name.find( '=', 0 ) == string::npos )
    {
        return true;
    }
    return false; // Is a parameter region which has no enter/exit
}

/* **************************************************************************************
 * class SCOREP_Score_MetricEvent
 ***************************************************************************************/
SCOREP_Score_MetricEvent::SCOREP_Score_MetricEvent( void ) : SCOREP_Score_Event( "Metric" )
{
}

bool
SCOREP_Score_MetricEvent::occursInRegion( SCOREP_Score_Profile* profile,
                                          uint64_t              regionId )
{
    string region_name = profile->getRegionName( regionId );
    if ( region_name.find( '=', 0 ) == string::npos )
    {
        return true;
    }
    return false; // Is a parameter region which has no enter/exit
}

void
SCOREP_Score_MetricEvent::setEventSize( std::string name, uint32_t size )
{
    if ( name == m_name )
    {
        m_size = 2 * size;            /* Regions have two metric sets (enter & exit) */
    }
}

/* **************************************************************************************
 * class SCOREP_Score_TimestampEvent
 ***************************************************************************************/
SCOREP_Score_TimestampEvent::SCOREP_Score_TimestampEvent( void )
    : SCOREP_Score_Event( "Timestamp" )
{
}

bool
SCOREP_Score_TimestampEvent::occursInRegion( SCOREP_Score_Profile* profile,
                                             uint64_t              regionId )
{
    string region_name = profile->getRegionName( regionId );
    if ( region_name.find( '=', 0 ) == string::npos )
    {
        return true;
    }
    return false; // Is a parameter region which has no enter/exit
}

void
SCOREP_Score_TimestampEvent::setEventSize( std::string name, uint32_t size )
{
    if ( name == m_name )
    {
        m_size = 2 * size;               /* Regions have two timestamps */
    }
}

/* **************************************************************************************
 * class SCOREP_Score_ParameterEvent
 ***************************************************************************************/
SCOREP_Score_ParameterEvent::SCOREP_Score_ParameterEvent( void )
    : SCOREP_Score_Event( "ParameterInt" )
{
}

bool
SCOREP_Score_ParameterEvent::occursInRegion( SCOREP_Score_Profile* profile,
                                             uint64_t              regionId )
{
    string region_name = profile->getRegionName( regionId );
    if ( region_name.find( '=', 0 ) == string::npos )
    {
        return false;
    }
    if ( region_name.substr( 0, 9 ) == "instance=" )
    {
        return false;                                        // Dynamic region
    }
    return true;
}

/* **************************************************************************************
 * class SCOREP_Score_NameMatchEvent
 ***************************************************************************************/
SCOREP_Score_NameMatchEvent::SCOREP_Score_NameMatchEvent
(
    std::string             eventName,
    std::deque<std::string> regionNames
) : SCOREP_Score_Event( eventName )
{
    m_region_names = regionNames;
}

bool
SCOREP_Score_NameMatchEvent::occursInRegion( SCOREP_Score_Profile* profile,
                                             uint64_t              regionId )
{
    string region_name = profile->getRegionName( regionId );
    for ( std::deque<std::string>::iterator i = m_region_names.begin();
          i != m_region_names.end(); i++ )
    {
        if ( *i == region_name )
        {
            return true;
        }
    }
    return false;
}

/* **************************************************************************************
 * class SCOREP_Score_PrefixMatchEvent
 ***************************************************************************************/
SCOREP_Score_PrefixMatchEvent::SCOREP_Score_PrefixMatchEvent
(
    std::string             eventName,
    std::deque<std::string> regionPrefix
) : SCOREP_Score_Event( eventName )
{
    m_region_prefix = regionPrefix;
}

bool
SCOREP_Score_PrefixMatchEvent::occursInRegion( SCOREP_Score_Profile* profile,
                                               uint64_t              regionId )
{
    string region_name = profile->getRegionName( regionId );
    for ( std::deque<std::string>::iterator i = m_region_prefix.begin();
          i != m_region_prefix.end(); i++ )
    {
        if ( *i == region_name.substr( 0, i->length() ) )
        {
            return true;
        }
    }
    return false;
}
