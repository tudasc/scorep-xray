/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2023-2024,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Common logic used for LLVM plugin.
 */

/* LLVM uses PIC in an internal header for a PassInstrumentationCallbacks object.
 * If PIC is defined elsewhere, compilation will abort with an error. Therefore,
 * remove the defined PIC if it exists. */
#ifdef PIC
#undef PIC
#endif

#include <config.h>

#include "scorep_llvm_plugin.hpp"
#include "scorep_llvm_plugin_function_instrumentation.hpp"
#include "scorep_llvm_plugin_exception_handling.hpp"

#include <llvm/ADT/StringSwitch.h>
#include <llvm/Support/SHA1.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

#if HAVE( LLVM_DEMANGLE )
#include <llvm/Demangle/Demangle.h>
#endif

#include <sstream>
#include <iomanip>

#include <fnmatch.h>

using namespace llvm;

// General Score-P arguments
// Filter file for compile time filtering
cl::list<std::string> SCOREP::Compiler::LLVMPlugin::FunctionFilter( "scorep-plugin-config-filter-file", cl::desc( "Compile time filter file" ),
                                                                    cl::ZeroOrMore );
// Verbosity
cl::opt<int> SCOREP::Compiler::LLVMPlugin::Verbosity( "scorep-plugin-config-verbosity", cl::init( 0 ),
                                                      cl::desc( "Compile time verbosity" ) );

// Specific arguments which can be set by the user via `scorep --llvm-plugin-arg=`
cl::opt<bool>SCOREP::Compiler::LLVMPlugin::EnableExceptionHandling( "scorep-plugin-exception-handling", cl::init( true ),
                                                                    cl::desc( "Enable exception handling for functions to preserve functional profile on thrown exceptions. "
                                                                              "Will introduce additional overhead." ) );

std::string
SCOREP::Compiler::LLVMPlugin::StringToSHA1( const StringRef& string )
{
    auto sha1 = SHA1();
    sha1.init();
    sha1.update( string );
    auto hash = sha1.result();

    std::stringstream ss;
    ss << std::hex << std::setfill( '0' );
    for ( const auto& element: hash )
    {
        ss << std::setw( 2 ) << static_cast<unsigned>( element );
    }

    return ss.str();
}

std::string
SCOREP::Compiler::LLVMPlugin::DemangleFunctionName( const std::string& mangledName )
{
    if ( mangledName.empty() )
    {
        return "";
    }
    #if HAVE( LLVM_DEMANGLE )
    return demangle( mangledName );
    #else
    return mangledName;
    #endif
}

std::string
SCOREP::Compiler::LLVMPlugin::DemangleFunctionGetBasename( const std::string& mangledName )
{
    if ( mangledName.empty() )
    {
        return "";
    }
    #if HAVE( LLVM_DEMANGLE )
    ItaniumPartialDemangler demangler;
    if ( !demangler.partialDemangle( mangledName.c_str() ) )
    {
        const char* function_name = demangler.getFunctionName( nullptr, nullptr );
        if ( !function_name )
        {
            return mangledName;
        }
        return function_name;
    }
    return demangle( mangledName );
    #else
    return mangledName;
    #endif
}

