/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_LINKTIME_WRAPPING_HPP
#define SCOREP_INSTRUMENTER_LINKTIME_WRAPPING_HPP

#include "scorep_instrumenter_adapter.hpp"

/**
 * @file
 */

/* **************************************************************************************
 * class SCOREP_Instrumenter_LinktimeWrappingAdapter
 * *************************************************************************************/

class SCOREP_Instrumenter_LinktimeWrappingAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    /**
     * Creates a SCOREP_Instrumenter_LinktimeWrappingAdapter instance.
     */
    SCOREP_Instrumenter_LinktimeWrappingAdapter();

    void
    printHelp( void ) override;

    bool
    checkCommand( const std::string& current,
                  const std::string& next ) override;

    std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine,
                       const std::string&           inputFile ) override;
};


#endif // SCOREP_INSTRUMENTER_LINKTIME_WRAPPING_HPP
