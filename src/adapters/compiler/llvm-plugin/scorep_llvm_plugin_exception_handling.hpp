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
 * @brief Header of the exception handling pass used for the LLVM plugin
 */


#ifndef SCOREP_LLVM_PLUGIN_EXCEPTION_HANDLING_H
#define SCOREP_LLVM_PLUGIN_EXCEPTION_HANDLING_H

#include <config.h>

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Instructions.h>

#include <SCOREP_Types.h>

namespace SCOREP::Compiler::LLVMPlugin
{
/**
 * @class ExceptionHandling
 * @brief Responsible for adding exception handling at the beginning of the optimization pass
 *
 * The ExceptionHandling class is an LLVM pass which adds C-style exception handling to function calls.
 * This allows Score-P to exit functions correctly even when an exception is thrown to the user.
 * The added exception handling uses the GCC C personality function. For this, a cleanup landing pad has to be added.
 * This landing pad then branches into a resume block which tells the runtime to pass the exception up the stack. We
 * use the resume block to exit the region later during function instrumentation.
 */
struct ExceptionHandling : llvm::PassInfoMixin<ExceptionHandling>
{
public:
    ExceptionHandling();

    llvm::PreservedAnalyses
    run( llvm::Module&                module,
         llvm::ModuleAnalysisManager& moduleAnalysisManager );

private:
    static llvm::AllocaInst*
    entry_block_add_alloca( llvm::Function& F,
                            llvm::Type*     type,
                            llvm::Constant* init = nullptr,
                            llvm::StringRef name = "" );

    llvm::BasicBlock*
    create_landing_pad_cleanup( llvm::Function&    F,
                                llvm::Instruction* exceptionPtr,
                                llvm::Instruction* exceptionType );

    static llvm::BasicBlock*
    create_resume( llvm::Function&    F,
                   llvm::Instruction* exceptionPtr,
                   llvm::Instruction* exceptionType );

    void
    add_exception_handling( llvm::Function& function );

    struct external_definitions
    {
        // GNU C Exception handling
        llvm::FunctionCallee personality;

        void
        init_prototypes( llvm::Module& M );
    };

    external_definitions m_external;
    SCOREP_Filter*       m_instrumentation_filter;
};
}

#endif //SCOREP_LLVM_PLUGIN_EXCEPTION_HANDLING_H
