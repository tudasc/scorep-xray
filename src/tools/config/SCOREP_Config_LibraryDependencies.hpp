/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file SCOREP_Config_LibraryDependecies.hpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Declares classes for the representation and analysis of library dependencies
 */

#ifndef SCOREP_CONFIG_LIBRARY_DEPENDENCY_HPP
#define SCOREP_CONFIG_LIBRARY_DEPENDENCY_HPP

#include <map>
#include <vector>
#include <string>

class SCOREP_Config_Library;

class SCOREP_Config_LibraryDependencies
{
    // ------------------------------------- Protected types
protected:
    class la_object
    {
public:
        la_object();

        la_object( const la_object& source );

        la_object( std::string              lib_name,
                   std::string              build_dir,
                   std::string              install_dir,
                   std::vector<std::string> libs,
                   std::vector<std::string> ldflags,
                   std::vector<std::string> rpaths,
                   std::vector<std::string> dependency_las );

        virtual
        ~la_object();
public:
        std::string              m_lib_name;
        std::string              m_build_dir;
        std::string              m_install_dir;
        std::vector<std::string> m_libs;
        std::vector<std::string> m_ldflags;
        std::vector<std::string> m_rpath;
        std::vector<std::string> m_dependency_las;
    };


    // ------------------------------------- Public functions
public:
    SCOREP_Config_LibraryDependencies();
    virtual
    ~SCOREP_Config_LibraryDependencies();

    std::string
    GetLibraries( const std::vector<std::string> input_libs );

    std::string
    GetLDFlags( const std::vector<std::string> libs,
                bool                           install );

    std::string
    GetRpathFlags( const std::vector<std::string> libs,
                   bool                           install );

    // ------------------------------------- Protected functions
protected:
    std::vector<std::string>
    get_dependencies( const std::vector<std::string> libs );

    // ------------------------------------- Public members
private:
    std::map< std::string, la_object> la_objects;
};

#endif
