/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_OPENACC_HPP
#define SCOREP_INSTRUMENTER_OPENACC_HPP

/**
 * @file
 *
 * Defines the class for OpenACC instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_OpenACCAdapter
 * *************************************************************************************/

/**
 * This class represents the OpenACC support.
 */
class SCOREP_Instrumenter_OpenACCAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_OpenACCAdapter( void );

    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );
    virtual void
    printHelp( void );
};

#endif
