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

#ifndef SCOREP_TOOLS_DIR_HPP
#define SCOREP_TOOLS_DIR_HPP

/**
 * @file       scorep_tools_dir.hpp
 *
 * @brief      Helper for directory traversal
 */

#include <string>

/** @brief Represents a directory entry. */
struct SCOREP_Tools_Dirent
{
    /** @brief Possible types of directory entries */
    enum Type
    {
        /** @brief Directory entry of unknown type. */
        UNKNOWN = 0,
        /** @brief Directory entry is a regular file. */
        REGULAR,
        /** @brief Directory entry is a directory. */
        DIRECTORY
    };

    /** @brief Default constructor */
    SCOREP_Tools_Dirent()
        :   type( UNKNOWN )
    {
    }

    /** @brief Copy constructor */
    SCOREP_Tools_Dirent( const SCOREP_Tools_Dirent& other )
        :   name( other.name )
        ,   type( other.type )
    {
    }

    /** @brief Assignment operator. */
    void
    operator=( const SCOREP_Tools_Dirent& rhs )
    {
        name = rhs.name;
        type = rhs.type;
    }

    /** @brief The name of this entry. Not the full path. */
    std::string name;
    /** @brief The type of this entry. */
    Type        type;
};

/** @brief Directory lister. */
class SCOREP_Tools_Dir
{
public:
    /** @brief Possible filters to restrict the directory entries. */
    enum DirentFilter
    {
        /** @brief No filter applied, all entries will be listed. */
        NO_FILTER      = 0,
        /** @brief Ignore entries starting with a '.' in their name */
        NO_DOT_ENTRIES = 1,
        /** @brief Filter entries of type directory out. */
        NO_DIRECTORIES = 2
    };

    /**
     * @brief Constructs a directory lister for the given directory and filter options.
     *
     * @param directory The directory to list.
     * @param filter    The filter to apply.
     */
    SCOREP_Tools_Dir( const std::string& directory,
                      DirentFilter       filter = NO_FILTER );

    /** @brief Destructor */
    ~SCOREP_Tools_Dir();

    /** @brief Is the lister in a good state to list more entries? */
    bool
    good();

    /** @brief Advance the lister to the next entry. */
    void
    next();

    /**
     * @brief Get access to the current directory entry.
     *
     * dir.good() must be hold, before calling this.
     */
    SCOREP_Tools_Dirent
    operator*();

    /** @brief Is directory listing supported? */
    static bool
    supported();

private:
    SCOREP_Tools_Dir();

    std::string         m_directory;
    DirentFilter        m_filter;
    void*               m_handle;
    SCOREP_Tools_Dirent m_current;
    bool                m_goodbit;
};

/**
 * @brief Stream oriented access to a directory lister.
 *
 * dir.good() must be hold, before calling this.
 */
inline SCOREP_Tools_Dir&
operator>>( SCOREP_Tools_Dir&    dir,
            SCOREP_Tools_Dirent& dirent )
{
    dirent = *dir;
    dir.next();
    return dir;
}

/**
 * @brief Support bit oprations on SCOREP_Tools_Dir::DirentFilter.
 * @{
 */

inline SCOREP_Tools_Dir::DirentFilter
operator~( SCOREP_Tools_Dir::DirentFilter a )
{
    return ( SCOREP_Tools_Dir::DirentFilter )~( unsigned )a;
}

inline SCOREP_Tools_Dir::DirentFilter
operator|( SCOREP_Tools_Dir::DirentFilter a,
           SCOREP_Tools_Dir::DirentFilter b )
{
    return ( SCOREP_Tools_Dir::DirentFilter )( ( unsigned )a | ( unsigned )b );
}

inline SCOREP_Tools_Dir::DirentFilter
operator&( SCOREP_Tools_Dir::DirentFilter a,
           SCOREP_Tools_Dir::DirentFilter b )
{
    return ( SCOREP_Tools_Dir::DirentFilter )( ( unsigned )a & ( unsigned )b );
}

inline SCOREP_Tools_Dir::DirentFilter
operator^( SCOREP_Tools_Dir::DirentFilter a,
           SCOREP_Tools_Dir::DirentFilter b )
{
    return ( SCOREP_Tools_Dir::DirentFilter )( ( unsigned )a ^ ( unsigned )b );
}

inline SCOREP_Tools_Dir::DirentFilter&
operator|=( SCOREP_Tools_Dir::DirentFilter& a,
            SCOREP_Tools_Dir::DirentFilter  b )
{
    return ( SCOREP_Tools_Dir::DirentFilter& )( ( unsigned& )a |= ( unsigned )b );
}

inline SCOREP_Tools_Dir::DirentFilter&
operator&=( SCOREP_Tools_Dir::DirentFilter& a,
            SCOREP_Tools_Dir::DirentFilter  b )
{
    return ( SCOREP_Tools_Dir::DirentFilter& )( ( unsigned& )a &= ( unsigned )b );
}

inline SCOREP_Tools_Dir::DirentFilter&
operator^=( SCOREP_Tools_Dir::DirentFilter& a,
            SCOREP_Tools_Dir::DirentFilter  b )
{
    return ( SCOREP_Tools_Dir::DirentFilter& )( ( unsigned& )a ^= ( unsigned )b );
}

/**
 * @}
 */

#endif // SCOREP_TOOLS_DIR_HPP
