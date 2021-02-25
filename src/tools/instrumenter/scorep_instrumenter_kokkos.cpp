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

/**
 * @file
 *
 * Implements the class for Kokkos instrumentation.
 */

#include <config.h>

#include "scorep_instrumenter_kokkos.hpp"

#include <iostream>

/* **************************************************************************************
 * class SCOREP_Instrumenter_KokkosAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_KokkosAdapter::SCOREP_Instrumenter_KokkosAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_KOKKOS, "kokkos" )
{
#if !HAVE_BACKEND( KOKKOS_SUPPORT )
    unsupported();
#endif
}

std::string
SCOREP_Instrumenter_KokkosAdapter::getConfigToolFlag( SCOREP_Instrumenter_CmdLine& /* cmdLine */ )
{
    std::string flags;

    if ( !isEnabled() )
    {
        flags += " --no" + m_name;
    }

    return flags;
}
void
SCOREP_Instrumenter_KokkosAdapter::printHelp( void )
{
    std::cout << "  --kokkos        Enables Kokkos instrumentation.\n"
              << "  --nokokkos      Disables Kokkos instrumentation."
              << std::endl;
}
