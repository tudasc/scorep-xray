/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_COMPILER_HPP
#define SCOREP_INSTRUMENTER_COMPILER_HPP

/**
 * @file
 *
 * Defines the class for compiler instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_CompilerAdapter
 * *************************************************************************************/

/**
 * This class represents the compiler instrumentation.
 */
class SCOREP_Instrumenter_CompilerAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_CompilerAdapter( void );

    bool
    supportInstrumentFilters( void ) const override;

    std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine,
                       const std::string&           inputFile ) override;

    void
    prelink( SCOREP_Instrumenter&         instrumenter,
             SCOREP_Instrumenter_CmdLine& cmdLine ) override;
};

#endif // SCOREP_INSTRUMENTER_COMPILER_HPP
