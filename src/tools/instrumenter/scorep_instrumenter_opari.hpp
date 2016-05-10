/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_OPARI_HPP
#define SCOREP_INSTRUMENTER_OPARI_HPP

/**
 * @file
 *
 * Defines the class for the opari instrumentation.
 */

#include "scorep_instrumenter_adapter.hpp"


/* **************************************************************************************
 * class SCOREP_Instrumenter_OpariAdapter
 * *************************************************************************************/

/**
 * This class represents the Opari2 instrumentation.
 */
class SCOREP_Instrumenter_OpariAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_OpariAdapter( void );

    virtual bool
    checkOption( const std::string& arg );

    virtual bool
    isEnabled( void ) const;

    virtual void
    printHelp( void );

    /**
     * Instruments @a source_file with Opari.
     * @param source_file File name of the source file, that is instrumented.
     * @returns the file name of the instrumented source file.
     */
    virtual std::string
    precompile( SCOREP_Instrumenter&         instrumenter,
                SCOREP_Instrumenter_CmdLine& cmdLine,
                const std::string&           source_file );

    /**
     * Performs the necessary actions for linking Opari instrumented object
     * files and libraries. Thus, it runs the awk script on the objects,
     * creates the POMP2_Init Function, and compiles it.
     */
    virtual void
    prelink( SCOREP_Instrumenter&         instrumenter,
             SCOREP_Instrumenter_CmdLine& cmdLine );

    virtual std::string
    getConfigToolFlag( SCOREP_Instrumenter_CmdLine& cmdLine );

    virtual void
    setBuildCheck( SCOREP_Instrumenter_CmdLine& cmdLine );

    virtual void
    setConfigValue( const std::string& key,
                    const std::string& value );

    virtual bool
    checkCommand( const std::string& current,
                  const std::string& next );
    virtual void
    checkDefaults( void );

    void
    setTpdMode( const bool use_tpd );

    void
    enableOpenmpDefault( void );

private:
    /**
     * Invokes the opari tool to instrument a source file.
     * @param input_file   Source file which is instrumented.
     * @param output_file  Filename for the instrumented source file
     */
    void
    invoke_opari( SCOREP_Instrumenter& instrumenter,
                  const std::string&   input_file,
                  const std::string&   output_file );

    /**
     * Runs a script on a list of object files to generate the Pomp_Init
     * function.
     * @param object_files A list of space separated object file names.
     * @param output_file  Filename for the generated source file.
     */
    void
    invoke_awk_script( SCOREP_Instrumenter& instrumenter,
                       const std::string&   object_files,
                       const std::string&   output_file );

    /**
     * Compiles the generated source file.
     * @param input_file  Source file which is compiled.
     * @param output_file Filename for the obejct file.
     */
    void
    compile_init_file( SCOREP_Instrumenter& instrumenter,
                       const std::string&   input_file,
                       const std::string&   output_file );

    /**
     * Sets OPARI2 parameters to explicitly specify fixed or free form fortran.
     * @param is_free  True if free form is specifed. False specifies fixed form.
     */
    void
    set_fortran_form( bool is_free );

    /**
     * The opari2 executable
     */
    std::string m_opari;

    /**
     * The awk script used to generate functions for initialization of Opari2
     * instrumented regions.
     */
    std::string m_opari_script;

    /**
     * The opari2-config tool
     */
    std::string m_opari_config;

    /**
     * The nm command
     */
    std::string m_nm;

    /**
     * C compiler command
     */
    std::string m_c_compiler;

    /**
     * Specifies whether OpenMP pragma instrumentation is enabled.
     */
    instrumentation_usage_t m_openmp;

    /**
     * Specifies whether user instrumentation is enabled.
     */
    instrumentation_usage_t m_pomp;

    /**
     * Specifies whether OpenMP parallel regions are instrumented.
     */
    instrumentation_usage_t m_parallel;

    /**
     * Specifies whether --tpd should be used.
     */
    bool m_use_tpd;

    /**
     * Specifies the OpenMP compiler flag.
     */
    std::string m_openmp_cflag;

#if SCOREP_BACKEND_COMPILER_INTEL
    /**
     * Specifies whether -mmic was given on the command line.
     */
    bool m_mmic_set;

#endif  /* SCOREP_BACKEND_COMPILER_INTEL */
};

#endif // SCOREP_INSTRUMENTER_OPARI_HPP
