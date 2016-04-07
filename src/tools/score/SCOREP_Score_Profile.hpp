/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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
#include <Cube.h>
#include "SCOREP_Score_Types.hpp"

/**
 * This class encapsulates the access of the estimator to the CUBE4 profile.
 */
class SCOREP_Score_Profile
{
public:
    /**
     * Creates an instance of SCOREP_Score_Profile.
     * @param cubeFile  The file name of the CUBE report.
     */
    SCOREP_Score_Profile( std::string cubeFile );

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
     * Returns sum of the time that an application spent in a region on all processes.
     * @param regionId  ID of the region for which the time is requested.
     */
    double
    getTotalTime( uint64_t regionId );

    /**
     * Returns the number of visits to a region on a specified process.
     * @param regionId  ID of the region for which the number of visits are requested.
     * @param process   The process number fo which the number of visits are requested.
     */
    uint64_t
    getVisits( uint64_t regionId,
               uint64_t process );

    /**
     * Returns the time that a region spent on a specified process.
     * @param regionId  ID of the region for which the time is requested.
     * @param process   The process number fo which the time is requested.
     */
    double
    getTime( uint64_t regionId,
             uint64_t process );

    /**
     * Returns the number of sampling hits to a region on a specified process.
     * @param regionId  ID of the region for which the number of hits are requested.
     * @param process   The process number fo which the number of hits are requested.
     */
    uint64_t
    getHits( uint64_t regionId,
             uint64_t process );

    /**
     * Returns sum of the number of visits for an specified region on all processes.
     * @param regionId  ID of the region for which the time is requested.
     */
    uint64_t
    getTotalVisits( uint64_t regionId );

    /**
     * Returns the maximum number of visits to a region on one process over all processes.
     * @param regionId  ID of the region for which the visits are requested.
     */
    uint64_t
    getMaxVisits( uint64_t regionId );

    /**
     * Returns the region name.
     * @param regionId  ID of the region for which the name is requested.
     */
    std::string
    getRegionName( uint64_t regionId );

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
    getRegionParadigm( uint64_t region );

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
     * Prints some basic infos on the profile. Used for debug purposes.
     */
    void
    print( void );

    /**
     * Returns the group for a region.
     * @param regionID  ID of the region for which the group is requested.
     */
    SCOREP_Score_Type
    getGroup( uint64_t regionId );

    /**
     * Returns the size of the profile report file.
     */
    uint64_t
    getFileSize( void );

private:
    /**
     * Calculates recursively whether a node is on a callpath to an MPI or OpenMP
     * region.
     * @param cnodes  List of callpath nodes from the CUBE report.
     * @param node    The node for which the status is checked.
     * @returns true if @a node appears on a callpath to a MPI or OpenMP region.
     */
    bool
    calculate_calltree_types( const std::vector<cube::Cnode*>* cnodes,
                              cube::Cnode*                     node );

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
    has_prefix_than_upper( const std::string& str,
                           const std::string& prefix );

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
     * Stores the size of the CUBE report file.
     */
    uint64_t m_file_size;

    /**
     * Number definitions per definition
     */
    std::map<std::string, uint64_t> m_definition_counters;
};


#endif // SCOREP_SCORE_PROFILE_HPP
