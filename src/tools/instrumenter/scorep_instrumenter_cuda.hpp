/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_CUDA_HPP
#define SCOREP_INSTRUMENTER_CUDA_HPP

/**
 * @file scorep_instrumenter_cuda.hpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
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
    virtual std::string
    getConfigToolFlag( void );
};

#endif