bool
SCOREP::Compiler::LLVMPlugin::FunctionIsInstrumentable( Function&      F,
                                                        SCOREP_Filter* filter )
{
    if ( F.empty() | !F.hasName() | F.isDeclaration() )
    {
        return false;
    }

    // First. check for user functions
    auto basename             = DemangleFunctionGetBasename( F.getName().str() );
    int  function_is_filtered = 0;
    SCOREP_Filter_MatchFunction( filter, basename.c_str(), F.getName().str().c_str(), &function_is_filtered );
    if ( bool( function_is_filtered ) )
    {
        return false;
    }
    // Check against user filtered files
    int file_is_filtered = 0;
    SCOREP_Filter_MatchFile( filter, F.getParent()->getSourceFileName().c_str(), &file_is_filtered );
    if ( bool( file_is_filtered ) )
    {
        return false;
    }

    // Check against function names we NEVER want to instrument
    return
        ( strncmp( basename.c_str(), "POMP", 4 ) != 0 )
        && ( strncmp( basename.c_str(), "Pomp", 4 ) != 0 )
        && ( strncmp( basename.c_str(), "pomp", 4 ) != 0 )
        && ( strncmp( basename.c_str(), "SCOREP_", 7 ) != 0 )
        && ( strncmp( basename.c_str(), "scorep_", 7 ) != 0 )
        && ( strncmp( basename.c_str(), "OTF2_", 5 ) != 0 )
        && ( strncmp( basename.c_str(), "otf2_", 5 ) != 0 )
        && ( strncmp( basename.c_str(), "cube_", 5 ) != 0 )
        && ( strncmp( basename.c_str(), "cubew_", 6 ) != 0 )
        && ( strncmp( basename.c_str(), ".omp", 4 ) != 0 )
        && ( !strstr( basename.c_str(), "DIR.OMP." ) )
        && ( !strstr( basename.c_str(), ".extracted" ) )
        #if HAVE( SCOREP_COMPILER_CC_INTEL_ONEAPI ) || HAVE( SCOREP_COMPILER_CXX_INTEL_ONEAPI ) || HAVE( SCOREP_COMPILER_FC_INTEL_ONEAPI )
        && ( !strstr( basename.c_str(), "_tree_reduce_" ) )
        #endif  /* Intel oneAPI compiler */
        && ( strncmp( basename.c_str(), "..omp", 5 ) != 0 )
        && ( strncmp( basename.c_str(), "__omp", 5 ) != 0 )
        && ( strncmp( basename.c_str(), "..acc", 5 ) != 0 )
        && ( strncmp( basename.c_str(), "virtual thunk", 13 ) != 0 )
        && ( strncmp( basename.c_str(), "non-virtual thunk", 17 ) != 0 )
        && ( !strstr( basename.c_str(), "Kokkos::Tools" ) )
        && ( !strstr( basename.c_str(), "Kokkos::Profiling" ) )
        && ( !strstr( basename.c_str(), "6Kokkos5Tools" ) )
        && ( !strstr( basename.c_str(), "6Kokkos9Profiling" ) )

        // Newly added filters needed for LLVM plugin functions
        // Basic C++
        && ( !strstr( basename.c_str(), "_GLOBAL__" ) )
        && ( !strstr( basename.c_str(), "__gnu_cxx::" ) )
        && ( !strstr( basename.c_str(), "__cxx_" ) )
        && ( !strstr( basename.c_str(), "std::" ) )
        // OpenMP
        && ( fnmatch( "__clang_*", basename.c_str(), 0 ) != 0 )
        && ( fnmatch( "omp*$omp*$*", basename.c_str(), 0 ) != 0 )
        && ( fnmatch( "*.omp_outlined*", F.getName().str().c_str(), 0 ) != 0 )
        && ( fnmatch( "*.omp_outlined_debug__*", F.getName().str().c_str(), 0 ) != 0 )
        && ( !strstr( basename.c_str(), "_omp_" ) )
        && ( !strstr( basename.c_str(), "_omp$" ) )
        && ( !strstr( basename.c_str(), "__kmpc" ) )
        && ( strncmp( basename.c_str(), "ompx::", 6 ) != 0 )
        && ( strncmp( basename.c_str(), "__keep_alive", 10 ) != 0 )
        && ( strncmp( basename.c_str(), "__assert_fail", 13 ) != 0 )
        && ( !strstr( basename.c_str(), "__internal_trig_" ) )
        && ( !strstr( basename.c_str(), "thread-local wrapper routine" ) )
        && ( !strstr( basename.c_str(), "(anonymous namespace)::invokeMicrotask" ) )
        && ( strncmp( basename.c_str(), ".red_init.", 10 ) != 0 )
        && ( strncmp( basename.c_str(), ".red_comb.", 10 ) != 0 )
        // MPI
        && ( strncmp( basename.c_str(), "MPI::", 5 ) != 0 )
        // CUDA
        && ( !strstr( basename.c_str(), "__device_stub__" ) )
        && ( fnmatch( "__cuda*", basename.c_str(), 0 ) != 0 )
        && ( fnmatch( "_*_nv_*", basename.c_str(), 0 ) != 0 )
        && ( fnmatch( "__sti_*cuda*", basename.c_str(), 0 ) != 0 )
        // HIP
        && ( fnmatch( "__hip*", basename.c_str(), 0 ) != 0 );
}

