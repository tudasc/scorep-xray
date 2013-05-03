/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
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
simplify_path( const std::string& path )
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

SCOREP_Instrumenter_InstallData::SCOREP_Instrumenter_InstallData( void )
{
    m_scorep_config    = SCOREP_PREFIX "/bin/scorep-config";
    m_c_compiler       = SCOREP_CC;
    m_cxx_compiler     = SCOREP_CXX;
    m_fortran_compiler = SCOREP_FC;
    m_openmp_cflags    = SCOREP_OPENMP_CFLAGS;
    m_nm               = "`" OPARI_CONFIG " --nm`";
    m_opari            = OPARI;
    m_opari_script     = "`" OPARI_CONFIG " --region-initialization`";
    m_opari_config     = OPARI_CONFIG;
    m_cobi             = SCOREP_COBI_PATH;
    m_cobi_config_dir  = COBI_CONFIG_DIR;
    m_pdt_bin_path     = PDT;
    m_pdt_config_file  = PDT_CONFIG;
    m_opari_params     = "";
    m_cobi_params      = "";
}

SCOREP_Instrumenter_InstallData::~SCOREP_Instrumenter_InstallData()
{
}

std::string
SCOREP_Instrumenter_InstallData::getScorepConfig( void )
{
    return m_scorep_config;
}

std::string
SCOREP_Instrumenter_InstallData::getCC( void )
{
    return m_c_compiler;
}

std::string
SCOREP_Instrumenter_InstallData::getCXX( void )
{
    return m_cxx_compiler;
}

std::string
SCOREP_Instrumenter_InstallData::getFC( void )
{
    return m_fortran_compiler;
}

std::string
SCOREP_Instrumenter_InstallData::getNm( void )
{
    return m_nm;
}

std::string
SCOREP_Instrumenter_InstallData::getOpari( void )
{
    return m_opari + " " + m_opari_params;
}

std::string
SCOREP_Instrumenter_InstallData::getOpariScript( void )
{
    return m_opari_script;
}

std::string
SCOREP_Instrumenter_InstallData::getOpariConfig( void )
{
    return m_opari_config;
}

std::string
SCOREP_Instrumenter_InstallData::getCobi( void )
{
    return m_cobi + " " + m_cobi_params;
}

std::string
SCOREP_Instrumenter_InstallData::getCobiConfigDir( void )
{
    return m_cobi_config_dir;
}

std::string
SCOREP_Instrumenter_InstallData::getPdtBinPath( void )
{
    return m_pdt_bin_path;
}

std::string
SCOREP_Instrumenter_InstallData::getPdtConfigFile( void )
{
    return m_pdt_config_file;
}

SCOREP_ErrorCode
SCOREP_Instrumenter_InstallData::readConfigFile( const std::string& arg0 )
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
SCOREP_Instrumenter_InstallData::setBuildCheck( void )
{
    m_scorep_config = simplify_path( BUILD_DIR "/scorep-config" )
                      + " --build-check";
    #if !HAVE( EXTERNAL_OPARI2 )
    m_opari_config = simplify_path( BUILD_DIR "/../vendor/opari2/build-frontend/opari2-config" )
                     + " --build-check";
    m_opari = simplify_path( BUILD_DIR "/../vendor/opari2/build-frontend/opari2" );
    #endif
    m_nm              = "`" + m_opari_config +  " --nm`";
    m_opari_script    = "`" + m_opari_config + " --region-initialization`";
    m_cobi_config_dir = simplify_path( BUILD_DIR "/../share" );
    m_pdt_config_file = simplify_path( BUILD_PDT_CONFIG );
}

void
SCOREP_Instrumenter_InstallData::setOpariParams( const std::string& params )
{
    m_opari_params += " " + params;
}

void
SCOREP_Instrumenter_InstallData::setCobiParams( const std::string& params )
{
    m_cobi_params = params;
}

void
SCOREP_Instrumenter_InstallData::setOpariFortranForm( bool is_free )
{
#if HAVE( OPARI2_FIX_AND_FREE_FORM_OPTIONS )
    if ( is_free )
    {
        setOpariParams( "--free-form" );
    }
    else
    {
        setOpariParams( "--fix-form" );
    }
#else
    if ( is_free )
    {
        setOpariParams( "--f90" );
    }
    else
    {
        setOpariParams( "--f77" );
    }
#endif
}


/* ****************************************************************************
   Compiler dependent implementations
******************************************************************************/

/* *************************************** CRAY */
#if SCOREP_BACKEND_COMPILER_CRAY
bool
SCOREP_Instrumenter_InstallData::isArgForOpenmp( const std::string& arg )
{
    return arg == m_openmp_cflags;
}

bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-dynamic";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    return arg == "-ffree";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    return arg == "-ffixed";
}

