/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_HIP_HPP
#define SCOREP_INSTRUMENTER_HIP_HPP

/**
 * @file
 *
 * Defines the class for HIP instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_HipAdapter
 * *************************************************************************************/

/**
 * This class represents the HIP support.
 */
class SCOREP_Instrumenter_HipAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_HipAdapter( void );

    void
    printHelp( void ) override;
    void
    checkCompilerName( const std::string& compiler ) override;
    bool
    checkCommand( const std::string& current,
                  const std::string& next ) override;
    void
    checkObjects( SCOREP_Instrumenter& instrumenter ) override;
    std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine,
                       const std::string&           inputFile ) override;

private:
    /* Is the hipcc compiler used */
    bool m_hipcc_compiler;
};

#endif
