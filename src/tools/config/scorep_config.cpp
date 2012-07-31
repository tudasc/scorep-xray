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
 * @status alpha
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @autors     Johannes Spazier <johannes.spazier@tu-dresden.de>
 * @autors     Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include <SCOREP_IO.h>

#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>
#include <scorep_config.hpp>
#include <SCOREP_Config_LibraryDependencies.hpp>

#define MODE_SEQ 0
#define MODE_OMP 1
#define MODE_MPI 2
#define MODE_HYB 3

#define ACTION_LIBS   1
#define ACTION_CFLAGS 2
#define ACTION_INCDIR 3
#define ACTION_CC     4
#define ACTION_CXX    5
#define ACTION_FC     6
#define ACTION_MPICC  7
#define ACTION_MPICXX 8
#define ACTION_MPIFC  9

#define SHORT_HELP \
    "\nUsage:\nscorep-config <command> [<options>]\n\n" \
    "To print out more detailed help information on available parameters, type\n" \
    "scorep-config --help\n"


#define HELPTEXT \
    "\nUsage:\nscorep-config <command> [<options>]\n" \
    "  Commands:\n" \
    "   --cflags  prints additional compiler flags. They already contain the\n" \
    "             include flags.\n" \
    "   --inc     prints the include flags. They are already contained in the\n" \
    "             output of the --cflags command\n" \
    "   --libs    prints the required linker flags\n" \
    "   --cc      prints the C compiler name\n" \
    "   --cxx     prints the C++ compiler name\n" \
    "   --fc      prints the Fortran compiler name\n" \
    "   --mpicc   prints the MPI C compiler name\n" \
    "   --mpicxx  prints the MPI C++ compiler name\n" \
    "   --mpifc   prints the MPI Fortran compiler name\n" \
    "   --help    prints this usage information\n" \
    "   --version prints the version number of the scorep package\n" \
    "   --scorep-revision prints the revision number of the scorep package\n" \
    "   --common-revision prints the revision number of the common package\n\n" \
    "  Options:\n" \
    "   --seq|--omp|--mpi|--hyb\n" \
    "            specifys the mode: seqential, OpenMP, MPI, or hybrid (MPI + OpenMP)\n" \
    "            Takes effect only for the --libs command. The default mode is MPI.\n\n" \
    "   --config=<config_file>\n" \
    "            Allows to specify a configuration file which overrides the data\n" \
    "            specified at build time of the tool.\n\n" \
    "   --user|--nouser\n" \
    "            Specifies whether manual user instrumentation is used. On default\n" \
    "            user instrumentation is disabled.\n\n" \
    "   --compiler|--nocompiler\n" \
    "            Specifies whether compiler instrumentation is used. On default\n" \
    "            compiler instrumentation is enabled.\n\n" \
    "   --fortran   Specifies that the required flags are for the Fortran compiler.\n\n" \
    "   --cuda   Specifies that the required flags are for the CUDA compiler.\n\n"


std::string
prepare_string( std::string str );

std::string
remove_multiple_whitespaces( std::string str );

std::string
replace_all( std::string &pattern,
             std::string &replacement,
             std::string  original );

