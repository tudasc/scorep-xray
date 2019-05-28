/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_CUDA_HPP
#define SCOREP_INSTRUMENTER_CUDA_HPP

/**
 * @file
 *
 * Defines the class for cuda instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_CudaAdapter
 * *************************************************************************************/

/**
 * This class represents the Cuda support.
 */
class SCOREP_Instrumenter_CudaAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_CudaAdapter( void );
    virtual void
    printHelp( void );
    virtual void
    checkCompilerName( const std::string& compiler );
    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );
    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );
    bool
    isNvcc( void );

private:
    /* Is the nvcc compiler used to drive the compilation/linking */
    bool m_nvcc_compiler;
};

#endif // SCOREP_INSTRUMENTER_CUDA_HPP
