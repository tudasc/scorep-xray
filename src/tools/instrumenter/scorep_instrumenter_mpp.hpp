/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_MPP_HPP
#define SCOREP_INSTRUMENTER_MPP_HPP

/**
 * @file
 *
 * Defines the multi-process paradigms.
 */

#include "scorep_instrumenter_selector.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_NoMpp
 * *************************************************************************************/

class SCOREP_Instrumenter_NoMpp : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_NoMpp( SCOREP_Instrumenter_Selector* selector );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_Mpi
 * *************************************************************************************/
class SCOREP_Instrumenter_Mpi : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_Mpi( SCOREP_Instrumenter_Selector* selector );

    virtual void
    checkCompilerName( const std::string& compiler );

    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );

    virtual bool
    checkWrapperOption( const std::string& current,
                        const std::string& next );

    virtual void
    checkObjects( SCOREP_Instrumenter& instrumenter );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_Shmem
 * *************************************************************************************/
class SCOREP_Instrumenter_Shmem : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_Shmem( SCOREP_Instrumenter_Selector* selector );

    virtual void
    checkCompilerName( const std::string& compiler );

    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );

    virtual void
    checkObjects( SCOREP_Instrumenter& instrumenter );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_Mpp
 * *************************************************************************************/
class SCOREP_Instrumenter_Mpp : public SCOREP_Instrumenter_Selector
{
public:
    SCOREP_Instrumenter_Mpp();
};

#endif // SCOREP_INSTRUMENTER_MPP_HPP
