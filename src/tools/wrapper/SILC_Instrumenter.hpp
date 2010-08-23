/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SILC_Instrumenter.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Class to steer the instrumentation of the user code.
 */

#ifndef SILC_INSTRUMENTER_H_
#define SILC_INSTRUMENTER_H_

#include <iostream>
#include <string>


#include "SILC_Application.hpp"

/* ****************************************************************************
   Class SILC_Instrumenter
******************************************************************************/
/**
 *  @brief performes instrumentation stage
 *
 *  This class examines the available compiler settings and the type of
 *  instrumentation
 */
class SILC_Instrumenter : public SILC_Application
{
    /* ********************************************************* Private Types */
private:
    typedef enum
    {
        enabled,
        detect,
        disabled
    } instrumentation_usage_t;

    typedef enum
    {
        silc_parse_mode_param,
        silc_parse_mode_command,
        silc_parse_mode_output,
        silc_parse_mode_config
    } silc_parse_mode_t;

    /* ******************************************************** Public methods */
public:

    /**
       Creates a new SILC_Instrumenter object.
     */
    SILC_Instrumenter();

    /**
       Destroys a SILC_Instrumenter object
     */
    virtual ~
    SILC_Instrumenter();

    /**
       Performs the instrumentation of an application
     */
    virtual int
    Run();

    virtual SILC_Error_Code
    ParseCmdLine( int    argc,
                  char** argv );

    virtual void
    PrintParameter();

    /* ******************************************************** Private methods */
private:

/**
 * @brief   extracts parameter from input file
 * It expects lines of the format key=value. Furthermore it truncates line
 * at the scrpit comment character '#'.
 *
 * @param line    input line from the config file
 */
    SILC_Error_Code
    read_parameter( std::string line );

    int
    execute_command();
    void
    check_parameter();
    silc_parse_mode_t
    parse_output( std::string arg );
    silc_parse_mode_t
    parse_command( std::string arg );
    silc_parse_mode_t
    parse_parameter( std::string arg );
    silc_parse_mode_t
    parse_config( std::string arg );
    void
    prepare_compiler();
    void
    prepare_user();
    void
    prepare_opari();

    SILC_Error_Code
    open_config_file( std::ifstream* inFile );
    SILC_Error_Code
    read_config_file();

    /* ******************************************************* Private members */
private:
    /* --------------------------------------------
       Flags for used adapters
       ------------------------------------------*/
    /**
       Specifies if compiler instrumentation is enabled. Default is enabled
     */
    instrumentation_usage_t compiler_instrumentation;

    /**
       Specifies if OPARI instrumentation is enabled. Default detect.
     */
    instrumentation_usage_t opari_instrumentation;

    /**
       Specifies if user instrumentation is enabled. Default is disabled.
     */
    instrumentation_usage_t user_instrumentation;

    /**
       Specifies if mpi wrappers are enabled. Default detect.
     */
    instrumentation_usage_t mpi_instrumentation;

    /* --------------------------------------------
       Flags for application type
       ------------------------------------------*/
    /**
       Specifies whether it is a MPI application.
     */
    instrumentation_usage_t is_mpi_application;

    /**
       Specifies whether it is an OpenMP application.
     */
    instrumentation_usage_t is_openmp_application;

    /* --------------------------------------------
       Work mode information
       ------------------------------------------*/
    /**
       True if compiling
     */
    bool is_compiling;

    /**
       True if linking
     */
    bool is_linking;

    /* --------------------------------------------
       Input command elements
       ------------------------------------------*/
    /**
        compiler/linker name
     */
    std::string compiler_name;

    /**
       compiler/linker flags
     */
    std::string compiler_flags;

    /**
       file name of the compiler/linker output
     */
    std::string output_name;

    /**
       input file names. Need to be separated because OPARI may
       perform source code modifications which take these as input and
       the original command needs the result from the OPRI output. Thus,
       they are then substituted by the OPRI output.
     */
    std::string input_files;

    /* --------------------------------------------
       Config file data
       ------------------------------------------*/
    /**
       Config file name. Can be set with the -config parameter.
       If it is empty, a config file is searched at standard locations.
     */
    std::string config_file;

    /**
       Stores compiler instruemntation flags
     */
    std::string compiler_instrumentation_flags;

    /**
       Stores include path of SILC header files
     */
    std::string silc_include_path;

    /**
       Stores library path of SILC libraries
     */
    std::string silc_library_path;
};

#endif /*SILC_INSTRUMENTER_H_*/
