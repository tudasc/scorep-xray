/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2013-2014, 2017,
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
 * Implements multi-process paradigms.
 */

#include <config.h>
#include "scorep_instrumenter_mpp.hpp"
#include "scorep_instrumenter_paradigm.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <scorep_config_tool_shmem.h>

#include <scorep_tools_utils.hpp>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_NoMpp
 * *************************************************************************************/
SCOREP_Instrumenter_NoMpp::SCOREP_Instrumenter_NoMpp
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "none", "", "No multi-process support." )
{
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Mpi
 * *************************************************************************************/
SCOREP_Instrumenter_Mpi::SCOREP_Instrumenter_Mpi
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "mpi", "",
                                  "MPI support using library wrapping" )
{
    if ( !SCOREP_HAVE_PDT_MPI_INSTRUMENTATION )
    {
        m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );
    }
#if !( HAVE_BACKEND( MPI_SUPPORT ) )
    unsupported();
#endif
}

void
SCOREP_Instrumenter_Mpi::checkCompilerName( const std::string& compiler )
{
    if ( remove_path( compiler ).substr( 0, 2 ) == "mp" )
    {
        m_selector->select( this, false );
    }
}

bool
SCOREP_Instrumenter_Mpi::checkCommand( const std::string& current,
                                       const std::string& next )
{
    if ( ( current == "-l" ) &&
         ( is_mpi_library( next ) ) )
    {
        m_selector->select( this, false );
        return true;
    }
    else if ( ( current.substr( 0, 2 ) == "-l" ) &&
              ( is_mpi_library( current.substr( 2 ) ) ) )
    {
        m_selector->select( this, false );
        return false;
    }
    return false;
}

bool
SCOREP_Instrumenter_Mpi::checkWrapperOption( const std::string& current,
                                             const std::string& next )
{
    static bool next_is_also_wrapper = false;

    if ( next_is_also_wrapper )
    {
        next_is_also_wrapper = false;
        return true;
    }
    else if ( current == "-ccl" )
    {
        next_is_also_wrapper = true;
        return true;
    }
    else if ( ( current.substr( 0, 4 ) == "-cc=" ) ||
              ( current.substr( 0, 4 ) == "-CC=" ) ||
              ( current.substr( 0, 5 ) == "-cxx=" ) ||
              ( current.substr( 0, 5 ) == "-f77=" ) ||
              ( current.substr( 0, 5 ) == "-f90=" ) ||
              ( current.substr( 0, 4 ) == "-fc=" ) )
    {
        return true;
    }
    return false;
}

void
SCOREP_Instrumenter_Mpi::checkObjects( SCOREP_Instrumenter& instrumenter )
{
    if ( m_selector->isAlreadySelected( this ) )
    {
        return;
    }

    std::vector<std::string>* object_list = instrumenter.getInputFiles();

    std::stringstream all_objects_stream;
    for ( std::vector<std::string>::iterator current_file = object_list->begin();
          current_file != object_list->end();
          current_file++ )
    {
        if ( is_object_file( *current_file ) || is_library( *current_file ) )
        {
            all_objects_stream << " " << *current_file;
        }
    }

    /* Get all static libs, exclude shared, as we can't wrap them anymore */
    all_objects_stream << instrumenter.getCommandLine().getLibraryFiles( false );

    std::string command = SCOREP_NM " " + all_objects_stream.str() + " 2>/dev/null | "
                          SCOREP_EGREP " -l 'U (MPI|mpi)_' >/dev/null 2>&1";
    if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
    {
        std::cerr << command << std::endl;
    }
    int return_value = system( command.c_str() );
    if ( return_value == 0 )
    {
        m_selector->select( this, false );
    }
}

bool
SCOREP_Instrumenter_Mpi::isInterpositionLibrary( const std::string& libraryName )
{
    return is_mpi_library( libraryName );
}

