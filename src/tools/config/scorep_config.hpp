/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 *
 * @maintainer Johannes Spazier <johannes.spazier@tu-dresden.de>
 * @autors     Johannes Spazier <johannes.spazier@tu-dresden.de>
 *
 */

#include <iostream>
#include <string>

#include "scorep_utility/SCOREP_ParserBase.hpp"

class SCOREP_Config : public SCOREP_ParserBase
{
private:

    std::string config_file;

public:

    std::string str_cc;
    std::string str_cxx;
    std::string str_fc;
    std::string str_mpicc;
    std::string str_mpicxx;
    std::string str_mpifc;
    std::string str_libs;
    std::string str_flags;
    std::string str_libdir;
    std::string str_incdir;
    std::string str_otf2_config;

    /** constructor and destructor */
    SCOREP_Config( char* arg0 );

    virtual
    ~
    SCOREP_Config();

    /** */
    virtual SCOREP_Error_Code
    ParseConfigFile( char* arg );

    /** callbacks */

    virtual void
    AddIncDir( std::string dir );

    virtual void
    AddLibDir( std::string dir );

    virtual void
    AddLib( std::string lib );

    virtual void
    SetValue( std::string key,
              std::string value );
};