std::string
SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( const std::string& input_file,
                                                         const std::string& output_file )
{
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( const std::string& input_file,
                                                           const std::string& output_file )
{
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( const std::string& input_file,
                                                               const std::string& output_file )
{
    return "-eP && mv "
           + remove_extension( remove_path( input_file ) ) + ".i "
           + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( std::string arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( std::string arg )
{
    return ( arg == "-E" ) || ( arg == "-eP" );
}

/* *************************************** GNU */
#elif SCOREP_BACKEND_COMPILER_GNU
bool
SCOREP_Instrumenter_InstallData::isArgForOpenmp( const std::string& arg )
{
    return arg == m_openmp_cflags;
}

bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-shared";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    return arg == "-ffree-form";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    return arg == "-ffixed-form";
}

std::string
SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( const std::string& input_file,
                                                         const std::string& output_file )
{
    return "-E -o " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( const std::string& input_file,
                                                           const std::string& output_file )
{
    return "-E -o " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( const std::string& input_file,
                                                               const std::string& output_file )
{
    return "-cpp -E -o " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( std::string arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( std::string arg )
{
    return arg == "-E";
}

/* *************************************** IBM */
#elif SCOREP_BACKEND_COMPILER_IBM
bool
SCOREP_Instrumenter_InstallData::isArgForOpenmp( const std::string& arg )
{
    return arg == m_openmp_cflags;
}

bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-qmkshrobj";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    return arg == "-qfree";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    return arg == "-qfixed";
}

std::string
SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( const std::string& input_file,
                                                         const std::string& output_file )
{
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( const std::string& input_file,
                                                           const std::string& output_file )
{
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( const std::string& input_file,
                                                               const std::string& output_file )
{
    std::string basename      = remove_extension( remove_path( input_file ) );
    std::string prep_file_v13 = "F" + basename + ".f";
    std::string prep_file_v14 = "F" + basename + scorep_tolower( get_extension( input_file ) );

    return "-d -qnoobject && if [ -e " + prep_file_v14 + " ]; then mv " + prep_file_v14 + " " + output_file + "; else mv " + prep_file_v13 + " " + output_file + "; fi";
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( std::string arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( std::string arg )
{
    return ( arg == "-E" ) || ( arg == "-qnoobject" );
}

/* *************************************** INTEL */
#elif SCOREP_BACKEND_COMPILER_INTEL
bool
SCOREP_Instrumenter_InstallData::isArgForOpenmp( const std::string& arg )
{
    return ( arg == m_openmp_cflags ) || ( arg == "-openmp" );
}

bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return ( arg == "-shared" ) || ( arg == "-dynamiclib" );
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    return arg == "-free";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    return arg == "-nofree";
}

std::string
SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( const std::string& input_file,
                                                         const std::string& output_file )
{
    return "-E -o " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( const std::string& input_file,
                                                           const std::string& output_file )
{
    return "-E -o " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( const std::string& input_file,
                                                               const std::string& output_file )
{
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "VT_LIB_DIR=. VT_ROOT=. VT_ADD_LIBS=\"\"";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( std::string arg )
{
    return ( arg.substr( 0, 16 ) == "-offload-option," ) ||
           ( arg.substr( 0, 26 ) == "-offload-attribute-target=" );
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( std::string arg )
{
    return arg == "-E";
}

/* *************************************** PGI */
#elif SCOREP_BACKEND_COMPILER_PGI
bool
SCOREP_Instrumenter_InstallData::isArgForOpenmp( const std::string& arg )
{
    return arg == m_openmp_cflags;
}

bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-shared";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    return arg == "-Mfree" || arg == "-Mfreeform";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    return arg == "-Mnofree" || arg == "-Mnofreeform";
}

std::string
SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( const std::string& input_file,
                                                         const std::string& output_file )
{
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( const std::string& input_file,
                                                           const std::string& output_file )
{
    return "-E -o " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( const std::string& input_file,
                                                               const std::string& output_file )
{
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( std::string arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( std::string arg )
{
    return arg == "-E";
}

/* *************************************** STUDIO */
#elif SCOREP_BACKEND_COMPILER_STUDIO
bool
SCOREP_Instrumenter_InstallData::isArgForOpenmp( const std::string& arg )
{
    return arg == m_openmp_cflags;
}

bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-G";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    return arg == "-free";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    return arg == "-fixed";
}

std::string
SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( const std::string& input_file,
                                                         const std::string& output_file )
{
    return "-E -o " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( const std::string& input_file,
                                                           const std::string& output_file )
{
    /* The sed statements remove every line directive if the following line
       starts with a closing bracket */
    if ( get_extension( output_file ) == ".i" )
    {
        return "-E | sed 'N;s/\\n)/)/;P;D;' | sed 's/#[0-9]*)/)/g' > "
               + output_file;
    }
    else
    {
        return "-E | sed 'N;s/\\n)/)/;P;D;' | sed 's/#[0-9]*)/)/g' > "
               + remove_extension( remove_path( output_file ) ) + ".i && mv "
               + remove_extension( remove_path( output_file ) ) + ".i "
               + output_file;
    }
}

std::string
SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( const std::string& input_file,
                                                               const std::string& output_file )
{
    std::string basename = remove_extension( remove_path( output_file ) );

    if ( get_extension( output_file ) == ".f90" )
    {
        return "-fpp -F -o " + basename
               + ".i && grep -v \\# "
               + basename + ".i > "
               + output_file;
    }
    else
    {
        return "-fpp -F -o " + basename
               + ".f90 && grep -v \\# "
               + basename + ".f90 > "
               + output_file
               + " && rm -f "
               + basename + ".f90";
    }
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( std::string arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( std::string arg )
{
    return ( arg == "-E" ) || ( arg == "-F" );
}

#endif

/* ****************************************************************************
   Private methods
******************************************************************************/

void
SCOREP_Instrumenter_InstallData::set_value( const std::string& key,
                                            const std::string& value )
{
    if ( key == "OPARI_CONFIG" && value != "" )
    {
        m_nm           = "`" + value + " --nm`";
        m_opari_script = "`" + value + " --region-initialization`";
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
SCOREP_Instrumenter_InstallData::set_pdt_path( const std::string& pdt )
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
            exit( EXIT_FAILURE );
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
