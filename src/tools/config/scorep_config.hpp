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

#include <SCOREP_ParserBase.hpp>

class SCOREP_Config : public SCOREP_ParserBase
{
private:

    std::string config_file;

public:

    std::string m_cc;
    std::string m_cxx;
    std::string m_fc;
    std::string m_mpicc;
    std::string m_mpicxx;
    std::string m_mpifc;
    std::string m_libs;
    std::string m_flags;
    std::string m_libdir;
    std::string m_rpath;
    std::string m_rpath_head;
    std::string m_rpath_delimiter;
    std::string m_incdir;
    std::string m_otf2_config;

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
