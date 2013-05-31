/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file scorep_instrumenter_compiler.cpp
 * @status alpha
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * Implements the class for compiler instrumentation.
 */

#include <config.h>
#include "scorep_instrumenter_compiler.hpp"
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
 * class SCOREP_Instrumenter_CompilerAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_CompilerAdapter::SCOREP_Instrumenter_CompilerAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_COMPILER, "compiler" )
{
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_COBI );
    m_default_off.push_back( SCOREP_INSTRUMENTER_ADAPTER_PDT );
}

std::string
SCOREP_Instrumenter_CompilerAdapter::getConfigToolFlag( void )
{
    if ( !isEnabled() )
    {
        return " --no" + m_name;
    }
    return "";
}

std::string
SCOREP_Instrumenter_CompilerAdapter::precompile
(
    SCOREP_Instrumenter&         instrumenter,
    SCOREP_Instrumenter_CmdLine& cmdLine,
    const std::string&           input_file
)
{
    /* The sun compiler can only instrument Fortran files. Thus, any C/C++
       files are not instrumented. To avoid user confusion, the instrumenter
       aborts in case a C/C++ file should be compiler instrumented.
     */
#if SCOREP_BACKEND_COMPILER_STUDIO
    static bool checked = false;
    if ( !checked && !isEnabled() )
    {
        checked = true;
    }
    if ( !checked )
    {
        std::string current_file = "";
        std::string extension    = "";
        std::string input_files  = cmdLine.getInputFiles();
        size_t      old_pos      = 0;
        size_t      cur_pos      = 0;
        while ( cur_pos != std::string::npos )
        {
            cur_pos = input_files.find( " ", old_pos );
            if ( old_pos < cur_pos ) // Discard a blank
            {
                current_file = input_files.substr( old_pos, cur_pos - old_pos );
                if ( !is_fortran_file( current_file ) )
                {
                    std::cerr << "Compiler instrumentation with the Sun compiler is "
                              << "only possible for Fortran files. If you want to "
                              << "switch off compiler instrumentation, please use the "
                              << "--nocompiler option."
                              << std::endl;
                    exit( EXIT_FAILURE );
                }
            }
            // Setup for next file
            old_pos = cur_pos + 1;
        }
        checked = true;
    }
#endif // SCOREP_BACKEND_COMPILER_STUDIO

    return input_file;
}
