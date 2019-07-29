/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SCORE_GROUP_HPP
#define SCOREP_SCORE_GROUP_HPP

/**
 * @file
 *
 * @brief      Defines a class which represents a group in the
 *             scorep-score tool.
 */

#include "SCOREP_Score_Types.hpp"
#include <string>
#include <stdint.h>

/**
 * This struct is used to keep track of the minimal required field
 * widths of the score output columns.
 */
struct SCOREP_Score_FieldWidths
{
    /**
     * Creates an instance of SCOREP_Score_Field_Widths, initializing
     * all width attributes with their minimal values (based on the
     * column header).
     */
    SCOREP_Score_FieldWidths()
        : m_type( 8 ),
        m_bytes( 10 ),
        m_visits( 6 ),
        m_hits( 4 ),
        m_time( 7 ),
        m_time_per_visit( 14 )
    {
    }

    /**
     * Stores the required width of the 'type' column.
     */
    int m_type;

    /**
     * Stores the required width of the 'max_buf[b]' column.
     */
    int m_bytes;

    /**
     * Stores the required width of the 'visits' column.
     */
    int m_visits;

    /**
     * Stores the required width of the 'hits' column.
     */
    int m_hits;

    /**
     * Stores the required width of the 'time[s]' column.
     */
    int m_time;

    /**
     * Stores the required width of the 'time/visit[us]' column.
     */
    int m_time_per_visit;
};


/**
 * This class represents a group. If the user requested a per
 * region list (with -r) every region is a group, too.
 * It stores relevant data for that group.
 */
class SCOREP_Score_Group
{
public:
    /**
     * Creates an instance of SCOREP_Score_Group.
     * @param type      Specifies the group type (ALL, FLT, OMP, ... ).
     * @param processes Specifies the number processes.
     * @param name      The name of the group or region.
     */
    SCOREP_Score_Group( uint64_t           type,
                        uint64_t           processes,
                        const std::string& name );
    /**
     * Destructor.
     */
    virtual
    ~SCOREP_Score_Group();

    /**
     * Updates the scoring metrics for one process in this group.
     * @param bytes          Number of bytes that are written to the trace.
     * @param numberOfVisits Number of visits for the new region.
     * @param numberOfHits   Number of hits for the new region.
     * @param time           Sum of time spent in this region in all visits.
     * @param process        The process num for which the data added.
     */
    void
    updateProcess( uint64_t process,
                   uint64_t bytes,
                   uint64_t numberOfVisits,
                   uint64_t numberOfHits,
                   double   time );

    /**
     * Updates the field width to the required values.
     * @param widths Current field widths.
     */
    void
    updateWidths( SCOREP_Score_FieldWidths& widths );

    /**
     * Prints the region data to the standard output device.
     * @param totalTime The total time spend in the application.
     * @param widths    Field widths used for printing.
     */
    void
    print( double                   totalTime,
           SCOREP_Score_FieldWidths widths,
           bool                     withHits );

    /**
     * Returns the time spend in this group on all processes.
     */
    double
    getTotalTime( void );

    /**
     * Returns the trace buffer requirements for the regions in this
     * group on the process that required the largest buffer.
     */
    uint64_t
    getMaxTraceBufferSize( void );

    /**
     * Returns the sum of trace buffer requirements for the regions in
     * this group over all processes.
     */
    uint64_t
    getTotalTraceBufferSize( void );

    /**
     * Configures whether filters are used and how this group is affected.
     * @param state  The new filter state.
     */
    void
    doFilter( SCOREP_Score_FilterState state );

private:
    /**
     * Stores the group type.
     */
    uint64_t m_type;

    /**
     * Stores the number of processes.
     */
    uint64_t m_processes;

    /**
     * Stores buffer requirements for each process.
     */
    uint64_t* m_max_buf;

    /**
     * Stores the sum of buffer requirements for all processes.
     */
    uint64_t m_total_buf;

    /**
     * Stores the number of visits for all processes.
     */
    uint64_t m_visits;

    /**
     * Stores the number of hits for all processes.
     */
    uint64_t m_hits;

    /**
     * Stores the total time for all processes.
     */
    double m_total_time;

    /**
     * Stores the group name.
     */
    std::string m_name;

    /**
     * Stores the filter state.
     */
    SCOREP_Score_FilterState m_filter;
};

#endif // SCOREP_SCORE_GROUP_HPP