int
main( int    argc,
      char** argv )
{
    int i;
    /* set default mode to mpi */
    int  mode     = MODE_MPI;
    int  action   = 0;
    int  ret      = EXIT_SUCCESS;
    bool user     = false;
    bool compiler = true;
    bool fortran  = false;
    bool cuda     = false;
    bool install  = true;

    const std::string scorep_libs[ 4 ] = { "scorep_serial",
                                           "scorep_omp",
                                           "scorep_mpi",
                                           "scorep_mpi_omp" };

    SCOREP_Config                     app( argv[ 0 ] );
    SCOREP_Config_LibraryDependencies deps;

    /* parsing the command line */
    for ( i = 1; i < argc; i++ )
    {
        if ( app.CheckForCommonArg( argv[ i ] ) )
        {
            continue;
        }
        else if ( strcmp( argv[ i ], "--help" ) == 0 || strcmp( argv[ i ], "-h" ) == 0 )
        {
            std::cout << HELPTEXT << std::endl;
            return EXIT_SUCCESS;
        }
        else if ( strcmp( argv[ i ], "--version" ) == 0 )
        {
            std::cout << PACKAGE_VERSION;
            std::cout.flush();
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--scorep-revision" ) == 0 )
        {
            std::cout << SCOREP_COMPONENT_REVISION << std::endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--common-revision" ) == 0 )
        {
            std::cout << SCOREP_COMMON_REVISION << std::endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--seq" ) == 0 )
        {
            mode = MODE_SEQ;
        }
        else if ( strcmp( argv[ i ], "--omp" ) == 0 )
        {
            mode = MODE_OMP;
        }
        else if ( strcmp( argv[ i ], "--mpi" ) == 0 )
        {
            mode = MODE_MPI;
        }
        else if ( strcmp( argv[ i ], "--hyb" ) == 0 )
        {
            mode = MODE_HYB;
        }
        else if ( strcmp( argv[ i ], "--libs" ) == 0 )
        {
            action = ACTION_LIBS;
        }
        else if ( strcmp( argv[ i ], "--cflags" ) == 0 )
        {
            action = ACTION_CFLAGS;
        }
        else if ( strcmp( argv[ i ], "--inc" ) == 0 )
        {
            action = ACTION_INCDIR;
        }
        else if ( strcmp( argv[ i ], "--cc" ) == 0 )
        {
            action = ACTION_CC;
        }
        else if ( strcmp( argv[ i ], "--cxx" ) == 0 )
        {
            action = ACTION_CXX;
        }
        else if ( strcmp( argv[ i ], "--fc" ) == 0 )
        {
            action = ACTION_FC;
        }
        else if ( strcmp( argv[ i ], "--mpicc" ) == 0 )
        {
            action = ACTION_MPICC;
        }
        else if ( strcmp( argv[ i ], "--mpicxx" ) == 0 )
        {
            action = ACTION_MPICXX;
        }
        else if ( strcmp( argv[ i ], "--mpifc" ) == 0 )
        {
            action = ACTION_MPIFC;
        }
        else if ( strcmp( argv[ i ], "--user" ) == 0 )
        {
            user = true;
        }
        else if ( strcmp( argv[ i ], "--nouser" ) == 0 )
        {
            user = false;
        }
        else if ( strcmp( argv[ i ], "--compiler" ) == 0 )
        {
            compiler = true;
        }
        else if ( strcmp( argv[ i ], "--nocompiler" ) == 0 )
        {
            compiler = false;
        }
        else if ( strcmp( argv[ i ], "--fortran" ) == 0 )
        {
            fortran = true;
        }
        else if ( strcmp( argv[ i ], "--cuda" ) == 0 )
        {
            cuda = true;
        }
        else if ( strcmp( argv[ i ], "--build-check" ) == 0 )
        {
            install = false;
        }

        else
        {
            std::cerr << "\nUnknown option " << argv[ i ] << ". Abort.\n" << std::endl;
            exit( EXIT_FAILURE );
        }
    }

    /* print data in case a config file was specified */
    std::string str;
    if ( app.IsConfigFileSet() )
    {
        if ( app.ParseConfigFile( argv[ 0 ] ) != SCOREP_SUCCESS )
        {
            std::cerr << "Unable to open config file." << std::endl;
            abort();
        }

        switch ( action )
        {
            case ACTION_LIBS:
                str = app.m_libdir + app.m_rpath + " -l" + scorep_libs[ mode ] + app.m_libs;
                if ( cuda )
                {
                    str                = " -Xlinker " + prepare_string( str );
                    app.m_otf2_config += " --cuda";
                }
                std::cout << str;
                std::cout.flush();

                app.m_otf2_config += " --libs";
                ret                = system( app.m_otf2_config.c_str() );

                break;

            case ACTION_CFLAGS:
                if ( compiler )
                {
                    str += "-g " + app.m_flags;
                }
                if ( user )
                {
                    if ( fortran )
                    {
                       #ifdef SCOREP_COMPILER_IBM
                        str += "-WF,-DSCOREP_USER_ENABLE ";
                       #else
                        str += "-DSCOREP_USER_ENABLE ";
                       #endif // SCOREP_COMPILER_IBM
                    }
                    else
                    {
                        str += "-DSCOREP_USER_ENABLE ";
                    }
                }

                #ifdef SCOREP_COMPILER_IBM
                if ( fortran && ( ( mode == MODE_OMP ) || ( mode == MODE_HYB ) ) )
                {
                    str += "-d -WF,-qlanglvl=classic ";
                }
                #endif

            // Append the include directories, too
            case ACTION_INCDIR:
                str += app.m_incdir;
                if ( cuda )
                {
                    str = " -Xcompiler " + prepare_string( str );
                    //app.m_otf2_config += " --cuda";
                }

                std::cout << str;
                std::cout.flush();
                //app.m_otf2_config += " --cflags";
                //ret                  = system( app.m_otf2_config.c_str() );
                break;

            case ACTION_CC:
                std::cout << app.m_cc;
                std::cout.flush();
                break;

            case ACTION_CXX:
                std::cout << app.m_cxx;
                std::cout.flush();
                break;

            case ACTION_FC:
                std::cout << app.m_fc;
                std::cout.flush();
                break;

            case ACTION_MPICC:
                std::cout << app.m_mpicc;
                std::cout.flush();
                break;

            case ACTION_MPICXX:
                std::cout << app.m_mpicxx;
                std::cout.flush();
                break;

            case ACTION_MPIFC:
                std::cout << app.m_mpifc;
                std::cout.flush();
                break;

            default:
                std::cout << SHORT_HELP << std::endl;
                break;
        }
    }
    else
    {
        std::deque<std::string> libs;
        libs.push_back( "lib" + scorep_libs[ mode ] );

        switch ( action )
        {
            case ACTION_LIBS:
                std::cout << deps.GetLDFlags( libs, install );
                str = deps.GetRpathFlags( libs, install,
                                          app.m_rpath_head,
                                          app.m_rpath_delimiter );
                if ( cuda )
                {
                    str = " -Xlinker " + prepare_string( str );
                }
                std::cout << str;
                std::cout << deps.GetLibraries( libs );
                std::cout.flush();
                break;

            case ACTION_CFLAGS:
                if ( compiler )
                {
                    str += "-g " SCOREP_CFLAGS " ";
                }
                if ( user )
                {
                    if ( fortran )
                    {
                       #ifdef SCOREP_COMPILER_IBM
                        str += "-WF,-DSCOREP_USER_ENABLE ";
                       #else
                        str += "-DSCOREP_USER_ENABLE ";
                       #endif // SCOREP_COMPILER_IBM
                    }
                    else
                    {
                        str += "-DSCOREP_USER_ENABLE ";
                    }
                }

                #ifdef SCOREP_COMPILER_IBM
                if ( fortran && ( ( mode == MODE_OMP ) || ( mode == MODE_HYB ) ) )
                {
                    str += "-d -WF,-qlanglvl=classic ";
                }
                #endif

            // Append the include directories, too
            case ACTION_INCDIR:
                if ( install )
                {
                    str += "-I" SCOREP_PREFIX "/include -I" SCOREP_PREFIX "/include/scorep ";
                }
                else
                {
                    str += "-I" BUILD_SCOREP_PREFIX "/include -I" BUILD_SCOREP_PREFIX "/include/scorep ";
                }
                if ( cuda )
                {
                    str = " -Xcompiler " + prepare_string( str );
                }

                std::cout << str;
                std::cout.flush();
                break;

            case ACTION_CC:
                std::cout << SCOREP_CC;
                std::cout.flush();
                break;

            case ACTION_CXX:
                std::cout << SCOREP_CXX;
                std::cout.flush();
                break;

            case ACTION_FC:
                std::cout << SCOREP_FC;
                std::cout.flush();
                break;

            case ACTION_MPICC:
                std::cout << SCOREP_MPICC;
                std::cout.flush();
                break;

            case ACTION_MPICXX:
                std::cout << SCOREP_MPICXX;
                std::cout.flush();
                break;

            case ACTION_MPIFC:
                std::cout << SCOREP_MPIFC;
                std::cout.flush();
                break;

            default:
                std::cout << SHORT_HELP << std::endl;
                break;
        }
    }

    return ret;
}


/** constructor and destructor */
SCOREP_Config::SCOREP_Config( char* arg0 )
{
    char* path = SCOREP_GetExecutablePath( arg0 );
    m_otf2_config = "otf2-config --backend";
    if ( path != NULL )
    {
        m_otf2_config = "/" + m_otf2_config;
        m_otf2_config = path + m_otf2_config;
    }
    m_cc     = SCOREP_CC;
    m_cxx    = SCOREP_CXX;
    m_fc     = SCOREP_FC;
    m_mpicc  = SCOREP_MPICC;
    m_mpicxx = SCOREP_MPICXX;
    m_mpifc  = SCOREP_MPIFC;

    // Replace ${wl} by LIBDIR_FLAG_WL and erase everything from
    // $libdir on in order to create m_rpath_head and
    // m_rpath_delimiter. This will work for most and for the relevant
    // (as we know in 2012-07) values of LIBDIR_FLAG_CC. Possible
    // values are (see also ticket 530,
    // https://silc.zih.tu-dresden.de/trac-silc/ticket/530):
    // '+b $libdir'
    // '-L$libdir'
    // '-R$libdir'
    // '-rpath $libdir'
    // '${wl}-blibpath:$libdir:'"$aix_libpath"
    // '${wl}+b ${wl}$libdir'
    // '${wl}-R,$libdir'
    // '${wl}-R $libdir:/usr/lib:/lib'
    // '${wl}-rpath,$libdir'
    // '${wl}--rpath ${wl}$libdir'
    // '${wl}-rpath ${wl}$libdir'
    // '${wl}-R $wl$libdir'
    std::string rpath_flag = LIBDIR_FLAG_CC;
    size_t      index      = 0;
    while ( true )
    {
        index = rpath_flag.find( "${wl}", index );
        if ( index == std::string::npos )
        {
            break;
        }
        rpath_flag.replace( index, strlen( "${wl}" ), LIBDIR_FLAG_WL );
        ++index;
    }
    index = rpath_flag.find( "$libdir", 0 );
    rpath_flag.erase( index );

    bool is_aix = ( "" != LIBDIR_AIX_LIBPATH );
    if ( is_aix )
    {
        m_rpath_head      = " " + rpath_flag;
        m_rpath_delimiter = ":";
    }
    else
    {
        m_rpath_head      = "";
        m_rpath_delimiter = " " + rpath_flag;
    }

    free( path );
}

SCOREP_Config::~SCOREP_Config()
{
}


/** */
SCOREP_Error_Code
SCOREP_Config::ParseConfigFile( char* arg0 )
{
    std::string arg = std::string( arg0 );
    return this->ReadConfigFile( arg );
}

/** callbacks */
void
SCOREP_Config::SetValue( std::string key,
                         std::string value )
{
    if ( key == "COMPILER_INSTRUMENTATION_CPPFLAGS" )
    {
        this->m_flags += value + " ";
    }
    else if ( key == "CC" && value != "" )
    {
        this->m_cc = value;
    }
    else if ( key == "CXX" && value != "" )
    {
        this->m_cxx = value;
    }
    else if ( key == "FC" && value != "" )
    {
        this->m_fc = value;
    }
    else if ( key == "MPICC" && value != "" )
    {
        this->m_mpicc = value;
    }
    else if ( key == "MPICXX" && value != "" )
    {
        this->m_mpicxx = value;
    }
    else if ( key == "MPIFC" && value != "" )
    {
        this->m_mpifc = value;
    }
    else if ( key == "PREFIX" && value != "" )
    {
        AddIncDir( value + "/include/scorep" );
        AddLibDir( value + "/lib" );
    }
    else if ( key == "OTF2_CONFIG" && value != "" )
    {
        this->m_otf2_config = value + " --backend";
    }
}

void
SCOREP_Config::AddIncDir( std::string dir )
{
    std::string incdir = "-I" + dir;

    if ( std::string::npos == this->m_incdir.find( incdir ) )
    {
        this->m_incdir += " " + incdir;
    }
}

void
SCOREP_Config::AddLibDir( std::string dir )
{
    if ( dir != "" )
    {
        std::string libdir = "-L" + dir + " ";

        if ( std::string::npos == this->m_libdir.find( libdir ) )
        {
            this->m_libdir += libdir;


            if ( this->m_rpath == "" )
            {
                this->m_rpath = this->m_rpath_head;
            }
            this->m_rpath += this->m_rpath_delimiter + dir;
        }
    }
}

void
SCOREP_Config::AddLib( std::string lib )
{
    if ( lib != "" )
    {
        if ( std::string::npos == this->m_libs.find( lib ) )
        {
            this->m_libs += " " + lib;
        }
    }
}

/** Make string with compiler or linker flags compatible to CUDA
 *  compiler requirements.
 *
 *  @param str              String to be processed.
 *
 *  @return Returns string with compiler or linker flags that can be
 *          passes to CUDA compiler.
 */
std::string
prepare_string( std::string str )
{
    std::string pattern1 = " ";
    std::string replace1 = ",";
    std::string pattern2 = LIBDIR_FLAG_WL;
    std::string replace2 = "";

    str = remove_multiple_whitespaces( str );
    /* Replace all white-spaces by comma */
    str = replace_all( pattern1, replace1, str );
    /* Replace flag for passing arguments to linker through compiler
     * (flags not needed because we use '-Xlinker' to specify linker
     * flags when using CUDA compiler */
    if ( pattern2.length() != 0 )
    {
        str = replace_all( pattern2, replace2, str );
    }

    return str;
}

/** Trim  and replace multiple white-spaces in @ str by a single one.
 *
 *  @param str              String to be processed.
 *
 *  @return Returns string where all multiple white-spaces are replaced
 *          by a single one.
 */
std::string
remove_multiple_whitespaces( std::string str )
{
    std::string            search = "  "; // this string contains 2 spaces
    std::string::size_type pos;

    /* Trim */
    pos = str.find_last_not_of( ' ' );
    if ( pos != std::string::npos )
    {
        str.erase( pos + 1 );
        pos = str.find_first_not_of( ' ' );
        if ( pos != std::string::npos )
        {
            str.erase( 0, pos );
        }
    }
    else
    {
        str.erase( str.begin(), str.end() );
    }

    /* Remove multiple white-spaces */
    while ( ( pos = str.find( search ) ) != std::string::npos )
    {
        /* remove 1 character from the string at index */
        str.erase( pos, 1 );
    }

    return str;
}

/** Replace all occurrences of @ pattern in string @ original by
 *  @ replacement.
 *
 *  @param pattern          String that should be replaced.
 *  @param replacement      Replacement for @ pattern.
 *  @param original         Input string.
 *
 *  @return Returns a string where all occurrences of @ pattern are
 *          replaced by @ replacement.
 */
std::string
replace_all( std::string &pattern,
             std::string &replacement,
             std::string  original )
{
    std::string::size_type pos            = original.find( pattern, 0 );
    int                    pattern_length = pattern.length();

    while ( pos != std::string::npos )
    {
        original.replace( pos, pattern_length, replacement );
        pos = original.find( pattern, 0 );
    }

    return original;
}
