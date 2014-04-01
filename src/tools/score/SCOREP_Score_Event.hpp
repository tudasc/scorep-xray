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
     * Constructs an new instance of SCOREP_Score_Event.
     * @param name The name of the event as it appears in OTF2.
     */
    SCOREP_Score_Event( std::string name );

    /**
     * Destructor.
     */
    virtual
    ~SCOREP_Score_Event();

    /**
     * Returns the event name.
     */
    virtual std::string
    getName( void );

    /*
     * Returns size of the event.
     */
    virtual uint32_t
    getEventSize( void );

    /**
     * Set event size.
     * @param name  Name of the event for which the size is given.
     * @param size  Number of bytes for that event.
     */
    virtual void
    setEventSize( std::string name,
                  uint32_t    size );

    /**
     * Returns whether this event occurs in the specified region.
     * @param profile  Pointer to the profile instance.
     * @param regionId Region identifier.
     */
    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );

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
};

/* **************************************************************************************
 * class SCOREP_Score_EnterEvent
 ***************************************************************************************/
class SCOREP_Score_EnterEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_EnterEvent( void );
    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );
};

/* **************************************************************************************
 * class SCOREP_Score_LeaveEvent
 ***************************************************************************************/
class SCOREP_Score_LeaveEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_LeaveEvent( void );
    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );
};

/* **************************************************************************************
 * class SCOREP_Score_MetricEvent
 ***************************************************************************************/
class SCOREP_Score_MetricEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_MetricEvent( void );
    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );
    virtual void
    setEventSize( std::string name,
                  uint32_t    size );
};

/* **************************************************************************************
 * class SCOREP_Score_TimestampEvent
 ***************************************************************************************/
class SCOREP_Score_TimestampEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_TimestampEvent( void );

    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );

    virtual void
    setEventSize( std::string name,
                  uint32_t    size );
};

/* **************************************************************************************
 * class SCOREP_Score_ParameterEvent
 ***************************************************************************************/
class SCOREP_Score_ParameterEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_ParameterEvent( void );
    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );
};

/* **************************************************************************************
 * class SCOREP_Score_NameMatchEvent
 ***************************************************************************************/
class SCOREP_Score_NameMatchEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_NameMatchEvent( std::string             eventName,
                                 std::deque<std::string> regionNames );
    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );

protected:
    std::deque<std::string> m_region_names;
};

/* **************************************************************************************
 * class SCOREP_Score_PrefixMatchEvent
 ***************************************************************************************/
class SCOREP_Score_PrefixMatchEvent : public SCOREP_Score_Event
{
public:
    SCOREP_Score_PrefixMatchEvent( std::string             eventName,
                                   std::deque<std::string> regionPrefix );
    virtual bool
    occursInRegion( SCOREP_Score_Profile* profile,
                    uint64_t              regionId );

protected:
    std::deque<std::string> m_region_prefix;
};


#endif /* SCOREP_SCORE_EVENT_H */
