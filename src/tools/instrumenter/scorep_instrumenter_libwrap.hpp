/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_LIBWRAP_HPP
#define SCOREP_INSTRUMENTER_LIBWRAP_HPP

/**
 * @file
 *
 * Defines the base class for an instrumentation method.
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdint.h>

#include "scorep_instrumenter_adapter.hpp"
#include <scorep_tools_config_parser.hpp>

/* **************************************************************************************
 * class SCOREP_Instrumenter_LibwrapAdapter
 * *************************************************************************************/

class SCOREP_Instrumenter_LibwrapAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    /**
     * Creates a SCOREP_Instrumenter_LibwrapAdapter instance.
     */
    SCOREP_Instrumenter_LibwrapAdapter();

    void
    printHelp( void );

    bool
    checkOption( const std::string& arg );

    std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    bool
    isInterpositionLibrary( const std::string& libraryName );

private:
    void
    add_libwrap( const std::string& wrapmode,
                 const std::string& arg,
                 const std::string& anchor );

    /**
     * Paths out of SCOREP_LIBWRAP_PATH.
     */
    std::vector<std::string> m_path;

    struct Wrapper
        : private SCOREP_Tools_ConfigParser
    {
        Wrapper( const std::string& anchor );

        std::string
        getAnchor( void );

        void
        set_value( const std::string& key,
                   const std::string& value );

        std::string                        m_prefix;
        std::string                        m_name;
        std::map<std::string, std::string> m_values;
        std::set<std::string>              m_libs;
        std::vector<std::string>           m_dlopen_libs;
    };

    /**
     * Currently selected libwrapper.
     */
    std::vector<std::pair<std::string, Wrapper> > m_wrappers;
};

#endif // SCOREP_INSTRUMENTER_LIBWRAP_HPP
