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


#ifndef SCOREP_LLVM_PLUGIN_H
#define SCOREP_LLVM_PLUGIN_H

#include <config.h>

#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include <string>

#include <SCOREP_Filter.h>

namespace SCOREP::Compiler::LLVMPlugin
{
extern llvm::cl::opt<std::string> FunctionFilter;
extern llvm::cl::opt<int>         Verbosity;

template<class ... Args>
inline void
VerboseMessage( const Args& ... args )
{
    if ( Verbosity > 0 )
    {
        ( llvm::errs() << ... << args );
        llvm::errs() << "\n";
    }
}

std::string
StringToSHA1( const llvm::StringRef& M );

std::string
DemangleFunctionName( const std::string& mangledName );

std::string
DemangleFunctionGetBasename( const std::string& mangledName );

bool
FunctionIsInstrumentable( llvm::Function& F,
                          SCOREP_Filter*  filter );

bool
ModuleIsInstrumentable( const llvm::Module& M );

SCOREP_Filter*
GetInstrumentationFilter();

llvm::Type*
GetCharPointerType( llvm::LLVMContext& context );

llvm::Type*
GetInt32PointerType( llvm::LLVMContext& context );
}
#endif //SCOREP_LLVM_PLUGIN_H
