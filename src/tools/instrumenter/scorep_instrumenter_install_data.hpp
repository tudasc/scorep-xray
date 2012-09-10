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
 * @file       scorep_instrumenter_install_config.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Declares a class which represent the install configuration of
 *             this Score-P installation.
 */

#ifndef SCOREP_INSTRUMENTER_INSTALL_CONFIG_H_
#define SCOREP_INSTRUMENTER_INSTALL_CONFIG_H_

#include <UTILS_Error.h>
#include <string>

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
{
    /* ****************************************************** Public methods */
public:
    /**
       Creates a new SCOREP_Instrumenter_InstallData object.
     */
    SCOREP_Instrumenter_InstallData();

    /**
       Destroys a SCOREP_Instrumenter_InstallData object.
     */
    virtual
    ~SCOREP_Instrumenter_InstallData();

    /**
       Returns the scorep-config tool.
     */
    std::string
    getScorepConfig();

    /**
       C compiler command
     */
    std::string
    getCC();

    /**
       Stores C compiler OpenMP flags
     */
    std::string
    getOpenmpFlags();

    /**
       The nm command
     */
    std::string
    getNm();

    /**
       The awk command
     */
    std::string
    getAwk();

    /**
       The opari2 executable
     */
    std::string
    getOpari();

    /**
       The awk script used to generate functions for initialization of Opari2
       instrumented regions.
     */
    std::string
    getOpariScript();

    /**
       The opari2-config tool
     */
    std::string
    getOpariConfig();

    /**
       The grep command
     */
    std::string
    getGrep();

    /**
       The cobi executable
     */
    std::string
    getCobi();

    /**
        Directory of the Cobi configuration files
     */
    std::string
    getCobiConfigDir();

    /**
        Path to PDT binaries.
     */
    std::string
    getPdtBinPath();

    /**
        PDT instrumentation specificaion file.
     */
    std::string
    getPdtConfigFile();

    /**
       Reads configuration data from a config file
     */
    SCOREP_ErrorCode
    readConfigFile( std::string arg0 );

    /**
       Perfroms the changes on the install data retrieval if it is a build
       check run, performed in the build directory instead of the install
       directory.
     */
    void
    setBuildCheck();


    /* ***************************************************** Private methods */
private:
    /**
       This function gives a (key, value) pair found in a configuration file
       and not processed by one of the former functions.
       @param key   The key
       @param value The value
     */
    void
    set_value( std::string key,
               std::string value );


    /**
       This function processes a setting of the PDT path from config file.
       @param pdt A string containing the binary directory of the PDT
       installation.
     */
    void
    set_pdt_path( std::string pdt );

    /**
       Extracts parameter from configuration file
       It expects lines of the format key=value. Furthermore it truncates line
       at the scrpit comment character '#'.
       @param line    input line from the config file
       @returns SCOREP_SUCCESS if the line was successfully parsed. Else it
                returns an error code.
     */
    SCOREP_ErrorCode
    read_parameter( std::string line );


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
       Stores C compiler OpenMP flags
     */
    std::string m_openmp_cflags;

    /**
       The nm command
     */
    std::string m_nm;

    /**
       The awk command
     */
    std::string m_awk;

    /**
       The opari2 executable
     */
    std::string m_opari;

    /**
       The awk script used to generate functions for initialization of Opari2
       instrumented regions.
     */
    std::string m_opari_script;

    /**
       The opari2-config tool
     */
    std::string m_opari_config;

    /**
       The grep command
     */
    std::string m_grep;

    /**
       The cobi executable
     */
    std::string m_cobi;

    /**
        Directory of the Cobi configuration files
     */
    std::string m_cobi_config_dir;

    /**
        Path to PDT binaries.
     */
    std::string m_pdt_bin_path;

    /**
        PDT instrumentation specificaion file.
     */
    std::string m_pdt_config_file;
};

#endif
