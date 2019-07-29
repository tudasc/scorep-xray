/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2016,2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2015,
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

#ifndef SCOREP_SCORE_ESTIMATOR_HPP
#define SCOREP_SCORE_ESTIMATOR_HPP

/**
 * @file
 *
 * @brief      Defines a class which performs calculations for trace
 *             size estimation.
 */

#include "SCOREP_Score_Group.hpp"
#include "SCOREP_Score_Profile.hpp"


#include <map>
#include <stdint.h>
#include <string>

class SCOREP_Score_Profile;
class SCOREP_Score_Event;
typedef struct SCOREP_Filter SCOREP_Filter;

/**
 * This class implements the estimation logic.
 */
class SCOREP_Score_Estimator
    : public SCOREP_Score_CalltreeVisitor
{
public:
    /**
     * Creates an instance of SCOREP_Score_Estimator.
     * @param profile   A pointer to the profile.
     * @param denseNum  Number of dense metrics that should be recorded in the trace.
     */
    SCOREP_Score_Estimator( SCOREP_Score_Profile* profile,
                            uint64_t              denseNum );

    /**
     * Destructor.
     */
    virtual
    ~SCOREP_Score_Estimator();

    /**
     * Calculates the group an region data.
     * @param showRegions  Pass true if the user wants to see per region data
     *                     in addition to the groups.
     * @param useMangled   Whether mangled or demangled region names are used for
     *                     display.
     */
    void
    calculate( bool showRegions,
               bool useMangled );

    /**
     * Returns bytes per visit of for a region
     * @param region Region index in m_profile
     *
     */
    uint64_t
    bytesPerVisit( uint64_t region );


    /**
     * Prints the group information to the screen.
     */
    void
    printGroups( void );

    /**
     * Prints the per region information to the screen.
     */
    void
    printRegions( void );

    /**
     * Reads and evaluates a filter file.
     * @param filterFile  The name of the filter file.
     */
    void
    initializeFilter( const std::string& filterFile );

    /**
     * Dumps the event sizes to the screen, Used for debug purposes.
     */
    void
    dumpEventSizes( void );

    // SCOREP_Score_CalltreeVisitor
    void
    operator()( uint64_t process,
                uint64_t region,
                uint64_t parentRegion,
                uint64_t visits,
                double   time,
                uint64_t hits,
                uint32_t numParameters,
                uint32_t strParameters );

private:
    /**
     * Checks whether @a region is filtered.
     * @param regionId  Specifies the region by its ID.
     */
    bool
    match_filter( uint64_t regionId );

    /**
     * Initialize per region data.
     * @param useMangled  Whether mangled names or demangled names are used for
     *                    display.
     */
    void
    initialize_regions( bool useMangled );

    /**
     * Delete list of groups.
     * @param groups Pointer to an array of pointer to SCOREP_Score_Group instances.
     * @param num    Number of entries in @a group.
     */
    void
    delete_groups( SCOREP_Score_Group** groups,
                   uint64_t             num );

    /**
     * Initializes the event sizes.
     */
    void
    calculate_event_sizes( void );

    /**
     * Regsiter the @p event by its name.
     */
    void
    registerEvent( SCOREP_Score_Event* event );

    /**
     * returns the size estimate for the named event.
     */
    uint32_t
    getEventSize( const std::string& eventName );

    /**
     * Sets the size estimate of the named event.
     */
    void
    setEventSize( const std::string& name,
                  uint32_t           size );

private:
    /**
     * True, if a filter is used.
     */
    bool m_has_filter;

    /**
     * Filter rules.
     */
    SCOREP_Filter* m_filter;

    /**
     * Stores the pointer to the profile.
     */
    SCOREP_Score_Profile* m_profile;

    /**
     * Array of pointers to the main groups (ALL, USR, MPI, COM, OMP, ...).
     */
    SCOREP_Score_Group** m_groups;

    /**
     * Array of pointers to the regions. NULL if the user does not
     * want to see per region data.
     */
    SCOREP_Score_Group** m_regions;

    /**
     * Array of pointers to the groups that represent the filtered amount
     * of events.
     */
    SCOREP_Score_Group** m_filtered;

    /**
     * Stores the minimal required field widths of the score output.
     */
    SCOREP_Score_FieldWidths m_widths;

    /**
     * Stores the number of region definitions.
     */
    uint64_t m_region_num;

    /**
     * Stores the number of processes.
     */
    uint64_t m_process_num;

    /**
     * Stores the number of dense metrics that should be taken into account.
     */
    uint64_t m_dense_num;

    /**
     * Stores the @p showRegions argument from @a calculate(), so that
     * @a operator() has access to it.
     */
    bool m_show_regions;

    /* Data used when traversing the calltree */
    uint64_t                m_bytes_per_num_parameter;
    uint64_t                m_bytes_per_str_parameter;
    uint64_t                m_bytes_per_hit;
    std::vector< uint64_t > m_bytes_per_visits;

    /**
     * Stores all events by its name.
     */
    std::map< std::string, SCOREP_Score_Event* > m_events;
};


#endif // SCOREP_SCORE_ESTIMATOR_HPP
