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
 * @file       scorep_instrumenter_opari.cpp
 */

#include <config.h>
#include "scorep_instrumenter_opari.hpp"
#include "scorep_instrumenter_cmd_line.hpp"
#include "scorep_instrumenter_utils.hpp"
#include "scorep_instrumenter_install_data.hpp"
#include "scorep_instrumenter.hpp"
#include <scorep_config_tool_backend.h>
#include <scorep_config_tool_mpi.h>

#include <algorithm>

/* ****************************************************************************
   Compiler specific defines
******************************************************************************/

#if SCOREP_BACKEND_COMPILER_CRAY
#define SCOREP_OPARI_MANGLING_SCHEME "cray"
#define SCOREP_ADDITIONAL_OPARI_FORTRAN_FLAGS "--nosrc "

#elif SCOREP_BACKEND_COMPILER_GNU
#define SCOREP_OPARI_MANGLING_SCHEME "gnu"
#define SCOREP_ADDITIONAL_OPARI_FORTRAN_FLAGS

#elif SCOREP_BACKEND_COMPILER_IBM
#define SCOREP_OPARI_MANGLING_SCHEME "ibm"
#define SCOREP_ADDITIONAL_OPARI_FORTRAN_FLAGS

#elif SCOREP_BACKEND_COMPILER_INTEL
#define SCOREP_OPARI_MANGLING_SCHEME "intel"
#define SCOREP_ADDITIONAL_OPARI_FORTRAN_FLAGS

#elif SCOREP_BACKEND_COMPILER_PGI
#define SCOREP_OPARI_MANGLING_SCHEME "pgi"
#define SCOREP_ADDITIONAL_OPARI_FORTRAN_FLAGS

#elif SCOREP_BACKEND_COMPILER_STUDIO
#define SCOREP_OPARI_MANGLING_SCHEME "sun"
#define SCOREP_ADDITIONAL_OPARI_FORTRAN_FLAGS

#endif

/* **************************************************************************************
 * class SCOREP_Instrumenter_OpariAdapter
 * *************************************************************************************/
SCOREP_Instrumenter_OpariAdapter::SCOREP_Instrumenter_OpariAdapter( void )
    : SCOREP_Instrumenter_Adapter( SCOREP_INSTRUMENTER_ADAPTER_OPARI, "opari" )
{
    m_default_on.push_back( SCOREP_INSTRUMENTER_ADAPTER_PREPROCESS );

    m_opari        = OPARI;
    m_opari_script = "`" OPARI_CONFIG " --region-initialization`";
    m_opari_config = OPARI_CONFIG;
    m_c_compiler   = SCOREP_CC;
    m_nm           = "`" OPARI_CONFIG " --nm`";
    m_pomp         = detect;
}

bool
SCOREP_Instrumenter_OpariAdapter::checkOption( std::string arg )
{
    if ( arg == "--pomp" )
    {
        m_pomp = enabled;
        return true;
    }
    if ( arg.substr( 0, 7 ) == "--pomp=" )
    {
        m_pomp    = enabled;
        m_params += " " + arg.substr( 7, std::string::npos );
        return true;
    }
    if ( arg == "--nopomp" )
    {
        m_pomp = disabled;
        return true;
    }
    if ( arg == "--noopari" )
    {
        m_usage = disabled;
        return true;
    }
    if ( arg == "--opari" )
    {
        m_usage = enabled;
        return true;
    }
    if ( arg.substr( 0, 8 ) == "--opari=" )
    {
        m_usage   = enabled;
        m_params += " " + arg.substr( 8, std::string::npos );
        return true;
    }
    return false;
}

bool
SCOREP_Instrumenter_OpariAdapter::isEnabled( void )
{
    return ( m_usage == enabled ) || ( m_pomp == enabled );
}

std::string
SCOREP_Instrumenter_OpariAdapter::getConfigToolFlag( void )
{
    if ( m_pomp == enabled )
    {
        return " --pomp";
    }
    return "";
}

void
SCOREP_Instrumenter_OpariAdapter::printHelp( void )
{
    std::cout << "  --pomp[=<paramter-list>]\n"
              << "                  Enables pomp user instrumentation.\n"
              << "                  You may add additional parameters that are passed to OPARI2.\n"
              << "                  By default, it enables also preprocessing\n";
    std::cout << "  --nopomp        Disables pomp user instrumentation.\n";
    std::cout << "  --opari[=<paramter-list>]\n"
              << "                  Enables OPARI2 instrumentation of OpenMP regions.\n"
              << "                  You may add additional parameters that are passed to OPARI2\n"
              << "                  By default, it enables also preprocessing and\n"
              << "                  pomp user instrumentation."
              << std::endl;
}


std::string
SCOREP_Instrumenter_OpariAdapter::precompile( SCOREP_Instrumenter&         instrumenter,
                                              SCOREP_Instrumenter_CmdLine& cmdLine,
                                              const std::string&           source_file )
{
    /* For Fortran source files, the extension must be in upper case to use the
       C-Preporcessor */
    std::string extension = get_extension( source_file );
    if ( is_fortran_file( source_file ) )
    {
        std::transform( extension.begin(),
                        extension.end(),
                        extension.begin(),
                        ::toupper );
    }

    std::string modified_file = remove_path( remove_extension( source_file )
                                             + ".opari"
                                             + extension );

    invoke_opari( instrumenter, source_file, modified_file );
    instrumenter.addTempFile( modified_file );
    instrumenter.addTempFile( remove_path( source_file + ".opari.inc" ) );
    return modified_file;
}

