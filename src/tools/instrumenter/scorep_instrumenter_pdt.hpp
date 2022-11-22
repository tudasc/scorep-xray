/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2020, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_PDT_HPP
#define SCOREP_INSTRUMENTER_PDT_HPP

/**
 * @file
 *
 * Defines the class for PDT instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"


/* **************************************************************************************
 * class SCOREP_Instrumenter_PdtAdapter
 * *************************************************************************************/

/**
 * This calss represents the PDT source code instrumentation.
 */
class SCOREP_Instrumenter_PdtAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_PdtAdapter( void );

    std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine,
                       const std::string&           inputFile ) override;

    /**
     * Instruments @a source_file with the Tau instrumentor.
     * @param source_file File name of the source file, that is instrumented.
     * @returns the file name of the instrumented source file.
     */
    std::string
    precompile( SCOREP_Instrumenter&         instrumenter,
                SCOREP_Instrumenter_CmdLine& cmdLine,
                const std::string&           source_file ) override;

    void
    setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine ) override;

    bool
    checkOption( const std::string& arg ) override;

private:
    std::string m_pdt_bin_path;
    std::string m_pdt_config_file;
};

#endif // SCOREP_INSTRUMENTER_PDT_HPP
