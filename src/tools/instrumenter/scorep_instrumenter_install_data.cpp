/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
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
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_adapter.hpp"
#include "scorep_instrumenter_selector.hpp"
#include "scorep_instrumenter_utils.hpp"
#include <scorep_tools_utils.hpp>
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <UTILS_IO.h>
#include <UTILS_CStr.h>

#include <iostream>
#include <fstream>
#include <stdlib.h>

/* ****************************************************************************
   Main interface
******************************************************************************/

SCOREP_Instrumenter_InstallData::SCOREP_Instrumenter_InstallData( void )
{
    m_scorep_config    = SCOREP_BINDIR "/scorep-config";
    m_c_compiler       = SCOREP_CC;
    m_cxx_compiler     = SCOREP_CXX;
    m_fortran_compiler = SCOREP_FC;
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

void
SCOREP_Instrumenter_InstallData::setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine )
{
    m_scorep_config = cmdLine.getPathToBinary() + "scorep-config --build-check";
}

/* ****************************************************************************
   Compiler dependent implementations
******************************************************************************/

/* *************************************** CRAY */
#if SCOREP_BACKEND_COMPILER_CRAY
bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-shared";
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
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return ( arg == "-E" ) || ( arg == "-eP" );
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    if ( ( current == "-A" ) ||
         ( current == "-b" ) ||
         ( current == "-d" ) ||
         ( current == "-e" ) ||
         ( current == "-h" ) ||
         ( current == "-m" ) ||
         ( current == "-M" ) ||
         ( current == "-N" ) ||
         ( current == "-O" ) ||
         ( current == "-r" ) ||
         ( current == "-R" ) ||
         ( current == "-s" ) ||
         ( current == "-x" ) ||
         ( current == "-X" ) ||
         ( current == "-Y" ) )
    {
        return true;
    }

    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return false;
}

/* *************************************** GNU */
#elif SCOREP_BACKEND_COMPILER_GNU
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
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return arg == "-E";
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    if ( ( current == "-x" ) ||
         ( current == "-include" ) )
    {
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return false;
}

/* *************************************** IBM */
#elif SCOREP_BACKEND_COMPILER_IBM
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
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return ( arg == "-E" ) || ( arg == "-qnoobject" );
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return arg == "-O4" ||
           arg == "-O5" ||
           arg == "-qipa" ||
           arg.substr( 0, 6 ) == "-qipa=";
}

/* *************************************** INTEL */
#elif SCOREP_BACKEND_COMPILER_INTEL
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
    /* We are responsible for the final space charachter. */
    return "VT_LIB_DIR=. VT_ROOT=. VT_ADD_LIBS=\"\" ";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return ( arg.substr( 0, 16 ) == "-offload-option," ) ||
           ( arg.substr( 0, 26 ) == "-offload-attribute-target=" ) ||
           ( arg.substr( 0, 14 ) == "-openmp-report" ) ||
           ( arg.substr( 0, 17 ) == "-opt-block-factor" ) ||
           ( arg.substr( 0, 16 ) == "-opt-jump-tables" ) ||
           ( arg == "-opt-loadpair" ) ||
           ( arg.substr( 0, 19 ) == "-opt-malloc-options" ) ||
           ( arg.substr( 0, 18 ) == "-opt-mem-bandwidth" ) ||
           ( arg == "-opt-mod-versioning" ) ||
           ( arg == "-opt-multi-version-aggressive" ) ||
           ( arg.substr( 0, 13 ) == "-opt-prefetch" ) ||
           ( arg.substr( 0, 23 ) == "-opt-ra-region-strategy" ) ||
           ( arg.substr( 0, 21 ) == "-opt-streaming-stores" ) ||
           ( arg == "-opt-subscript-in-range" ) ||
           ( arg.substr( 0, 11 ) == "-openmp-lib" ) ||
           ( arg == "-openmp-profile" ) ||
           ( arg == "-openmp-stubs" ) ||
           ( arg.substr( 0, 12 ) == "-openmp-link" ) ||
           ( arg.substr( 0, 21 ) == "-openmp-threadprivate" ) ||
           ( arg.substr( 0, 11 ) == "-opt-report" ) ||
           ( arg == "-onetrip" ) ||
           ( arg == "-openmp" );
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return arg == "-E";
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    if ( current == "-include" )
    {
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return false;
}

/* *************************************** PGI */
#elif SCOREP_BACKEND_COMPILER_PGI
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
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return arg == "-E";
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    if ( current == "-tp" )
    {
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return false;
}

/* *************************************** STUDIO */
#elif SCOREP_BACKEND_COMPILER_STUDIO
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
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return ( arg == "-E" ) || ( arg == "-F" );
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    if ( ( current == "-h" ) ||
         ( current == "-xMF" ) )
    {
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return false;
}

#elif SCOREP_BACKEND_COMPILER_FUJITSU
bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-shared";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    return arg == "-Free";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    return arg == "-Fixed";
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
    return "-E > " + output_file;
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return arg == "-E";
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    if ( current == "-x" )
    {
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return false;
}

/* *************************************** CLANG */
/*
 * @note Clang currently (2019) supports no Fortran and only brings it in
 *       optional extensions. One such extension uses GCC functionality.
 */
#elif SCOREP_BACKEND_COMPILER_CLANG
bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return arg == "-shared";
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
    /* No Fortran support yet */
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
    /* No Fortran support yet */
    return false;
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
    /* No Fortran support yet */
    return "";
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return arg == "-E";
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
    if ( current == "-x" )
    {
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    return false;
}

#else
#error "Missing OPARI specific OpenMP compiler handling for your compiler, extension required."
#endif

/* ****************************************************************************
   Private methods
******************************************************************************/

void
SCOREP_Instrumenter_InstallData::set_value( const std::string& key,
                                            const std::string& value )
{
    if ( key == "CC"  && value != "" )
    {
        m_c_compiler = value;
    }
    else if ( key == "SCOREP_CONFIG" && value != "" )
    {
        m_scorep_config = value;
    }
    else
    {
        SCOREP_Instrumenter_Adapter::setAllConfigValue( key, value );
        SCOREP_Instrumenter_Selector::setAllConfigValue( key, value );
    }
}
