/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_INSTRUMENTER_PREPROCESS_HPP
#define SCOREP_INSTRUMENTER_PREPROCESS_HPP

/**
 * @file scorep_instrumenter_preprocess.hpp
 *
 * Defines the class for the preprocessing.
 */

#include "scorep_instrumenter_adapter.hpp"

/* **************************************************************************************
 * class SCOREP_Instrumenter_PreprocessAdapter
 * *************************************************************************************/

/**
 * This class represents the source code preprocessing.
 */
class SCOREP_Instrumenter_PreprocessAdapter : public SCOREP_Instrumenter_Adapter
{
public:
    SCOREP_Instrumenter_PreprocessAdapter( void );

    /**
     * Preprocess a source file
     * @param input_file  Source file which is preprocessed.
     * @returns Filename for the preprocessed source file.
     */
    virtual std::string
    precompile( SCOREP_Instrumenter&         instrumenter,
                SCOREP_Instrumenter_CmdLine& cmdLine,
                const std::string&           source_file );
    virtual std::string
    getConfigToolFlag( void );
};

#endif
