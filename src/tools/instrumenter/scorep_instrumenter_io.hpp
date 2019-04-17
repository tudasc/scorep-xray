/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2018,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_IO_HPP
#define SCOREP_INSTRUMENTER_IO_HPP

/**
 * @file
 *
 * Defines the I/O libraries which can be wrapped.
 */

#include "scorep_instrumenter_selector.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_IoParadigm
 * *************************************************************************************/

class SCOREP_Instrumenter_IoParadigm : public SCOREP_Instrumenter_Paradigm
{
    /* ----------------------------------------------------------------------- methods */
public:
    /**
     * Creates a SCOREP_Instrumenter_Paradigm instance.
     * @param selector    The selector for the paradigm group of this paradigm.
     * @param name        The name of the paradigm.
     * @param variant     The name of the paradigm variant or implementation.
     * @param description Description of the paradigm which is displayed on the help
     *                    output.
     */
    SCOREP_Instrumenter_IoParadigm( SCOREP_Instrumenter_Selector* selector,
                                    const std::string&            name,
                                    const std::string&            variant,
                                    const std::string&            description );

    /**
     * Deletes a SCOREP_Instrumenter_IoParadigm instance.
     */
    virtual
    ~SCOREP_Instrumenter_IoParadigm();

    /**
     * Returns the name as used for the selection of the scorep-config tool.
     */
    virtual std::string
    getConfigName( void );

    /**
     * Evaluates an scorep option for this paradigm.
     * Overwrite this function if you need to process different or more parameters.
     * Handles also the 'linktime:' and 'runtime:' prefixes.
     * @param arg  The option that is evaluated.
     * @returns true, if this argument selects this paradigm.
     */
    virtual bool
    checkOption( const std::string& arg );

    /* ----------------------------------------------------------------------- members */
private:
    std::string m_wrapmode;
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_NoIo
 * *************************************************************************************/

class SCOREP_Instrumenter_NoIo : public SCOREP_Instrumenter_Paradigm
{
public:
    SCOREP_Instrumenter_NoIo( SCOREP_Instrumenter_Selector* selector );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_Posix
 * *************************************************************************************/
class SCOREP_Instrumenter_Posix : public SCOREP_Instrumenter_IoParadigm
{
public:
    SCOREP_Instrumenter_Posix( SCOREP_Instrumenter_Selector* selector );

    bool
    isInterpositionLibrary( const std::string& libraryName );
};

/* **************************************************************************************
 * class SCOREP_Instrumenter_Io
 * *************************************************************************************/
class SCOREP_Instrumenter_Io : public SCOREP_Instrumenter_Selector
{
public:
    SCOREP_Instrumenter_Io();

    void
    printHelp( void );
};

#endif
