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
     * manual instrumentation
     */
    INST_TYPE_MANUAL,
    /**
     * binary instrumentation by Dyninst
     */
    INST_TYPE_DYNINST
} SILC_InstType;



/*! @brief Class Silc_Instrumenter.
 *         performes instrumentation stage
 *
 *  This class examines the available compiler settings and the type of instrumentation
 */

class Silc_Instrumenter : public Silc_Application {
public:



/** @brief
 * default constructor
 */
    Silc_Instrumenter();
    virtual ~Silc_Instrumenter();

/** @brief
 * reads the defined input data file
 */
    virtual bool
    silc_readConfigFile
    (
        std::string fileName
    );

/** @brief
 * examines environment variables
 */
    bool
    silc_readEnvVars
    (
    );


/** @brief
 * get command line attributes
 */
    bool
    silc_parseCmdLine
    (
        int    argc,
        char** argv
    );

/** @brief
 * perform instrumentation stage
 */
    int
    silc_run
    (
    );


/** @brief
 * list of getter functions
 */

/** @brief
 * used language of user code
 */
    bool
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
    silc_instType
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
    bool
    readParameter
    (
        std::string &     instring,
        const std::string parameter,
        std::string &     value
    );



private:

/** @brief
 *  instrumentation type
 */
    std::string instType;

/** @brief
 *  compiler command env. name
 */
    std::string compCmdEnv;

/** @brief
 *  compiler flags
 */
    std::string compFlagsEnv;

/** @brief
 *  compiler command
 */
    std::string compCmd;

/** @brief
 *  compiler arguments
 */
    std::string compArgs;

/** @brief
 *  compiler flags
 */
    std::string compFlags;

/** @brief
 *  linker flags
 */
    std::string compLdFlags;

/** @brief
 *  compiler instrumentation flag
 */
    std::string compInstFlags;

/** @brief
 *  libraries to link
 */
    std::string compLibs;

// additional members for additional compiler settings
};

#endif /*SILC_INSTRUMENTER_H_*/
