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
 * @file SCOREP_Config_LibraryDependecies.cpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Implements the representation and analysis of library dependencies
 */

#include <config.h>
#include <SCOREP_Config_LibraryDependencies.hpp>

#include <iostream>
#include <stdlib.h>

using namespace std;

/* **************************************************************************************
                                                                          local functions
****************************************************************************************/

static bool
has_item( const vector<string> input, string item )
{
    vector<string>::const_iterator i;
    for ( i = input.begin(); i < input.end(); i++ )
    {
        if ( *i == item )
        {
            return true;
        }
    }
    return false;
}

static vector<string>
remove_double_entries( const vector<string> input )
{
    vector<string>                         inverse;
    vector<string>::const_reverse_iterator i;
    for ( i = input.rbegin(); i < input.rend(); i++ )
    {
        if ( !has_item( inverse, *i ) )
        {
            inverse.push_back( *i );
        }
    }

    vector<string> output;
    for ( i = inverse.rbegin(); i < inverse.rend(); i++ )
    {
        output.push_back( *i );
    }
    return output;
}

static string
vector_to_string( const vector<string> input )
{
    string                         output;
    vector<string>::const_iterator i;
    for ( i = input.begin(); i < input.end(); i++ )
    {
        output += " " + *i;
    }
    return output;
}

/* **************************************************************************************
                                                                          class la_object
****************************************************************************************/

SCOREP_Config_LibraryDependencies::la_object::la_object()
{
}

SCOREP_Config_LibraryDependencies::la_object::la_object( const la_object &source )
{
    m_lib_name       = source.m_lib_name;
    m_build_dir      = source.m_build_dir;
    m_install_dir    = source.m_install_dir;
    m_libs           = source.m_libs;
    m_ldflags        = source.m_ldflags;
    m_rpath          = source.m_rpath;
    m_dependency_las = source.m_dependency_las;
}


SCOREP_Config_LibraryDependencies::la_object::la_object( string         lib_name,
                                                         string         build_dir,
                                                         string         install_dir,
                                                         vector<string> libs,
                                                         vector<string> ldflags,
                                                         vector<string> rpath,
                                                         vector<string> dependency_las )
{
    m_lib_name       = lib_name;
    m_build_dir      = build_dir;
    m_install_dir    = install_dir;
    m_libs           = libs;
    m_ldflags        = ldflags;
    m_rpath          = rpath;
    m_dependency_las = dependency_las;
}

SCOREP_Config_LibraryDependencies::la_object::~la_object()
{
}

/* **************************************************************************************
                                                  class SCOREP_Config_LibraryDependencies
****************************************************************************************/

SCOREP_Config_LibraryDependencies::SCOREP_Config_LibraryDependencies()
{
    vector<string> libs;
    vector<string> ldflags;
    vector<string> rpaths;
    vector<string> dependency_las;

  #include <scorep_library_dependencies.cpp>
}

SCOREP_Config_LibraryDependencies::~SCOREP_Config_LibraryDependencies()
{
}

string
SCOREP_Config_LibraryDependencies::GetLibraries( const vector<string> input_libs )
{
    vector<string>           deps = get_dependencies( input_libs );
    vector<string>           libs;
    vector<string>::iterator i;
    for ( i = deps.begin(); i < deps.end(); i++ )
    {
        la_object obj = la_objects[ *i ];
        libs.push_back( "-l" + obj.m_lib_name.substr( 3 ) );
        libs.insert( libs.end(),
                     obj.m_libs.begin(),
                     obj.m_libs.end() );
    }
    libs = remove_double_entries( libs );

    return vector_to_string( libs );
}

string
SCOREP_Config_LibraryDependencies::GetLDFlags( const vector<string> libs, bool install )
{
    vector<string>           deps = get_dependencies( libs );
    vector<string>           flags;
    vector<string>::iterator i;
    for ( i = deps.begin(); i < deps.end(); i++ )
    {
        la_object obj = la_objects[ *i ];
        if ( install )
        {
            flags.push_back( "-L" + obj.m_install_dir );
        }
        else
        {
            flags.push_back( "-L" + obj.m_build_dir );
        }
        flags.insert( flags.end(),
                      obj.m_ldflags.begin(),
                      obj.m_ldflags.end() );
    }
    flags = remove_double_entries( flags );

    return vector_to_string( flags );
}

string
SCOREP_Config_LibraryDependencies::GetRpathFlags( const vector<string> libs, bool install )
{
    vector<string>           deps = get_dependencies( libs );
    vector<string>           flags;
    vector<string>::iterator i;
    for ( i = deps.begin(); i < deps.end(); i++ )
    {
        la_object obj = la_objects[ *i ];
        if ( install )
        {
            flags.push_back( "-Wl,-rpath," + obj.m_install_dir );
        }
        else
        {
            flags.push_back( "-Wl,-rpath," + obj.m_build_dir );
        }
        flags.insert( flags.end(),
                      obj.m_rpath.begin(),
                      obj.m_rpath.end() );
    }
    flags = remove_double_entries( flags );

    return vector_to_string( flags );
}

vector<string>
SCOREP_Config_LibraryDependencies::get_dependencies( const vector<string> libs )
{
    vector<string> deps = libs;
    //vector<string>::iterator i;
    //for (i=deps.begin(); i < deps.end(); i++)
    for ( int i = 0; i < deps.size(); i++ )
    {
        if ( la_objects.find( deps[ i ] ) == la_objects.end() )
        {
            cerr << "ERROR: Can not resolve dependency \"" << deps[ i ] << "\"" << endl;
            exit( EXIT_FAILURE );
        }
        la_object obj = la_objects[ deps[ i ] ];

        deps.insert( deps.end(),
                     obj.m_dependency_las.begin(),
                     obj.m_dependency_las.end() );
    }
    return remove_double_entries( deps );
}
