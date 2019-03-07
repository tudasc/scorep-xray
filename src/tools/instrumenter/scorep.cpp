/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * The instrumentation tool of Score-P: scorep.
 * .
 */
#include <config.h>
#include "scorep_instrumenter.hpp"
#include "scorep_instrumenter_adapter.hpp"
#include "scorep_instrumenter_selector.hpp"
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
    std::cout << "\nThis is the Score-P instrumentation tool. The usage is:\n"
              << toolname << " <options> <original command>\n\n"
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
    std::cout << "\nThis is the Score-P instrumentation tool. The usage is:\n"
              << toolname << " <options> <original command>\n\n"
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
              << "  --keep-files    Do not delete temporarily created files after successful\n"
              << "                  instrumentation. By default, temporary files are deleted\n"
              << "                  if no error occurs during instrumentation.\n"
              << "  --instrument-filter=<file>\n"
              << "                  Specifies the filter file for filtering functions during\n"
              << "                  compile-time. Not supported by all instrumentation methods.\n"
              << "                  It applies the same syntax, as the one used by Score-P during\n"
              << "                  run-time.\n"
              << "  --version       Prints the Score-P version and exits.\n"
              << "  --disable-preprocessing\n"
              << "                  Tells scorep to skip all preprocessing related steps,\n"
              << "                  the input files are already preprocessed.\n"
#if defined( SCOREP_SHARED_BUILD ) && defined ( SCOREP_STATIC_BUILD )
        << "  --static        Enforce static linking of the Score-P libraries.\n"
        << "  --dynamic       Enforce dynamic linking of the Score-P libraries.\n"
#endif
#if defined( SCOREP_SHARED_BUILD )
        << "  --no-as-needed  Adds a GNU ld linker flag to fix undefined references\n"
        "                  when using shared Score-P libraries. This happens on\n"
        "                  systems using --as-needed as linker default. It will\n"
        "                  be handled transparently in future releases of Score-P.\n"
#endif
    ;
    SCOREP_Instrumenter_Selector::printAll();
    SCOREP_Instrumenter_Adapter::printAll();
    std::cout << std::endl;
    std::cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">" << std::endl;
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
