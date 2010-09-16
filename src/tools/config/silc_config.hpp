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
 *
 * @maintainer Johannes Spazier <johannes.spazier@tu-dresden.de>
 * @autors     Johannes Spazier <johannes.spazier@tu-dresden.de>
 *
 */

#include <iostream>
#include <string>

#include "silc_utility/SILC_ParserBase.hpp"

class SILC_Config : public SILC_ParserBase
{
private:

    std::string config_file;

public:

    std::string str_cc;
    std::string str_libs;
    std::string str_flags;
    std::string str_libdir;
    std::string str_incdir;

    /** constructor and destructor */
    SILC_Config();

    virtual ~
    SILC_Config();


    /** */
    virtual int
    ParseConfigFile( char* arg );


    /** these methods must be overwritten, but they are not needed in class SILC_Config */
    virtual SILC_Error_Code
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
};
