/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @status     alpha
 * @file       scorep_instrumenter_install_config.cpp
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 */

#include <config.h>
#include <scorep_instrumenter_install_data.hpp>
#include <scorep_instrumenter_utils.hpp>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <UTILS_IO.h>
#include <UTILS_CStr.h>

#include <iostream>
#include <fstream>
#include <stdlib.h>


/* ****************************************************************************
   Helper functions
******************************************************************************/
static inline std::string
simplify_path( std::string path )
{
    char* buffer = UTILS_CStr_dup( path.c_str() );
    UTILS_IO_SimplifyPath( buffer );
    std::string simple_path = buffer;
    free( buffer );
    return simple_path;
}

/* ****************************************************************************
   Main interface
******************************************************************************/

SCOREP_Instrumenter_InstallData::SCOREP_Instrumenter_InstallData()
{
    m_scorep_config   = SCOREP_PREFIX "/bin/scorep-config";
    m_c_compiler      = SCOREP_CC;
    m_openmp_cflags   = SCOREP_OPENMP_CFLAGS;
    m_nm              = "`" OPARI_CONFIG " --nm`";
    m_awk             =  "`" OPARI_CONFIG " --awk-cmd`";
    m_opari           = OPARI;
    m_opari_script    = "`" OPARI_CONFIG " --awk-script`";
    m_opari_config    = OPARI_CONFIG;
    m_grep            =  "`" OPARI_CONFIG " --egrep`";
    m_cobi            = SCOREP_COBI_PATH;
    m_cobi_config_dir = COBI_CONFIG_DIR;
    m_pdt_bin_path    = PDT;
    m_pdt_config_file = PDT_CONFIG;
}

SCOREP_Instrumenter_InstallData::~SCOREP_Instrumenter_InstallData()
{
}

std::string
SCOREP_Instrumenter_InstallData::getScorepConfig()
{
    return m_scorep_config;
}

std::string
SCOREP_Instrumenter_InstallData::getCC()
{
    return m_c_compiler;
}

std::string
SCOREP_Instrumenter_InstallData::getOpenmpFlags()
{
    return m_openmp_cflags;
}

std::string
SCOREP_Instrumenter_InstallData::getNm()
{
    return m_nm;
}

std::string
SCOREP_Instrumenter_InstallData::getAwk()
{
    return m_awk;
}

std::string
SCOREP_Instrumenter_InstallData::getOpari()
{
    return m_opari;
}

std::string
SCOREP_Instrumenter_InstallData::getOpariScript()
{
    return m_opari_script;
}

std::string
SCOREP_Instrumenter_InstallData::getOpariConfig()
{
    return m_opari_config;
}

std::string
SCOREP_Instrumenter_InstallData::getGrep()
{
    return m_grep;
}

std::string
SCOREP_Instrumenter_InstallData::getCobi()
{
    return m_cobi;
}

std::string
SCOREP_Instrumenter_InstallData::getCobiConfigDir()
{
    return m_cobi_config_dir;
}

std::string
SCOREP_Instrumenter_InstallData::getPdtBinPath()
{
    return m_pdt_bin_path;
}

std::string
SCOREP_Instrumenter_InstallData::getPdtConfigFile()
{
    return m_pdt_config_file;
}

SCOREP_ErrorCode
SCOREP_Instrumenter_InstallData::readConfigFile( std::string arg0 )
{
    std::ifstream in_file;
    in_file.open( arg0.c_str() );

    if ( in_file.good() )
    {
        while ( in_file.good() )
        {
            char line[ 512 ] = { "" };
            in_file.getline( line, 512 );
            read_parameter( line );
        }
        return SCOREP_SUCCESS;
    }
    else
    {
        return SCOREP_ERROR_FILE_CAN_NOT_OPEN;
    }
}

void
SCOREP_Instrumenter_InstallData::setBuildCheck()
{
    m_scorep_config = simplify_path( BUILD_DIR "/scorep-config" )
                      + " --build-check";
    #if !HAVE( EXTERNAL_OPARI2 )
    m_opari_config = simplify_path( BUILD_DIR "/../vendor/opari2/build-frontend/opari2-config" )
                     + " --build-check";
    m_opari = simplify_path( BUILD_DIR "/../vendor/opari2/build-frontend/opari2" );
    #endif
    m_nm              = "`" + m_opari_config +  " --nm`";
    m_awk             = "`" + m_opari_config +  " --awk-cmd`";
    m_opari_script    = "`" + m_opari_config + " --awk-script`";
    m_grep            = "`" + m_opari_config + " --egrep`";
    m_cobi_config_dir = simplify_path( BUILD_DIR "/../share" );
    m_pdt_config_file = simplify_path( BUILD_PDT_CONFIG );
}

/* ****************************************************************************
   Private methods
******************************************************************************/

void
SCOREP_Instrumenter_InstallData::set_value( std::string key,
                                            std::string value )
{
    if ( key == "EGREP" && value != "" )
    {
        m_grep = value;
    }

    else if ( key == "OPARI_CONFIG" && value != "" )
    {
        m_nm           = "`" + value + " --nm`";
        m_awk          = "`" + value + " --awk-cmd`";
        m_opari_script = "`" + value + " --awk-script`";
        m_grep         = "`" + value + " --egrep`";
        m_opari_config = value;
    }
    else if ( key == "PDT" && value != "" )
    {
        set_pdt_path( value );
    }
    else if ( key == "OPENMP_CFLAGS" && value != "" )
    {
        m_openmp_cflags = value;
    }
    else if ( key == "OPARI" && value != "" )
    {
        m_opari = value;
    }
    else if ( key == "CC"  && value != "" )
    {
        m_c_compiler = value;
    }
    else if ( key == "PDT_CONFIG" && value != "" )
    {
        m_pdt_config_file = value;
    }
    else if ( key == "SCOREP_CONFIG" && value != "" )
    {
        m_scorep_config = value;
    }
    else if ( key == "COBI_CONFIG_DIR" && value != "" )
    {
        m_cobi_config_dir = value;
    }
}

void
SCOREP_Instrumenter_InstallData::set_pdt_path( std::string pdt )
{
    if ( pdt == "yes" )
    {
        char* path = UTILS_GetExecutablePath( "tau_instrumentor" );
        if ( path != NULL )
        {
            m_pdt_bin_path = path;
            free( path );
        }
        else
        {
            std::cout << "ERROR: Unable to find PDT binaries.\n";
            abort();
        }
    }
    else if ( pdt == "no" )
    {
        return;
    }
    else
    {
        m_pdt_bin_path = pdt;
    }
}

SCOREP_ErrorCode
SCOREP_Instrumenter_InstallData::read_parameter( std::string line )
{
    /* check for comments */
    int pos = line.find( "#" );
    if ( pos == 0 )
    {
        return SCOREP_SUCCESS;                      // Whole line cemmented out
    }
    if ( pos != std::string::npos )
    {
        // Truncate line at comment
        line = line.substr( pos, line.length() - pos - 1 );
    }

    /* separate value and key */
    pos = line.find( "=" );
    if ( pos == std::string::npos )
    {
        return SCOREP_ERROR_PARSE_NO_SEPARATOR;
    }
    std::string key   = line.substr( 0, pos );
    std::string value = line.substr( pos + 2, line.length() - pos - 3 );

    set_value( key, value );

    return SCOREP_SUCCESS;
}
