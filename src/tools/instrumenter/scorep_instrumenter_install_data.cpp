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
 * Copyright (c) 2009-2013, 2020-2022,
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


bool
SCOREP_Instrumenter_InstallData::isPreprocessFlag( const std::string& arg )
{
    return ( arg == "-E" ) || ( arg == "-eP" ) || ( arg == "-qnoobject" );
}

std::string
SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars( void )
{
#if SCOREP_BACKEND_COMPILER_CC_INTEL || \
    SCOREP_BACKEND_COMPILER_CXX_INTEL || \
    SCOREP_BACKEND_COMPILER_FC_INTEL
    /* We are responsible for the final space character. */
    return "VT_LIB_DIR=. VT_ROOT=. VT_ADD_LIBS=\"\" ";
#endif
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgForShared( const std::string& arg )
{
    return ( arg == "-shared" ) || ( arg == "-dynamiclib" ) || ( arg == "-qmkshrobj" );
}

bool
SCOREP_Instrumenter_InstallData::isArgForFreeform( const std::string& arg )
{
#if SCOREP_BACKEND_HAVE_FC_COMPILER
#if SCOREP_BACKEND_COMPILER_FC_CRAY
    return arg == "-ffree";
#elif SCOREP_BACKEND_COMPILER_FC_GNU
    return arg == "-ffree-form";
#elif SCOREP_BACKEND_COMPILER_FC_IBM
    return arg == "-qfree";
#elif SCOREP_BACKEND_COMPILER_FC_INTEL || SCOREP_BACKEND_COMPILER_FC_INTEL_ONEAPI
    return arg == "-free";
#elif SCOREP_BACKEND_COMPILER_FC_PGI
    return arg == "-Mfree" || arg == "-Mfreeform";
#elif SCOREP_BACKEND_COMPILER_FC_FUJITSU
    return arg == "-Free";
#elif SCOREP_BACKEND_COMPILER_FC_CLANG
    return arg == "-Mfree" || arg == "-Mfreeform";
#else
#error Compiler not handled
#endif
#endif /* SCOREP_BACKEND_HAVE_FC_COMPILER */
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isArgForFixedform( const std::string& arg )
{
#if SCOREP_BACKEND_HAVE_FC_COMPILER
#if SCOREP_BACKEND_COMPILER_FC_CRAY
    return arg == "-ffixed";
#elif SCOREP_BACKEND_COMPILER_FC_GNU
    return arg == "-ffixed-form";
#elif SCOREP_BACKEND_COMPILER_FC_IBM
    return arg == "-qfixed";
#elif SCOREP_BACKEND_COMPILER_FC_INTEL || SCOREP_BACKEND_COMPILER_FC_INTEL_ONEAPI
    return arg == "-nofree";
#elif SCOREP_BACKEND_COMPILER_FC_PGI
    return arg == "-Mfixed" || arg == "-Mnofree" || arg == "-Mnofreeform";
#elif SCOREP_BACKEND_COMPILER_FC_FUJITSU
    return arg == "-Fixed";
#elif SCOREP_BACKEND_COMPILER_FC_CLANG
    return arg == "-Mfixed" || arg == "-Mnofree" || arg == "-Mnofreeform";
#else
#error Compiler not handled
#endif
#endif /* SCOREP_BACKEND_HAVE_FC_COMPILER */
    return false;
}

std::string
SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( const std::string& input_file,
                                                         const std::string& output_file )
{
#if SCOREP_BACKEND_COMPILER_CC_GNU || \
    SCOREP_BACKEND_COMPILER_CC_INTEL || \
    SCOREP_BACKEND_COMPILER_CC_INTEL_ONEAPI || \
    SCOREP_BACKEND_COMPILER_CC_CLANG
    return "-E -o " + output_file;
#elif SCOREP_BACKEND_COMPILER_CC_CRAY || \
    SCOREP_BACKEND_COMPILER_CC_IBM || \
    SCOREP_BACKEND_COMPILER_CC_PGI || \
    SCOREP_BACKEND_COMPILER_CC_FUJITSU
    return "-E > " + output_file;
#else
#error Compiler not handled
#endif
}

std::string
SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( const std::string& input_file,
                                                           const std::string& output_file )
{
#if SCOREP_BACKEND_COMPILER_CXX_GNU || \
    SCOREP_BACKEND_COMPILER_CXX_INTEL || \
    SCOREP_BACKEND_COMPILER_CXX_INTEL_ONEAPI || \
    SCOREP_BACKEND_COMPILER_CXX_PGI || \
    SCOREP_BACKEND_COMPILER_CXX_CLANG
    return "-E -o " + output_file;
#elif SCOREP_BACKEND_COMPILER_CXX_CRAY || \
    SCOREP_BACKEND_COMPILER_CXX_IBM || \
    SCOREP_BACKEND_COMPILER_CXX_FUJITSU
    return "-E > " + output_file;
#else
#error Compiler not handled
#endif
}

std::string
SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( const std::string& input_file,
                                                               const std::string& output_file )
{
#if SCOREP_BACKEND_HAVE_FC_COMPILER
#if SCOREP_BACKEND_COMPILER_FC_CRAY
    return "-eP && mv "
           + remove_extension( remove_path( input_file ) ) + ".i "
           + output_file;
#elif SCOREP_BACKEND_COMPILER_FC_GNU
    return "-cpp -E -o " + output_file;
#elif SCOREP_BACKEND_COMPILER_FC_IBM
    std::string basename      = remove_extension( remove_path( input_file ) );
    std::string prep_file_v13 = "F" + basename + ".f";
    std::string prep_file_v14 = "F" + basename + scorep_tolower( get_extension( input_file ) );
    return "-d -qnoobject && if [ -e " + prep_file_v14 + " ]; then mv " + prep_file_v14 + " " + output_file + "; else mv " + prep_file_v13 + " " + output_file + "; fi";
#elif SCOREP_BACKEND_COMPILER_FC_INTEL || \
    SCOREP_BACKEND_COMPILER_FC_INTEL_ONEAPI || \
    SCOREP_BACKEND_COMPILER_FC_PGI || \
    SCOREP_BACKEND_COMPILER_FC_FUJITSU || \
    SCOREP_BACKEND_COMPILER_FC_CLANG
    return "-E > " + output_file;
#else
#error Compiler not handled
#endif
#endif /* SCOREP_BACKEND_HAVE_FC_COMPILER */
    return "";
}

bool
SCOREP_Instrumenter_InstallData::isArgWithO( const std::string& arg )
{
#if SCOREP_BACKEND_COMPILER_CC_INTEL || SCOREP_BACKEND_COMPILER_CXX_INTEL || SCOREP_BACKEND_COMPILER_FC_INTEL
    // Following options have been seen with the Intel compilers only. Using
    // with other compilers is probably safe.
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
#endif
    return false;
}

bool
SCOREP_Instrumenter_InstallData::isCompositeArg( const std::string& current,
                                                 const std::string& next )
{
#if SCOREP_BACKEND_COMPILER_CC_CRAY && SCOREP_BACKEND_COMPILER_CXX_CRAY && SCOREP_BACKEND_COMPILER_FC_CRAY
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
#elif SCOREP_BACKEND_COMPILER_CC_GNU && SCOREP_BACKEND_COMPILER_CXX_GNU && SCOREP_BACKEND_COMPILER_FC_GNU
    if ( ( current == "-x" ) ||
         ( current == "-include" ) )
    {
        return true;
    }
    return false;
#elif SCOREP_BACKEND_COMPILER_CC_IBM && SCOREP_BACKEND_COMPILER_CXX_IBM && SCOREP_BACKEND_COMPILER_FC_IBM
    return false;
#elif SCOREP_BACKEND_COMPILER_CC_INTEL && SCOREP_BACKEND_COMPILER_CXX_INTEL && SCOREP_BACKEND_COMPILER_FC_INTEL
    if ( current == "-include" )
    {
        return true;
    }
    return false;
#elif SCOREP_BACKEND_COMPILER_CC_PGI && SCOREP_BACKEND_COMPILER_CXX_PGI && SCOREP_BACKEND_COMPILER_FC_PGI
    if ( current == "-tp" )
    {
        return true;
    }
    return false;
#elif SCOREP_BACKEND_COMPILER_CC_FUJITSU && SCOREP_BACKEND_COMPILER_CXX_FUJITSU && SCOREP_BACKEND_COMPILER_FC_FUJITSU
    if ( current == "-x" )
    {
        return true;
    }
    return false;
#elif SCOREP_BACKEND_COMPILER_CC_CLANG && SCOREP_BACKEND_COMPILER_CXX_CLANG
    if ( current == "-x" )
    {
        return true;
    }
    return false;
#else
    // Non-unique vendor installation: -x || -include might cover lots of cases.
    if ( ( current == "-x" ) ||
         ( current == "-include" ) )
    {
        return true;
    }
    return false;
#endif
}

bool
SCOREP_Instrumenter_InstallData::conflictsWithLinktimeWrapping( const std::string& arg )
{
    // Assumption: it is likely that if the IBM compiler is used, then it is
    // used for all languages.
#if SCOREP_BACKEND_COMPILER_CC_IBM && SCOREP_BACKEND_COMPILER_CXX_IBM && SCOREP_BACKEND_COMPILER_FC_IBM
    return arg == "-O4" ||
           arg == "-O5" ||
           arg == "-qipa" ||
           arg.substr( 0, 6 ) == "-qipa=";
#endif
    return false;
}
