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


#ifndef SILC_INSTRUMENTER_H_
#define SILC_INSTRUMENTER_H_

#include <iostream>
#include <string>


#include "Silc_Application.hpp"

/** @def
 * list of supported languages
 */
typedef enum { LANG_CC, LANG_CXX, LANG_F77, LANG_F90 } SILC_LangType;

/** @def
 * list of supported intrumentation types
 */
typedef enum
{
    /**
     * auto. instr. by GNU compiler
     */
    INST_TYPE_GNU,

    /**
     * PGI
     */
    INST_TYPE_PGI,

    /**
     * PGI9
     */
    INST_TYPE_PGI9,

    /**
     * SUM
     */
    INST_TYPE_SUN,
    /**
     * IBM
     */
    INST_TYPE_XL,
    /**
     * NEC SX
     */
    INST_TYPE_FTRACE,
    /**
     * OpenUH
     */
    INST_TYPE_OPENUH,

    /**
     * binary instrumentation by Dyninst
     */
    INST_TYPE_DYNINST,

    /**
     * manual user instrumentation
     */
    INST_TYPE_USER,

    /**
     * sequential instrumentation
     */
    INST_TYPE_SEQ,

    /**
     * manual MPI instrumentation
     */
    INST_TYPE_MPI,

    /**
     * manual OpenMP instrumentation
     */
    INST_TYPE_OPENMP,

    /**
     * manual hybrid instrumentation
     */
    INST_TYPE_HYBRID,

    /**
     * default instrumentation value
     */
    INST_TYPE_DEFAULT
} SILC_InstType;



/*! @brief Class Silc_Instrumenter.
 *         performes instrumentation stage
 *
 *  This class examines the available compiler settings and the type of instrumentation
 */

class Silc_Instrumenter : public Silc_Application
{
public:



/** @brief
 * default constructor
 */
    Silc_Instrumenter
    (
    );
    virtual ~
    Silc_Instrumenter
        ();

/** @brief
 * reads the defined input data file
 */
    virtual SILC_Error_Code
    silc_readConfigFile
    (
        std::string fileName
    );

/** @brief
 * examines environment variables
 */
    SILC_Error_Code
    silc_readEnvVars
    (
    );


/** @brief
 * get command line attributes
 */
    virtual SILC_Error_Code
    silc_parseCmdLine
    (
        int    argc,
        char** argv
    );

/** @brief
 * perform instrumentation stage
 */
    virtual int
    silc_run
    (
    );

/** @brief
 * used language of user code
 */
    SILC_Error_Code
    silc_setLanguage
    (
        const int lang
    );

/** @brief
 * compiler commands
 */
    void
    silc_compilerCmd
    (
        const std::string cmd
    );

/** @brief
 * intrumentation type
 */
    void
    silc_instString
    (
        const std::string cmd
    );

/** @brief
 *  list of compiler flags
 */
    void
    silc_compilerArg
    (
        const std::string arg
    );

/** @brief
 *  list of libraries
 */
    void
    silc_compilerLib
    (
        const std::string lib
    );

/** @brief
 *  extracts parameter from input file
 */
    SILC_Error_Code
    silc_readParameter
    (
        std::string &     instring,
        const std::string parameter,
        std::string &     value
    );

/** @brief
 *  check for given instrumentation type
 */
    void
    silc_instType
    (
        std::string insttype
    );



/** @brief
 *  prints all instrumentation parameters as read from input file
 */
    virtual void
    silc_printParameter
    (
    );


private:

/** @brief
 *  instrumentation type
 */
    SILC_InstType _instType;


/** @brief
 *  used language
 */
    std::string _language;

/** @brief
 *  instrumentation string
 */
    std::string _instString;

/** @brief
 *  compiler command
 */
    std::string _compiler;

/** @brief
 *  compiler flags
 */
    std::string _compFlags;

/** @brief
 *  compiler command
 */
    std::string _linkerFlags;

/** @brief
 *  compiler arguments
 */
    std::string _libraries;

/** @brief
 *  compiler flags
 */
    std::string _inclDir;

/** @brief
 *  linker flags
 */
    std::string _libDir;

/** @brief
 *  compiler instrumentation flag
 */
    std::string _instDefault;

/** @brief
 *  available compiler instrumentation
 */
    std::string _instAvail;

/** @brief
 *  gnu compiler instrumentation flags
 */
    std::string _instGnu;

/** @brief
 *  pgi compiler instrumentation flags
 */
    std::string _instPgi;

/** @brief
 *  pgi9 compiler instrumentation flags
 */
    std::string _instPgi9;

/** @brief
 *  sun compiler instrumentation flags
 */
    std::string _instSun;

/** @brief
 *  xl compiler instrumentation flags
 */
    std::string _instXl;

/** @brief
 *  ftrace compiler instrumentation flags
 */
    std::string _instFtrace;

/** @brief
 *  openUH compiler instrumentation flags
 */
    std::string _instOpenuh;

// additional members for additional compiler settings
};

#endif /*SILC_INSTRUMENTER_H_*/
