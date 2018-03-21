/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_INSTRUMENTER_INSTALL_CONFIG_HPP
#define SCOREP_INSTRUMENTER_INSTALL_CONFIG_HPP

/**
 * @file
 *
 * @brief      Declares a class which represent the install configuration of
 *             this Score-P installation.
 */

#include <SCOREP_ErrorCodes.h>
#include <string>

#include <scorep_tools_config_parser.hpp>

class SCOREP_Instrumenter_CmdLine;

/* ****************************************************************************
   class SCOREP_Instrumenter_InstallData
******************************************************************************/

/**
 * This class represents the current installation configuration of Score-P to
 * the instrumenter. Thus, it provides installation dependent data. By default,
 * it uses values determined during configure time of the Score-P build.
 * However, it may read-in data from a config file.
 */
class SCOREP_Instrumenter_InstallData
    : public SCOREP_Tools_ConfigParser
{
    /* ****************************************************** Public methods */
public:
    /**
       Creates a new SCOREP_Instrumenter_InstallData object.
     */
    SCOREP_Instrumenter_InstallData( void );

    /**
       Destroys a SCOREP_Instrumenter_InstallData object.
     */
    virtual
    ~SCOREP_Instrumenter_InstallData();

    /**
       Returns the scorep-config tool.
     */
    std::string
    getScorepConfig( void );

    /**
       C compiler command
     */
    std::string
    getCC( void );

    /**
       C++ compiler command
     */
    std::string
    getCXX( void );

    /**
       Fortran compiler command
     */
    std::string
    getFC( void );

    /**
       Perfroms the changes on the install data retrieval if it is a build
       check run, performed in the build directory instead of the install
       directory.
     */
    void
    setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine );

    /* ************************************** Compiler dependent implemented */

    /**
       Checks whether @a arg is the compiler option to build a shared library.
       @param arg the argument that is considered
     */
    static bool
    isArgForShared( const std::string& arg );

    /**
       Checks whether @a arg is the compiler option to enable free form Fortran
       @param arg the argument that is considered
     */
    static bool
    isArgForFreeform( const std::string& arg );

    /**
       Checks whether @a arg is the compiler option to enable fixed form Fortran
       @param arg the argument that is considered
     */
    static bool
    isArgForFixedform( const std::string& arg );

    static std::string
    getCPreprocessingFlags( const std::string& input_file,
                            const std::string& output_file );

    static std::string
    getCxxPreprocessingFlags( const std::string& input_file,
                              const std::string& output_file );

    static std::string
    getFortranPreprocessingFlags( const std::string& input_file,
                                  const std::string& output_file );

    /**
       Returns envaronment variables needed by the compiler.
       If the string is non empty, it must include a final space character.
     */
    static std::string
    getCompilerEnvironmentVars( void );

    /**
       Checks whether an argument that starts with -o is an argument that does not
       specify an output file name.
     */
    static bool
    isArgWithO( const std::string& arg );

    /**
       Checks whether an argument instructs the compiler to only preprocess.
     */
    static bool
    isPreprocessFlag( const std::string& arg );

    /**
       Checks whether the @a current parameter expects an argument in the next
       argument.
       @param current  The current parameter.
       @param next     The next parameter.
     */
    static bool
    isCompositeArg( const std::string& current,
                    const std::string& next );

    /**
       Checks whether the current @a arg parameter conflicts with linktime wrapping.
       @param arg   The current parameter.
     */
    static bool
    conflictsWithLinktimeWrapping( const std::string& arg );

    /* ***************************************************** Private methods */
private:
    /**
       This function gives a (key, value) pair found in a configuration file
       and not processed by one of the former functions.
       @param key   The key
       @param value The value
     */
    void
    set_value( const std::string& key,
               const std::string& value );


    /**
       This function processes a setting of the PDT path from config file.
       @param pdt A string containing the binary directory of the PDT
       installation.
     */
    void
    set_pdt_path( const std::string& pdt );

    /* ***************************************************** Private members */
private:
    /**
       The scorep-config tool.
     */
    std::string m_scorep_config;

    /**
       C compiler command
     */
    std::string m_c_compiler;

    /**
       C++ compiler command
     */
    std::string m_cxx_compiler;

    /**
       Fortran compiler command
     */
    std::string m_fortran_compiler;
};

#endif // SCOREP_INSTRUMENTER_INSTALL_CONFIG_HPP
