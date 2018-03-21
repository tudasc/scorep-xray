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

/**
 * @file
 */

#include <config.h>

#include "scorep_tools_dir.hpp"

#if HAVE( DIRENT )
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <iostream>

#include "scorep_tools_utils.hpp"

using std::string;

SCOREP_Tools_Dir::SCOREP_Tools_Dir()
    :   m_filter( NO_FILTER )
    ,   m_handle( 0 )
    ,   m_goodbit( false )
{
}


SCOREP_Tools_Dir::SCOREP_Tools_Dir( const string& directory,
                                    DirentFilter  filter )
    :   m_directory( directory )
    ,   m_filter( filter )
    ,   m_handle( 0 )
    ,   m_goodbit( false )
{
#if HAVE( DIRENT )
    m_handle = opendir( m_directory.c_str() );
#endif

    next();
}

SCOREP_Tools_Dir::~SCOREP_Tools_Dir()
{
#if HAVE( DIRENT )
    if ( m_handle )
    {
        closedir( ( DIR* )m_handle );
    }
#endif
}

void
SCOREP_Tools_Dir::next()
{
#if HAVE( DIRENT )
    m_goodbit = false;
    while ( m_handle )
    {
        struct dirent* dirent = readdir( ( DIR* )m_handle );
        if ( !dirent )
        {
            closedir( ( DIR* )m_handle );
            m_handle = 0;
            break;
        }

        m_current.name = string( &dirent->d_name[ 0 ] );
        m_current.type = SCOREP_Tools_Dirent::UNKNOWN;

        if ( m_filter & NO_DOT_ENTRIES
             && ( m_current.name == "." || m_current.name == ".." ) )
        {
            continue;
        }

        if ( m_filter & ~NO_DOT_ENTRIES )
        {
            string      full = join_path( m_directory, m_current.name );
            struct stat stat_buf;
            if ( 0 == stat( m_current.name.c_str(), &stat_buf ) )
            {
                if ( S_ISREG( stat_buf.st_mode ) )
                {
                    m_current.type = SCOREP_Tools_Dirent::REGULAR;
                }
                if ( S_ISDIR( stat_buf.st_mode ) )
                {
                    m_current.type = SCOREP_Tools_Dirent::DIRECTORY;
                    if ( m_filter & NO_DIRECTORIES )
                    {
                        continue;
                    }
                }
            }
        }
        m_goodbit = true;
        break;
    }
#endif
}

bool
SCOREP_Tools_Dir::good()
{
    return m_goodbit;
}

SCOREP_Tools_Dirent
SCOREP_Tools_Dir::operator*()
{
    if ( m_goodbit )
    {
        return m_current;
    }
    else
    {
        return SCOREP_Tools_Dirent();
    }
}

bool
SCOREP_Tools_Dir::supported()
{
#if HAVE( DIRENT )
    return true;
#else
    return false;
#endif
}
