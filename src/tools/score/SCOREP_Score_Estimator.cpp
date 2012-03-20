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
 * @status     alpha
 * @file       SCOREP_Score_Estimator.cxx
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Implements a class which performs calculations for trace
 *             size estimation.
 */

#include <config.h>
#include <math.h>
#include <SCOREP_Score_Estimator.hpp>

using namespace std;

SCOREP_Score_Estimator::SCOREP_Score_Estimator( SCOREP_Score_Profile* profile )
{
    m_profile = profile;
    m_regions = profile->GetNumberOfRegions();

    m_timestamp = 8;
    m_dense     = calculate_dense_metric();

    m_enter = calculate_enter();
    m_exit  = calculate_exit();
}

SCOREP_Score_Estimator::~SCOREP_Score_Estimator()
{
}

void
SCOREP_Score_Estimator::DumpEventSizes()
{
    cout << "Dense metrics: " << m_dense << endl;
    cout << "Enter:         " << m_enter << endl;
    cout << "Exit:          " << m_exit  << endl;
}

uint32_t
SCOREP_Score_Estimator::calculate_enter()
{
    return m_timestamp +                      // timestamp
           get_compressed_size( m_regions ) + // region handle
           m_dense;                           // metrics
}

uint32_t
SCOREP_Score_Estimator::calculate_exit()
{
    return m_timestamp +                      // timestamp
           get_compressed_size( m_regions ) + // region handle
           m_dense;                           // metrics
}

uint32_t
SCOREP_Score_Estimator::calculate_dense_metric()
{
    // How to determine the number of dense metricss ?
    uint32_t number = 0;

    if ( number == 0 )
    {
        return 0;
    }
    return get_compressed_size( m_profile->GetNumberOfMetrics() ) + // handle
           1 +                                                      // number
           number +                                                 // type ids
           number * 8;                                              // values
}

uint32_t
SCOREP_Score_Estimator::get_compressed_size( uint64_t max_value )
{
    return ( uint32_t )1 +                  // Number of leading zeros
           ceil( log10( max_value ) );      // Number
}