void
SCOREP_Instrumenter_OpariAdapter::prelink( SCOREP_Instrumenter&         instrumenter,
                                           SCOREP_Instrumenter_CmdLine& cmdLine )
{
    // When linking a shared library, it must not contain a pomp init file
    if ( cmdLine.isTargetSharedLib() )
    {
        return;
    }

    std::string output_name  = cmdLine.getOutputName();
    std::string input_files  = instrumenter.getInputFiles();
    std::string current_file = "";
    std::string object_files = "";
    std::string init_source  = output_name + ".pomp_init.c";
    std::string init_object  = output_name + ".pomp_init.o";
    size_t      old_pos      = 0;
    size_t      cur_pos      = 0;

    // Compile list of library and object files
    while ( cur_pos != std::string::npos )
    {
        cur_pos = input_files.find( " ", old_pos );
        if ( old_pos < cur_pos ) // Discard a blank
        {
            current_file = input_files.substr( old_pos, cur_pos - old_pos );
            if ( is_object_file( current_file ) || is_library( current_file ) )
            {
                object_files += " " + current_file;
            }
        }
        // Setup for next file
        old_pos = cur_pos + 1;
    }
    object_files += cmdLine.getLibraryFiles();

    // Create and compile the POMP2 init file.
    invoke_awk_script( instrumenter, object_files, init_source );
    compile_init_file( instrumenter, init_source, init_object );

    /* Add the object file for POMP2 initialization to the input files for linking.
       Prepend it to the front of input symbols, because some compilers do not
       find the POMP_Init_<ID> symbols (in libraries) if it is appended.
       See  ticket #627. */
    instrumenter.prependInputFile( init_object );
}

void
SCOREP_Instrumenter_OpariAdapter::setBuildCheck( void )
{
    #if !HAVE( EXTERNAL_OPARI2 )
    m_opari_config = simplify_path( BUILD_DIR "/../vendor/opari2/build-frontend/opari2-config" )
                     + " --build-check";
    m_opari = simplify_path( BUILD_DIR "/../vendor/opari2/build-frontend/opari2" );
    #endif
    m_nm           = "`" + m_opari_config +  " --nm`";
    m_opari_script = "`" + m_opari_config + " --region-initialization`";
}

void
SCOREP_Instrumenter_OpariAdapter::setConfigValue( const std::string& key,
                                                  const std::string& value )
{
    if ( key == "OPARI_CONFIG" && value != "" )
    {
        m_nm           = "`" + value + " --nm`";
        m_opari_script = "`" + value + " --region-initialization`";
        m_opari_config = value;
    }
    else if ( key == "OPARI" && value != "" )
    {
        m_opari = value;
    }
}

bool
SCOREP_Instrumenter_OpariAdapter::checkCommand( const std::string& current,
                                                const std::string& next )
{
    std::string arg = ( current == "-f" ? current + next : current );

    if ( SCOREP_Instrumenter_InstallData::isArgForFreeform( arg ) )
    {
        set_fortran_form( true );
        return current == "-f";
    }
    if ( SCOREP_Instrumenter_InstallData::isArgForFixedform( arg ) )
    {
        set_fortran_form( false );
        return current == "-f";
    }
    return false;
}

/* ------------------------------------------------------------------- private methods */
void
SCOREP_Instrumenter_OpariAdapter::invoke_opari( SCOREP_Instrumenter& instrumenter,
                                                const std::string&   input_file,
                                                const std::string&   output_file )
{
    std::string command = m_opari + m_params + " --tpd "
                          SCOREP_ADDITIONAL_OPARI_FORTRAN_FLAGS
                          "--tpd-mangling=" SCOREP_OPARI_MANGLING_SCHEME " ";

    SCOREP_Instrumenter_Adapter* adapter = getAdapter( SCOREP_INSTRUMENTER_ADAPTER_PDT );
    if ( ( adapter != NULL ) && adapter->isEnabled() && is_fortran_file( input_file ) )
    {
        command += "--nosrc ";
    }

    adapter = getAdapter( SCOREP_INSTRUMENTER_ADAPTER_PREPROCESS );
    if ( ( adapter != NULL ) && adapter->isEnabled() )
    {
        command += "--preprocessed ";
    }

    if ( m_pomp == disabled )
    {
        command += "--disable=region ";
    }
    if ( m_usage == disabled )
    {
        command += "--disable=omp ";
    }

    command += input_file + " " + output_file;

    instrumenter.executeCommand( command );
}

void
SCOREP_Instrumenter_OpariAdapter::invoke_awk_script( SCOREP_Instrumenter& instrumenter,
                                                     const std::string&   object_files,
                                                     const std::string&   output_file )
{
    std::string command = m_nm + " " +  object_files
                          + " | " + m_opari_script
                          + " > " + output_file;
    instrumenter.executeCommand( command );

    instrumenter.addTempFile( output_file );
}

void
SCOREP_Instrumenter_OpariAdapter::compile_init_file( SCOREP_Instrumenter& instrumenter,
                                                     const std::string&   input_file,
                                                     const std::string&   output_file )
{
    std::string command = m_c_compiler
                          + " -c " + input_file
                          + " `" + m_opari_config + " --cflags` "
                          + " -o " + output_file;

    instrumenter.executeCommand( command );

    instrumenter.addTempFile( output_file );
}

void
SCOREP_Instrumenter_OpariAdapter::set_fortran_form( bool is_free )
{
#if HAVE( OPARI2_FIX_AND_FREE_FORM_OPTIONS )
    if ( is_free )
    {
        m_params += " --free-form";
    }
    else
    {
        m_params += " --fix-form";
    }
#else
    if ( is_free )
    {
        m_params += " --f90";
    }
    else
    {
        m_params += " --f77";
    }
#endif
}
