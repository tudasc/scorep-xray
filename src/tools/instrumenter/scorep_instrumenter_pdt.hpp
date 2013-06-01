/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_PDT_HPP
#define SCOREP_INSTRUMENTER_PDT_HPP

/**
 * @file scorep_instrumenter_pdt.hpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
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

    virtual std::string
    getConfigToolFlag( void );

    /**
     * Instruments @a source_file with the Tau instrumentor.
     * @param source_file File name of the source file, that is instrumented.
     * @returns the file name of the instrumented source file.
     */
    virtual std::string
    precompile( SCOREP_Instrumenter&         instrumenter,
                SCOREP_Instrumenter_CmdLine& cmdLine,
                const std::string&           source_file );

    virtual void
    setBuildCheck( void );

    virtual void
    setConfigValue( const std::string& key,
                    const std::string& value );

private:
    void
    set_pdt_path( const std::string& pdt );

private:
    std::string m_pdt_bin_path;
    std::string m_pdt_config_file;
};

#endif