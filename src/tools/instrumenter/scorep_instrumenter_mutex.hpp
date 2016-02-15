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

#ifndef SCOREP_INSTRUMENTER_MUTEX_HPP
#define SCOREP_INSTRUMENTER_MUTEX_HPP

/**
 * @file
 *
 * Defines mutex locks.
 */

#include "scorep_instrumenter_selector.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexMockup
 * *************************************************************************************/

class SCOREP_Instrumenter_MutexMockup : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_MutexMockup( SCOREP_Instrumenter_Selector* selector );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexOmp
 * *************************************************************************************/
class SCOREP_Instrumenter_MutexOmp : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_MutexOmp( SCOREP_Instrumenter_Selector* selector );

    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexPthread
 * *************************************************************************************/
class SCOREP_Instrumenter_MutexPthread : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_MutexPthread( SCOREP_Instrumenter_Selector* selector );

    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexPthreadWrap
 * *************************************************************************************/
class SCOREP_Instrumenter_MutexPthreadWrap : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_MutexPthreadWrap( SCOREP_Instrumenter_Selector* selector );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_MutexPthreadSpinlock
 * *************************************************************************************/
class SCOREP_Instrumenter_MutexPthreadSpinlock : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_MutexPthreadSpinlock( SCOREP_Instrumenter_Selector* selector );

    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_Mutex
 * *************************************************************************************/
class SCOREP_Instrumenter_Mutex : public SCOREP_Instrumenter_Selector
{
public:
    SCOREP_Instrumenter_Mutex();
};

#endif // SCOREP_INSTRUMENTER_MUTEX_HPP
