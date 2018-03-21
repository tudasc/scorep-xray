/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2014, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
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

#include <scorep_tools_utils.hpp>

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
SCOREP_Instrumenter_PreprocessAdapter::preprocess( SCOREP_Instrumenter&         instrumenter,
                                                   SCOREP_Instrumenter_CmdLine& cmdLine,
                                                   const std::string&           source_file )
{
    std::string orig_ext    = get_extension( source_file );
    std::string output_file = source_file;
    std::string command;

    // Remove problematic arguments from command line
#if SCOREP_BACKEND_COMPILER_CRAY
    cmdLine.removeUserArg( "-eZ" );
#endif

    // Prepare file for preprocessing
    if ( !is_fortran_file( source_file ) )
    {
        output_file = remove_extension( remove_path( source_file ) )
                      + create_random_string()
                      + ".input"
                      + orig_ext;

        command = "echo \"#include <stdint.h>\n"
                  "#include <opari2/pomp2_lib.h>\n"
                  "#include <opari2/pomp2_user_lib.h>\n"
                  "___POMP2_INCLUDE___\n"
                  "#line 1 \\\"" + undo_backslashing( source_file ) + "\\\"\" > " + output_file;
        instrumenter.executeCommand( command );

        command = "cat " + source_file + " >> " + output_file;
        instrumenter.executeCommand( command );
        instrumenter.addTempFile( output_file );
    }
    // Some Fortran compiler preprocess only if extension is in upper case
    else if ( orig_ext != scorep_toupper( orig_ext ) )
    {
        output_file = remove_extension( remove_path( source_file ) )
                      + ".input"
                      + scorep_toupper( orig_ext );

        command = "echo \"#line 1 \\\"" + undo_backslashing( source_file ) + "\\\"\" > " + output_file;
        #if SCOREP_BACKEND_COMPILER_CRAY
        // Cray ftn does chokes on '#line number' but accepts
        // '# number'. If the semantics is the same is investigated.
        command = "echo \"# 1 \\\"" + undo_backslashing( source_file ) + "\\\"\" > " + output_file;
        #endif
        #if SCOREP_BACKEND_COMPILER_STUDIO
        // Above approach did not work for studio compiler.
        // Start with an empty output_file as we append below.
        command = "> " + output_file;
        #endif
        instrumenter.executeCommand( command );

        instrumenter.executeCommand( "cat " + source_file + " >> " + output_file );
        instrumenter.addTempFile( output_file );
    }

    // Need to call the preprocessor explicitly
    cmdLine.enableSeparatePreprocessingStep();

    return output_file;
}