bool
SCOREP::Compiler::LLVMPlugin::ModuleIsInstrumentable( const Module& M )
{
    if ( M.empty() )
    {
        VerboseMessage( "[Score-P] Skipping LLVM pass on module ", M.getName().str(), ". Is empty." );
        return false;
    }
    // This is the case for CUDA code which gets compiled by LLVM in the second pass round via pxtas
    SmallVector<StringRef, 4> Components;
    StringRef( M.getTargetTriple() ).split( Components, '-', /*MaxSplit*/ 3 );
    auto instrumentable_arch = StringSwitch<bool>( Components[ 0 ] )
                               .Case( "amdgcn", false )
                               .Case( "nvptx", false )
                               .Case( "nvptx64", false )
                               .Case( "spir64", false )
                               .Cases( "spirv32", "spirv32v1.0", "spirv32v1.1", "spirv32v1.2",
                                       "spirv32v1.3", "spirv32v1.4", "spirv32v1.5", false )
                               .Cases( "spirv64", "spirv64v1.0", "spirv64v1.1", "spirv64v1.2",
                                       "spirv64v1.3", "spirv64v1.4", "spirv64v1.5", false )
                               .Default( true );
    if ( !instrumentable_arch )
    {
        VerboseMessage( "[Score-P] Skipping LLVM pass on module ", M.getName().str().c_str(),
                        ". Detected non host compilation target." );
        return false;
    }

    return true;
}

SCOREP_Filter*
SCOREP::Compiler::LLVMPlugin::GetInstrumentationFilter()
{
    static SCOREP_Filter* instrumentation_filter = nullptr;

    if ( instrumentation_filter )
    {
        return instrumentation_filter;
    }
    if ( FunctionFilter.empty() )
    {
        return nullptr;
    }

    instrumentation_filter = SCOREP_Filter_New();
    if ( !instrumentation_filter )
    {
        report_fatal_error( "Could not allocate memory for instrumentation filter.", false );
    }
    for ( auto& filter_file : FunctionFilter )
    {
        VerboseMessage( "[Score-P] Initialize compile time filter with file",
                        filter_file.c_str() );

        SCOREP_ErrorCode result = SCOREP_Filter_ParseFile( instrumentation_filter,
                                                           filter_file.c_str() );
        if ( result != SCOREP_SUCCESS )
        {
            report_fatal_error( "Could not parse instrumentation filter file.", false );
        }
    }

    return instrumentation_filter;
}


llvm::Type*
SCOREP::Compiler::LLVMPlugin::GetCharPointerType( llvm::LLVMContext& context )
{
    #if LLVM_VERSION_MAJOR > 17
    auto pointer_type = PointerType::get( context, 0 );
    #else
    auto pointer_type = Type::getInt8PtrTy( context, 0 );
    #endif

    return pointer_type;
}


llvm::Type*
SCOREP::Compiler::LLVMPlugin::GetInt32PointerType( llvm::LLVMContext& context )
{
    #if LLVM_VERSION_MAJOR > 17
    auto int_pointer = PointerType::get( context, 0 );
    #else
    auto int_pointer = Type::getInt32PtrTy( context );
    #endif

    return int_pointer;
}


/* LLVM passes should at least offer one of the following methods for pass registration
 * - llvm::PassPluginLibraryInfo get##Name##PluginInfo();
 * - extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() LLVM_ATTRIBUTE_WEAK;
 * Just to be safe, we register both ways */
extern "C" llvm::PassPluginLibraryInfo
getScorePInstrumentationPluginInfo()
{
    return { LLVM_PLUGIN_API_VERSION,
             "Score-P LLVM Instrumentation",
             LLVM_VERSION_STRING,
             [ ]( PassBuilder& PB ) {
                 // LLVM 13.0 and older have OptimizationLevel inside the PassBuilder class.
                 // Newer versions use a separate header for it.
                 #if not __has_include( "llvm/Passes/OptimizationLevel.h" )
                 using OptimizationLevel = PassBuilder::OptimizationLevel;
                 #endif
                 if ( SCOREP::Compiler::LLVMPlugin::EnableExceptionHandling )
                 {
                     PB.registerPipelineStartEPCallback(
                         [ ]( llvm::ModulePassManager& PM, OptimizationLevel level ) -> void {
                    PM.addPass( SCOREP::Compiler::LLVMPlugin::ExceptionHandling() );
                } );
                 }
                 PB.registerOptimizerLastEPCallback(
                     [ ]( llvm::ModulePassManager& PM, OptimizationLevel level ) -> void {
                PM.addPass( SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation() );
            } );
             } };
}


extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return getScorePInstrumentationPluginInfo();
}
