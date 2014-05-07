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
 * @brief      Defines a class which contains the base class for
 *             different event representations.
 */

#ifndef SCOREP_SCORE_EVENT_H
#define SCOREP_SCORE_EVENT_H

#include <string>
#include <deque>
#include <map>
#include <set>
#include <stdint.h>

class SCOREP_Score_Profile;

/* **************************************************************************************
 * class SCOREP_Score_Event
 ***************************************************************************************/
class SCOREP_Score_Event
{
    /*------------------------------------------------ public functions */
public:

    /**
     * Regsiter the @p event by its name.
     */
    static void
    RegisterEvent( SCOREP_Score_Event* event );

    /**
     * returns the size estimate for the named event.
     */
    static uint32_t
    GetEventSize( const std::string& eventName );

    /**
     * Sets the size estimate of the named event.
     */
    static void
    SetEventSize( const std::string& name,
                  uint32_t           size );

    /**
     * Constructs an new instance of SCOREP_Score_Event.
     * @param name The name of the event as it appears in OTF2.
     */
    SCOREP_Score_Event( const std::string& name );

    /**
     * Destructor.
     */
    virtual
    ~SCOREP_Score_Event();

    /**
     * Returns the event name.
     */
    virtual const std::string&
    getName( void );

    /*
     * Returns size of the event.
     */
    virtual uint32_t
    getEventSize( void ) const;

    /**
     * Set event size.
     * @param size  Number of bytes for that event.
     */
    virtual void
    setEventSize( uint32_t size );

    /**
     * Returns whether this event occurs in the specified region.
     * @param regionName  The specified regions name.
     */
    virtual bool
    occursInRegion( const std::string& regionName );

    /*------------------------------------------------ protected members */
protected:
    /**
     * Stores the event name.
     */
    std::string m_name;

    /**
     * Stores the event size.
     */
    uint32_t m_size;

public:
    /**
     * Stores all events by its name.
     */
    static std::map< std::string, SCOREP_Score_Event* > m_all_events;
};

/* **************************************************************************************
 * class SCOREP_Score_EnterEvent
 ***************************************************************************************/
class SCOREP_Score_EnterEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_EnterEvent( void );
    virtual bool
    occursInRegion( const std::string& regionName );
};

/* **************************************************************************************
 * class SCOREP_Score_LeaveEvent
 ***************************************************************************************/
class SCOREP_Score_LeaveEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_LeaveEvent( void );
    virtual bool
    occursInRegion( const std::string& regionName );
};

/* **************************************************************************************
 * class SCOREP_Score_MetricEvent
 ***************************************************************************************/
class SCOREP_Score_MetricEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_MetricEvent( uint64_t numDense );
    virtual bool
    occursInRegion( const std::string& regionName );
    virtual void
    setEventSize( uint32_t size );

private:
    uint64_t m_num_dense;
};

/* **************************************************************************************
 * class SCOREP_Score_TimestampEvent
 ***************************************************************************************/
class SCOREP_Score_TimestampEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_TimestampEvent( void );

    virtual bool
    occursInRegion( const std::string& regionName );

    virtual void
    setEventSize( uint32_t size );
};

/* **************************************************************************************
 * class SCOREP_Score_ParameterEvent
 ***************************************************************************************/
class SCOREP_Score_ParameterEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_ParameterEvent( void );
    virtual bool
    occursInRegion( const std::string& regionName );
};

/* **************************************************************************************
 * class SCOREP_Score_NameMatchEvent
 ***************************************************************************************/
class SCOREP_Score_NameMatchEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_NameMatchEvent( const std::string&           eventName,
                                 const std::set<std::string>& regionNames );
    virtual bool
    occursInRegion( const std::string& regionName );

protected:
    std::set<std::string> m_region_names;
};

/* **************************************************************************************
 * class SCOREP_Score_PrefixMatchEvent
 ***************************************************************************************/
class SCOREP_Score_PrefixMatchEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_PrefixMatchEvent( const std::string&             eventName,
                                   const std::deque<std::string>& regionPrefix );
    virtual bool
    occursInRegion( const std::string& regionName );

protected:
    std::deque<std::string> m_region_prefix;
};


#endif /* SCOREP_SCORE_EVENT_H */
