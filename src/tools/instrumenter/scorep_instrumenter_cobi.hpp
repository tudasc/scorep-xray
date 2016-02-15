/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_COBI_HPP
#define SCOREP_INSTRUMENTER_COBI_HPP

/**
 * @file
 *
 * Defines the class for the cobi instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_CobiAdapter
 * *************************************************************************************/

/**
 * This class represents the cobi instrumentation.
 */
class SCOREP_Instrumenter_CobiAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_CobiAdapter( void );

    /**
     * Invoke the binary instrumenter Cobi. Instruments the binary
     * @a orig_name. The instrumented file will be named @a output_name.
     * @param orig_name   The name of the uninstrumented exectable
     * @param output_name The name of the instrumented executable
     * @return The return value of the cobi execution.
     */
    virtual void
    postlink( SCOREP_Instrumenter&         instrumenter,
              SCOREP_Instrumenter_CmdLine& cmdLine );
    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    virtual void
    setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine );

    virtual void
    setConfigValue( const std::string& key,
                    const std::string& value );

private:
    std::string m_cobi;
    std::string m_cobi_config_dir;
};

#endif // SCOREP_INSTRUMENTER_COBI_HPP
