/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 * @status alpha
 * The instrumentation tool of Score-P: scorep.
 * .
 */
#include <config.h>
#include "scorep_instrumenter.hpp"
#include "scorep_instrumenter_adapter.hpp"
#include <scorep_config_tool_backend.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <deque>

/**
   Contains the name of the tool for help output
 */
const std::string toolname = "scorep";

/**
    Prints a short usage message.
 */
void
print_short_usage( void )
{
    std::cout << "\nThis is the SCOREP instrumentation tool. The usage is:\n"
              << toolname << " <options> <orginal command>\n\n"
              << "To print out more detailed help information on available parameters, "
              << "type\n"
              << toolname << " --help\n"
              << std::endl;
}

/**
   Prints the long help text.
 */
void
print_help( void )
{
    std::cout << "\nThis is the SCOREP instrumentation tool. The usage is:\n"
              << toolname << " <options> <orginal command>\n\n"
              << "Common options are:\n"
              << "  --help, -h      Show help output. Does not execute any other command.\n"
              << "  --config=<file> Specifies file for the instrumentation configuration.\n"
              << "  -v, --verbose[=<value>] Specifies the verbosity level. The following\n"
              << "                  levels are available:\n"
              << "                  0 = No output\n"
              << "                  1 = Executed commands are displayed (default if no\n"
              << "                      value is specified)\n"
              << "                  2 = Detailed information is displayed\n"

              << "  --dry-run       Only displays the executed commands. It does not\n"
              << "                  execute any command.\n"
              << "  --keep-files    Do not delete temporarily created files after successfull\n"
              << "                  instrumentation. By default, temporary files are deleted\n"
              << "                  if no error occures during instrumentation.\n"
              << "  --version       Prints the Score-P version and exits.\n"
              << "  --mpi           Enables mpi wrapper. They are enabled by default if it is an\n"
              << "                  mpi program.\n"
              << "  --nompi         Disables mpi wrappers. They are disabled by default if\n"
              << "                  it is no mpi program.\n"
              << "  --openmp        Enables OpenMP support. Needed if the instrumentation\n"
              << "                  does not correctly identify your application as OpenMP\n"
              << "                  program.\n"
              << "  --noopenmp      Disables OpenMP support.\n"
#if defined( SCOREP_SHARED_BUILD ) && defined ( SCOREP_STATIC_BUILD )
#if HAVE_LINK_FLAG_BSTATIC
    << "  --static        Enforce static linking of the Score-P libraries.\n"
#endif
#if HAVE_LINK_FLAG_BDYNAMIC
    << "  --dynamic       Enforce dynamic linking of the Score-P libraries."
#endif
#endif
    << std::endl;
    SCOREP_Instrumenter_Adapter::printAll();
    std::cout << std::endl;
}

/**
   Main routine of the scorep instrumentation tool.
   @param argc Number of arguments.
   @param argv List of arguments
   @returns If an error occurs, -1 is returned, if help was called, 0 is
            returned. Else it returns the return value from the user command.
 */
int
main( int   argc,
      char* argv[] )
{
    if ( argc < 2 )
    {
        print_short_usage();
        return EXIT_FAILURE;
    }

    SCOREP_Instrumenter_InstallData install_data;
    SCOREP_Instrumenter_CmdLine     command_line( install_data );
    SCOREP_Instrumenter             app( install_data, command_line );

    command_line.ParseCmdLine( argc, argv );
    return app.Run();
}
