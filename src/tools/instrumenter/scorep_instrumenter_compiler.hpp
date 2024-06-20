/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2022-2024,
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

#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_LLVM_PLUGIN )
#include <vector>
#endif

#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_XRAY_PLUGIN )
#include "scorep_xray_config.h"
#include <vector>
#endif

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

    void
    printHelp( void ) override;

    bool
    supportInstrumentFilters( void ) const override;

    std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine,
                       const std::string&           inputFile ) override;

    bool
    checkOption( const std::string& arg ) override;

    void
    prelink( SCOREP_Instrumenter&         instrumenter,
             SCOREP_Instrumenter_CmdLine& cmdLine ) override;

#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_LLVM_PLUGIN )
private:
    std::vector<std::string> m_llvm_plugin_args;
#endif
#if HAVE_BACKEND( SCOREP_COMPILER_INSTRUMENTATION_XRAY_PLUGIN )
    private:
    XRayPlugin::Config xrayConfig;
    std::vector<std::string> m_xray_plugin_args;
#endif
};

#endif // SCOREP_INSTRUMENTER_COMPILER_HPP
