/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2015,
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
    supportInstrumentFilters( void ) const;

    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    virtual std::string
    precompile( SCOREP_Instrumenter&         instrumenter,
                SCOREP_Instrumenter_CmdLine& cmdLine,
                const std::string&           source_file );

    virtual void
    prelink( SCOREP_Instrumenter&         instrumenter,
             SCOREP_Instrumenter_CmdLine& cmdLine );
};

#endif // SCOREP_INSTRUMENTER_COMPILER_HPP
