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
    SCOREP_Score_Estimator( SCOREP_Score_Profile* profile );
    virtual
    ~SCOREP_Score_Estimator();

    void
    CalculateGroups();

    void
    Print();

    void
    DumpEventSizes();

private:
    uint32_t
    get_compressed_size( uint64_t max_value );

    // Calculate event sizes
    uint32_t
    calculate_dense_metric();
    uint32_t
    calculate_enter();
    uint32_t
    calculate_exit();

private:
    SCOREP_Score_Profile* m_profile;
    SCOREP_Score_Group**  m_groups;

    // Number of definitions
    uint64_t m_regions;
    uint64_t m_processes;

    // Size of events
    uint32_t m_timestamp;
    uint32_t m_dense;
    uint32_t m_enter;
    uint32_t m_exit;
};


#endif // SCOREP_SCORE_ESTIMATOR_H
