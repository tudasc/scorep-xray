/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file scorep_instrumenter_preprocess.cpp
 *
 * Implements the class for preprocessing.
 */

#include <config.h>
#include "scorep_instrumenter_preprocess.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

/* **************************************************************************************
 * class SCOREP_Instrumenter_PreprocessAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_PreprocessAdapter::SCOREP_Instrumenter_PreprocessAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_PREPROCESS, "preprocess" )
{
    m_conflicts.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );
    m_prerequisites.push_back( SCOREP_INSTRUMENTER_ADAPTER_OPARI );
}

std::string
SCOREP_Instrumenter_PreprocessAdapter::precompile( SCOREP_Instrumenter&         instrumenter,
                                                   SCOREP_Instrumenter_CmdLine& cmdLine,
                                                   const std::string&           source_file )
{
    std::string orig_ext   = get_extension( source_file );
    std::string input_file = source_file;
    std::string command;

    // Determine language
    std::string language = "c";
    if ( is_fortran_file( source_file ) )
    {
        language = "f";
    }
    else if ( is_cpp_file( source_file ) )
    {
        language = "cxx";
    }

    // Prepare file for preprocessing
    if ( !is_fortran_file( source_file ) )
    {
        input_file = remove_extension( remove_path( source_file ) )
                     + ".input"
                     + orig_ext;

        command = "echo \"#include <stdint.h>\n"
                  "#include <opari2/pomp2_lib.h>\n"
                  "#include <opari2/pomp2_user_lib.h>\n"
                  "___POMP2_INCLUDE___\n"
                  "#line 1 \\\"" + undo_backslashing( source_file ) + "\\\"\" > " + input_file;
        instrumenter.executeCommand( command );

        command = "cat " + source_file + " >> " + input_file;
        instrumenter.executeCommand( command );
        instrumenter.addTempFile( input_file );
    }
    // Some Fortran compiler preprocess only if extension is in upper case
    else if ( orig_ext != scorep_toupper( orig_ext ) )
    {
        input_file = remove_extension( remove_path( source_file ) )
                     + ".input"
                     + scorep_toupper( orig_ext );

        command = "echo \"#line 1 \\\"" + undo_backslashing( source_file ) + "\\\"\" > " + input_file;
        #if SCOREP_BACKEND_COMPILER_CRAY
        // Cray ftn does chokes on '#line number' but accepts
        // '# number'. If the semantics is the same is investigated.
        command = "echo \"# 1 \\\"" + undo_backslashing( source_file ) + "\\\"\" > " + input_file;
        #endif
        #if SCOREP_BACKEND_COMPILER_STUDIO
        // Above approach did not work for studio compiler.
        // Start with an empty input_file as we append below.
        command = "> " + input_file;
        #endif
        instrumenter.executeCommand( command );

        instrumenter.executeCommand( "cat " + source_file + " >> " + input_file );
        instrumenter.addTempFile( input_file );
    }

    // Preprocess file
    command = SCOREP_Instrumenter_InstallData::getCompilerEnvironmentVars()
              + cmdLine.getCompilerName()
              + " " + cmdLine.getFlagsBeforeInterpositionLib()
              + " `" + instrumenter.getConfigBaseCall() + " --" + language + "flags`"
              + " " + instrumenter.getCompilerFlags()
              + " " + cmdLine.getFlagsAfterInterpositionLib()
              + " " + input_file;

    std::string output_file = remove_extension( remove_path( source_file ) )
                              + ".prep"
                              + orig_ext;

    if ( is_c_file( source_file ) )
    {
        command += " " + SCOREP_Instrumenter_InstallData::getCPreprocessingFlags( input_file,
                                                                                  output_file );
    }
    else if ( is_cpp_file( source_file ) )
    {
        command += " " +  SCOREP_Instrumenter_InstallData::getCxxPreprocessingFlags( input_file,
                                                                                     output_file );
    }

    else if ( is_fortran_file( source_file ) )
    {
        command += " " +  SCOREP_Instrumenter_InstallData::getFortranPreprocessingFlags( input_file,
                                                                                         output_file );
    }

    instrumenter.executeCommand( command );
    instrumenter.addTempFile( output_file );

    return output_file;
}