bool
SCOREP_Instrumenter_Mpi::is_mpi_library( const std::string& libraryName )
{
    return check_lib_name( libraryName, "mpi" ) ||
           check_lib_name( libraryName, "mpich" ) ||
           check_lib_name( libraryName, "mpigf" ) ||
           check_lib_name( libraryName, "mpigi" ) ||
           check_lib_name( libraryName, "mpifort" ) ||
           check_lib_name( libraryName, "mpicxx" );
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Shmem
 * *************************************************************************************/
SCOREP_Instrumenter_Shmem::SCOREP_Instrumenter_Shmem
(
    SCOREP_Instrumenter_Selector* selector
) : SCOREP_Instrumenter_Paradigm( selector, "shmem", "",
                                  "SHMEM support using library wrapping" )
{
#if !HAVE_BACKEND( SHMEM_PROFILING_INTERFACE )
    m_requires.push_back( SCOREP_INSTRUMENTER_ADAPTER_LINKTIME_WRAPPING );
#endif
#if !( HAVE_BACKEND( SHMEM_SUPPORT ) )
    unsupported();
#endif
}

void
SCOREP_Instrumenter_Shmem::checkCompilerName( const std::string& compiler )
{
    /* Adapt for other SHMEM compilers than OpenSHMEM (osh**) */
    if ( compiler.substr( 0, 3 ) == "osh" )
    {
        m_selector->select( this, false );
    }
}

bool
SCOREP_Instrumenter_Shmem::checkCommand( const std::string& current,
                                         const std::string& next )
{
    if ( ( current == "-l" ) &&
         ( is_shmem_library( next ) ) )
    {
        m_selector->select( this, false );
        return true;
    }
    else if ( ( current.substr( 0, 2 ) == "-l" ) &&
              ( is_shmem_library( current.substr( 2 ) ) ) )
    {
        m_selector->select( this, false );
        return false;
    }
    return false;
}

void
SCOREP_Instrumenter_Shmem::checkObjects( SCOREP_Instrumenter& instrumenter )
{
    if ( m_selector->isAlreadySelected( this ) )
    {
        return;
    }

    std::vector<std::string>* object_list = instrumenter.getInputFiles();

    std::stringstream all_objects_stream;
    for ( std::vector<std::string>::iterator current_file = object_list->begin();
          current_file != object_list->end();
          current_file++ )
    {
        bool allow_dynamic = true;
#if !HAVE_BACKEND( SHMEM_PROFILING_INTERFACE )
        // with only link-time wrapping, we must exclude dynamic libraries
        allow_dynamic = false;
#endif
        if ( is_object_file( *current_file )
             || is_library( *current_file, allow_dynamic ) )
        {
            all_objects_stream << " " << *current_file;
        }
    }

    /* Get all static libs, exclude shared, as we can't wrap them anymore */
    all_objects_stream << instrumenter.getCommandLine().getLibraryFiles( false );

    std::string command = SCOREP_NM " " + all_objects_stream.str() + " 2>/dev/null | "
                          SCOREP_EGREP " -l 'U (shmem_|my_pe|_my_pe|num_pes|_num_pes|start_pes|shmalloc|shfree|shmemalign|shrealloc)' >/dev/null 2>&1";
    if ( instrumenter.getCommandLine().getVerbosity() >= 1 )
    {
        std::cerr << command << std::endl;
    }
    int return_value = system( command.c_str() );
    if ( return_value == 0 )
    {
        m_selector->select( this, false );
    }
}

bool
SCOREP_Instrumenter_Shmem::isInterpositionLibrary( const std::string& libraryName )
{
    return is_shmem_library( libraryName );
}

bool
SCOREP_Instrumenter_Shmem::is_shmem_library( const std::string& libraryName )
{
    /*
     * Check for libopenshmem (OpenSHMEM)
     *           liboshmem    (OpenMPI)
     *           libsma       (SGI MPT, Cray SHMEM)
     */

    if ( SCOREP_SHMEM_LIB_NAME != "" )
    {
        return check_lib_name( libraryName, SCOREP_SHMEM_LIB_NAME );
    }

    return false;
}

/* **************************************************************************************
 * class SCOREP_Instrumenter_Mpp
 * *************************************************************************************/
SCOREP_Instrumenter_Mpp::SCOREP_Instrumenter_Mpp()
    : SCOREP_Instrumenter_Selector( "mpp", false )
{
    m_paradigm_list.push_back( new SCOREP_Instrumenter_NoMpp( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_Mpi( this ) );
    m_paradigm_list.push_back( new SCOREP_Instrumenter_Shmem( this ) );
    m_current_selection.push_back( m_paradigm_list.front() );
}
