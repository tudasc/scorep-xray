/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
    std::string str_libs;
    std::string str_flags;
    std::string str_libdir;
    std::string str_incdir;

    /** constructor and destructor */
    SCOREP_Config();

    virtual ~
    SCOREP_Config();


    /** */
    virtual SCOREP_Error_Code
    ParseConfigFile( char* arg );


    /** these methods must be overwritten, but they are not needed in class SCOREP_Config */
    virtual SCOREP_Error_Code
    ParseCmdLine( int    argc,
                  char** argv );

    virtual int
    Run();

    virtual void
    PrintParameter();


    /** callbacks */

    virtual void
    SetCompilerFlags( std::string flags );

    virtual void
    AddIncDir( std::string dir );

    virtual void
    AddLibDir( std::string dir );

    virtual void
    AddLib( std::string lib );

    virtual void
    SetCompiler( std::string value );

    virtual void
    SetCxx( std::string value );

    virtual void
    SetFc( std::string value );

    virtual void
    SetPrefix( std::string value );
};
