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
 * @brief Header of the instrumentation pass used for the LLVM plugin
 */


#ifndef SCOREP_LLVM_PLUGIN_FUNCTION_INSTRUMENTATION_H
#define SCOREP_LLVM_PLUGIN_FUNCTION_INSTRUMENTATION_H

#include <config.h>

#include <llvm/ADT/SetVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Value.h>

#include <memory>

#include <SCOREP_Types.h>

namespace SCOREP::Compiler::LLVMPlugin
{
struct FunctionInstrumentation
    : llvm::PassInfoMixin<FunctionInstrumentation>
{
public:
    FunctionInstrumentation();

    llvm::PreservedAnalyses
    run( llvm::Module&                module,
         llvm::ModuleAnalysisManager& moduleAnalysisManager );

private:
    void
    insert_register_functions( llvm::Module&                           module,
                               const llvm::SetVector<llvm::Function*>& worklist );

    void
    ir_builder_set_insert( llvm::BasicBlock* basicBlock );

    void
    ir_builder_set_insert( llvm::Instruction* instruction );

    llvm::Value*
    get_region_id( llvm::Function&    function,
                   llvm::Instruction* instruction );

    llvm::Value*
    get_region_desc( llvm::Function&    function,
                     llvm::Instruction* instruction );

    void
    add_register_region( llvm::Function&    function,
                         llvm::Instruction* instruction );

    void
    add_enter_region( llvm::Function&    function,
                      llvm::Instruction* instruction,
                      llvm::Value*       regionId );

    void
    add_exit_region( llvm::Function&    function,
                     llvm::Instruction* instruction,
                     llvm::Value*       regionId );

    void
    instrument( llvm::Function& function );

    struct external_definitions
    {
        // Score-P functions
        llvm::FunctionCallee scorep_plugin_register_region;
        llvm::FunctionCallee scorep_plugin_enter_region;
        llvm::FunctionCallee scorep_plugin_exit_region;
        llvm::FunctionCallee module_register_regions;

        // Score-P types
        llvm::StructType* scorep_region_desc;

        void
        init_prototypes( llvm::Module& module );
    };

    external_definitions                m_external;
    std::string                         m_module_identifier;
    SCOREP_Filter*                      m_instrumentation_filter;
    std::unique_ptr<llvm::IRBuilder<> > m_builder;
};
}


#endif //SCOREP_LLVM_PLUGIN_FUNCTION_INSTRUMENTATION_H
