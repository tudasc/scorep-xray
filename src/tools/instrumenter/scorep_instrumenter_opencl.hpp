/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_OPENCL_HPP
#define SCOREP_INSTRUMENTER_OPENCL_HPP

/**
 * @file
 *
 * Defines the class for OpenCL instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_OpenCLAdapter
 * *************************************************************************************/

/**
 * This class represents the OpenCL support.
 */
class SCOREP_Instrumenter_OpenCLAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_OpenCLAdapter( void );

    void
    printHelp( void );

    std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    bool
    checkOption( const std::string& arg );

    bool
    checkCommand( const std::string& current,
                  const std::string& next );

    bool
    isInterpositionLibrary( const std::string& libraryName );

private:
    bool
    is_opencl_library( const std::string& libraryName );

    std::string m_wrapmode;
};

#endif // SCOREP_INSTRUMENTER_OPENCL_HPP
