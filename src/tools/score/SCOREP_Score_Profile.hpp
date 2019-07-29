/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013, 2015,
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

#ifndef SCOREP_SCORE_PROFILE_HPP
#define SCOREP_SCORE_PROFILE_HPP

/**
 * @file
 *
 * @brief      Defines a class which represents a flat profile in the
 *             scorep-score tool.
 */

#include <string>
#include <set>
#include <Cube.h>
#include "SCOREP_Score_Types.hpp"

class SCOREP_Score_CalltreeVisitor
{
public:
    virtual void
    operator()( uint64_t process,
                uint64_t region,
                uint64_t parentRegion,
                uint64_t visits,
                double   time,
                uint64_t hits,
                uint32_t numParameters,
                uint32_t strParameters ) = 0;
};

/**
 * This class encapsulates the access of the estimator to the CUBE4 profile.
 */
class SCOREP_Score_Profile
{
public:
    /**
     * Creates an instance of SCOREP_Score_Profile.
     * @param cubeFile  The CUBE report.
     *
     *  Creates Score Profile, using previosly set m_cube
     */
    SCOREP_Score_Profile( cube::Cube* cube );

    /**
     * Destructor.
     */
    virtual
    ~SCOREP_Score_Profile();

    /**
     * Predicate whether this profile used sampling.
     */
    bool
    hasHits( void ) const;

    /**
     * Returns sum of the number of visits for an specified region on all processes.
     * @param regionId  ID of the region for which the time is requested.
     */
    uint64_t
    getTotalVisits( uint64_t regionId );

    /**
     * Returns the region name.
     * @param regionId  ID of the region for which the name is requested.
     */
    std::string
    getRegionName( uint64_t regionId ) const;

    /**
     * Returns the mangled region name.
     * @param regionId  ID of the region for which the mangled name is requested.
     */
    std::string
    getMangledName( uint64_t regionId );

    /**
     * Returns the paradigm name.
     * @param regionId  ID of the region for which the paradigm is requested.
     */
    std::string
    getRegionParadigm( uint64_t region ) const;

    /**
     * Returns the name of the source file where a region was implemented.
     * @param regionId  ID of the region for which the source file is requested.
     */
    std::string
    getFileName( uint64_t regionId );

    /**
     * Returns the number of region definitions.
     */
    uint64_t
    getNumberOfRegions( void );

    /**
     * Returns the number of processes.
     */
    uint64_t
    getNumberOfProcesses( void );

    /**
     * Returns the number of locations.
     */
    uint64_t
    getMaxNumberOfLocationsPerProcess( void );

    /**
     * Returns the number of metric definitions.
     */
    uint64_t
    getNumberOfMetrics( void );

    /**
     * Returns the counters of definitions from the profile.
     */
    const std::map<std::string, uint64_t>&
    getDefinitionCounters( void );

    /**
     * Returns the group for a region.
     * @param regionID  ID of the region for which the group is requested.
     */
    SCOREP_Score_Type
    getGroup( uint64_t regionId );

    /**
     * Returns true if @a region is a (callpath's) root region.
     */
    bool
    isRootRegion( uint64_t region ) const;

    /**
     * Returns true if @a region does not contribute to trace enter and leave events.
     * E.g., the program region and MEASUREMENT_OFF
     */
    bool
    omitInTraceEnterLeaveEvents( uint64_t region ) const;

    /**
     * Returns true if @a region is a dynamic region.
     */
    bool
    isDynamicRegion( uint64_t region ) const;

    /**
     * Iterator over call tree of process @a process.
     */
    void
    iterateCalltree( uint64_t                      process,
                     SCOREP_Score_CalltreeVisitor& visitor );

    /**
     * Returns a value >= 0 if the number of program arguments is provided by the cube file.
     * Note: in a future version this function should return independent values per (MPMD) root
     * region and the entire scoring should be per root region as well.
     */
    int64_t
    getNumberOfProgramArguments() const;

private:
    /**
     * Calculates recursively whether a node is on a callpath to an MPI or OpenMP
     * region.
     * @param cnodes  List of callpath nodes from the CUBE report.
     * @param node    The node for which the status is checked.
     * @returns true if @a node appears on a callpath to a MPI or OpenMP region.
     */
    bool
    calculate_calltree_types( cube::Cnode* node );

    void
    iterate_calltree_rec( uint64_t                      process,
                          SCOREP_Score_CalltreeVisitor& visitor,
                          cube::Cnode*                  node );

    /**
     * Checks whether a region is an MPI or OpenMP region.
     * @param regionID  ID of the region for which the type is requested.
     */
    SCOREP_Score_Type
    get_definition_type( uint64_t region );

    /**
     * Checks whether the given string @p str has the prefix @p prefix followed
     * by an upper case letter.
     * @param str     The string to check.
     * @param prefix  The prefix which @p string must have.
     */
    bool
    has_prefix_then_upper( const std::string& str,
                           const std::string& prefix );

    uint64_t
    get_visits( cube::Cnode* node,
                uint64_t     process );

    double
    get_time( cube::Cnode* node,
              uint64_t     process );

    uint64_t
    get_hits( cube::Cnode* node,
              uint64_t     process );

private:
    /**
     * Stores a pointer to the CUBE data structure.
     */
    cube::Cube* m_cube;

    /**
     * Stores the CUBE visits metric definition object.
     */
    cube::Metric* m_visits;

    /**
     * Stores the CUBE time metric definition object.
     */
    cube::Metric* m_time;

    /**
     * Stores the CUBE hits metric definition object.
     */
    cube::Metric* m_hits;

    /**
     * Stores a list of CUBE process obejcts.
     */
    std::vector<cube::Process*> m_processes;

    /**
     * Stores a list of CUBE region objects.
     */
    std::vector<cube::Region*> m_regions;

    /**
     * Stores a mapping of regionIds to region types.
     */
    SCOREP_Score_Type* m_region_types;

    /**
     * Number definitions per definition
     */
    std::map<std::string, uint64_t> m_definition_counters;

    /**
     * Definition specific arguments
     */
    std::map<std::string, uint64_t> m_definition_arguments;

    /**
     * Set of (callpath's) root regions, usually just one. MPMD programs might have several.
     */
    std::set<uint64_t> m_root_regions;

    /**
     * Set of regions that don't contribute to trace enter and leave event.
     */
    std::set<uint64_t> m_regions_to_omit_in_trace_enter_leave_events;

    /**
     * Set of dynamic regions.
     */
    std::set<uint64_t> m_dynamic_regions;

    int64_t m_num_arguments;
};


#endif // SCOREP_SCORE_PROFILE_HPP
