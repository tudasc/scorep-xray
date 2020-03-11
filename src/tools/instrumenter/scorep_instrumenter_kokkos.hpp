/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_KOKKOS_HPP
#define SCOREP_INSTRUMENTER_KOKKOS_HPP

/**
 * @file
 *
 * Defines the class for Kokkos instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_KokkosAdapter
 * *************************************************************************************/

/**
 * This class represents the Kokkos support.
 */
class SCOREP_Instrumenter_KokkosAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_KokkosAdapter( void );

    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    virtual void
    printHelp( void );
};

#endif
