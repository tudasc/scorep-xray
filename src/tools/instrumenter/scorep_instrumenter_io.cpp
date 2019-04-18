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

/**
 * @file
 *
 * Implements selector of the I/O libraries which can be wrapped.
 */

#include <config.h>
#include "scorep_instrumenter_io.hpp"
#include "scorep_instrumenter_paradigm.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_IoParadigm
 * *************************************************************************************/
SCOREP_Instrumenter_IoParadigm::SCOREP_Instrumenter_IoParadigm(
    SCOREP_Instrumenter_Selector* selector,
    const std::string&            name,
    const std::string&            variant,
    const std::string&            description )
    : SCOREP_Instrumenter_Paradigm( selector, name, variant, description )
    , m_wrapmode( SCOREP_LIBWRAP_DEFAULT_MODE )
{
}

SCOREP_Instrumenter_IoParadigm::~SCOREP_Instrumenter_IoParadigm()
{
}

std::string
SCOREP_Instrumenter_IoParadigm::getConfigName( void )
{
    if ( m_wrapmode.size() )
    {
        return m_wrapmode + ":" + getName();
    }
    return getName();
}

bool
SCOREP_Instrumenter_IoParadigm::checkOption( const std::string& _arg )
{
    std::string arg( _arg );
    std::string wrapmode = SCOREP_LIBWRAP_DEFAULT_MODE;

    if ( arg.compare( 0, 9, "linktime:" ) == 0 )
    {
#if !HAVE_BACKEND( LIBWRAP_LINKTIME_SUPPORT )
        std::cerr << "[Score-P] ERROR: Linktime wrapping not support in '--" << _arg << "'" << std::endl;
        exit( EXIT_FAILURE );
#endif
        wrapmode = "linktime";
        arg.erase( 0, 9 );
    }
    else if ( arg.compare( 0, 8, "runtime:" ) == 0 )
    {
#if !HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )
        std::cerr << "[Score-P] ERROR: Runtime wrapping not support in '--" << _arg << "'" << std::endl;
        exit( EXIT_FAILURE );
#endif
        wrapmode = "runtime";
        arg.erase( 0, 8 );
    }

    bool result = SCOREP_Instrumenter_Paradigm::checkOption( arg );
    if ( result )
    {
        m_requires.erase( std::remove( m_requires.begin(), m_requires.end(), SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING ), m_requires.end() );
        if ( wrapmode == "linktime" )
        {
            m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING );
        }
        m_wrapmode = wrapmode;
    }
    return result;
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_NoIo
 * *************************************************************************************/
SCOREP_Instrumenter_NoIo::SCOREP_Instrumenter_NoIo
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "none", "", "No I/O wrapping support." )
{
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Posix
 * *************************************************************************************/
SCOREP_Instrumenter_Posix::SCOREP_Instrumenter_Posix
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_IoParadigm( selector, "posix", "",
                                    "POSIX I/O support using library wrapping. "
                                    "This includes the file descriptor based POSIX API (i.e., `open`/`close`). "
                                    "The POSIX asynchronous I/O API (i.e., `aio_read`/`aio_write`), if available. "
                                    "And the ISO C `FILE` handle based API (i.e., `fopen`/`fclose`)." )
{
#if !( HAVE_BACKEND( POSIX_IO_SUPPORT ) )
    unsupported();
#endif
}

bool
SCOREP_Instrumenter_Posix::isInterpositionLibrary( const std::string& libraryName )
{
    return check_lib_name( libraryName, "rt" );
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Io
 * *************************************************************************************/
SCOREP_Instrumenter_Io::SCOREP_Instrumenter_Io()
    : SCOREP_Instrumenter_Selector( "io", true )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_NoIo( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_Posix( this ) );
    m_current_selection.push_back( m_paradigm_list.front() );
}

void
SCOREP_Instrumenter_Io::printHelp( void )
{
    std::cout << "  --" << m_name << "=[<wrap-mode>:]<paradigm>[:<variant>]";
    if ( m_mode == MULTI_SELECTION )
    {
        std::cout << "(,[<wrap-mode>:]<paradigm>[:<variant>])*";
    }
    std::cout << "\n";
    std::cout << "                  <wrap-mode> may be 'linktime' or 'runtime'.\n";
    std::cout << "                  The default is the first supported mode in the above order.\n";
    std::cout << "                  Possible paradigms and variants are:\n";

    SCOREP_Instrumenter_ParadigmList::iterator paradigm;
    for ( paradigm = m_paradigm_list.begin();
          paradigm != m_paradigm_list.end();
          paradigm++ )
    {
        ( *paradigm )->printHelp();
    }
}
