/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SCOREP_Score_Estimator.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Defines a class which performs calculations for trace
 *             size estimation.
 */

#ifndef SCOREP_SCORE_ESTIMATOR_H
#define SCOREP_SCORE_ESTIMATOR_H

#include <SCOREP_Score_Profile.hpp>
#include <SCOREP_Score_Group.hpp>

class SCOREP_Score_Estimator
{
public:
    SCOREP_Score_Estimator( SCOREP_Score_Profile* profile,
                            uint32_t              denseBum );
    virtual
    ~SCOREP_Score_Estimator();

    void
    Calculate( bool showRegions );
    void
    PrintGroups();
    void
    PrintRegions();

    void
    InitializeFilter( std::string filterFile );

    void
    DumpEventSizes();

private:
    bool
    match_filter( uint64_t region );

    void
    initialize_regions();

    void
    delete_groups( SCOREP_Score_Group** groups,
                   uint64_t             num );

    uint32_t
    get_compressed_size( uint64_t max_value );

    void
    calculate_event_sizes();

    void
    add_header_size( uint32_t* size );

private:
    bool m_has_filter;

    SCOREP_Score_Profile* m_profile;
    SCOREP_Score_Group**  m_groups;
    SCOREP_Score_Group**  m_regions;
    SCOREP_Score_Group**  m_filtered;

    // Number of definitions
    uint64_t m_region_num;
    uint64_t m_process_num;
    uint32_t m_dense_num;

    // Size of events
    uint32_t m_timestamp;
    uint32_t m_dense;
    uint32_t m_enter;
    uint32_t m_exit;

    uint32_t m_send;
    uint32_t m_isend;
    uint32_t m_isend_complete;
    uint32_t m_irecv_request;
    uint32_t m_recv;
    uint32_t m_irecv;
    uint32_t m_collective;
    uint32_t m_fork;
    uint32_t m_join;
    uint32_t m_acquire_lock;
    uint32_t m_release_lock;
    uint32_t m_task_create;
    uint32_t m_task_switch;
    uint32_t m_task_complete;
    uint32_t m_parameter;
};


#endif // SCOREP_SCORE_ESTIMATOR_H
