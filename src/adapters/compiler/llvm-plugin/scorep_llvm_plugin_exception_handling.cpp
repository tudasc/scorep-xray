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
 * @brief Implementation of the exception handling pass used for the LLVM plugin
 */

#include <config.h>

#include "scorep_llvm_plugin.hpp"
#include "scorep_llvm_plugin_exception_handling.hpp"

#include <llvm/ADT/SetVector.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Transforms/Utils/Local.h>

#if __has_include( "llvm/IR/EHPersonalities.h" )
#include <llvm/IR/EHPersonalities.h>
#else
#include <llvm/Analysis/EHPersonalities.h>
#endif

using namespace llvm;


SCOREP::Compiler::LLVMPlugin::ExceptionHandling::ExceptionHandling()
    : PassInfoMixin<SCOREP::Compiler::LLVMPlugin::ExceptionHandling>()
{
    m_instrumentation_filter = GetInstrumentationFilter();
}

PreservedAnalyses
SCOREP::Compiler::LLVMPlugin::ExceptionHandling::run( Module&                module,
                                                      ModuleAnalysisManager& moduleAnalysisManager )
{
    if ( !ModuleIsInstrumentable( module ) )
    {
        return PreservedAnalyses::all();
    }
    if ( module.getNamedMetadata( "scorep.exception_handling.module_visited" ) )
    {
        VerboseMessage( "[Score-P] Skipping LLVM pass 'SCOREP::Compiler::LLVMPlugin::ExceptionHandling' on module ",
                        module.getName().str(), ". Module was instrumented already." );
        return PreservedAnalyses::all();
    }
    VerboseMessage( "[Score-P] Running LLVM pass 'SCOREP::Compiler::LLVMPlugin::ExceptionHandling' on module ",
                    module.getName().str() );

    m_external.init_prototypes( module );

    for ( auto& function: module )
    {
        add_exception_handling( function );
    }

    module.getOrInsertNamedMetadata( "scorep.exception_handling.module_visited" );
    verifyModule( module );

    return PreservedAnalyses::none();
}


void
SCOREP::Compiler::LLVMPlugin::ExceptionHandling::add_exception_handling( Function& function )
{
    if ( !FunctionIsInstrumentable( function, m_instrumentation_filter ) )
    {
        VerboseMessage( "[Score-P] Filtered function ", DemangleFunctionGetBasename( function.getName().str() ) );
        return;
    }
    VerboseMessage( "[Score-P] Instrumenting function ", DemangleFunctionGetBasename( function.getName().str() ) );
    // Those two are required for proper exception handling.
    auto exception_type = entry_block_add_alloca( function, Type::getInt32Ty( function.getContext() ), nullptr,
                                                  "exception.type" );
    auto exception_ptr = entry_block_add_alloca( function, GetCharPointerType( function.getContext() ), nullptr,
                                                 "exception.slot" );
    // Keep an original copy of our instructions to iterate through
    // This ensures that we do not run into any issues because we add / remove stuff
    SetVector<Instruction*> worklist;
    for ( inst_iterator iterator = inst_begin( function ), end = inst_end( function ); iterator != end; ++iterator )
    {
        worklist.insert( &*iterator );
    }

    bool has_call_instruction = false;
    for ( auto instruction: worklist )
    {
        if ( auto call_instruction = dyn_cast<CallInst>( instruction ) )
        {
            // When compiling with debug, we get functions like `llvm.dbg.*` which we do not want to instrument
            // In fact, instrumenting them will cause issues. Therefore, skip them.
            if ( auto called_function = call_instruction->getCalledFunction() )
            {
                if ( called_function->hasName() && called_function->getName().contains( "llvm." ) )
                {
                    continue;
                }
            }
            auto catch_block = create_landing_pad_cleanup( function, exception_ptr, exception_type );
            changeToInvokeAndSplitBasicBlock( call_instruction, catch_block );
            has_call_instruction = true;
        }
    }

    if ( !has_call_instruction )
    {
        return;
    }

    // Allow the function to unwind the exception stack by removing the nounwind attribute
    if ( function.hasFnAttribute( Attribute::NoUnwind ) )
    {
        function.removeFnAttr( Attribute::NoUnwind );
    }

    // Sets how the exception unwinding is handled.
    if ( !function.hasPersonalityFn() )
    {
        auto pointer_type = GetCharPointerType( function.getContext() );

        function.setPersonalityFn(
            ConstantExpr::getPointerCast( dyn_cast<Constant>( m_external.personality.getCallee() ),
                                          pointer_type ) );
    }
}


void
SCOREP::Compiler::LLVMPlugin::ExceptionHandling::external_definitions::init_prototypes( Module& M )
{
    // GNU C personality function
    personality = M.getOrInsertFunction( getEHPersonalityName( EHPersonality::GNU_C ),
                                         Type::getInt32Ty( M.getContext() ) );
}


AllocaInst*
SCOREP::Compiler::LLVMPlugin::ExceptionHandling::entry_block_add_alloca( Function& F,
                                                                         Type*     type,
                                                                         Constant* init,
                                                                         StringRef name )
{
    auto&       basic_block = F.getEntryBlock();
    IRBuilder<> builder { &basic_block, basic_block.begin() };

    AllocaInst* alloc = builder.CreateAlloca( type, nullptr, name );
    if ( init )
    {
        builder.CreateStore( init, alloc );
    }
    return alloc;
}


BasicBlock*
SCOREP::Compiler::LLVMPlugin::ExceptionHandling::create_landing_pad_cleanup( Function&    F,
                                                                             Instruction* exceptionPtr,
                                                                             Instruction* exceptionType )
{
    // Add a basic landing pad which will be invoked with all exceptions (ptr, i32)
    // This landing pad will not handle the exception directly but will just "cleanup" which allows us to enter this
    // block with the C personality function. We just pass the exception to our resume block for later handling.
    BasicBlock* landing_pad_cleanup = BasicBlock::Create( F.getContext(), "cleanup", &F );
    IRBuilder<> builder { landing_pad_cleanup };
    auto        landing_pad = builder.CreateLandingPad(
        StructType::get( F.getContext(), { GetCharPointerType( F.getContext() ), builder.getInt32Ty() } ), 0, "lp.cleanup" );
    landing_pad->setCleanup( true );
    auto extract0 = builder.CreateExtractValue( landing_pad, 0 );
    builder.CreateStore( extract0, exceptionPtr );
    auto extract1 = builder.CreateExtractValue( landing_pad, 1 );
    builder.CreateStore( extract1, exceptionType );

    builder.CreateBr( create_resume( F, exceptionPtr, exceptionType ) );
    return landing_pad_cleanup;
}

BasicBlock*
SCOREP::Compiler::LLVMPlugin::ExceptionHandling::create_resume( Function&    F,
                                                                Instruction* exceptionPtr,
                                                                Instruction* exceptionType )
{
    // This block is required for proper unwinding of the thrown exception.
    // We use the resume instruction later to add an exit region call.
    BasicBlock* resume_block = BasicBlock::Create( F.getContext(), "resume", &F );
    IRBuilder<> builder( resume_block );
    Value*      exception_ptr_val  = builder.CreateLoad( GetCharPointerType( F.getContext() ), exceptionPtr );
    Value*      exception_type_val = builder.CreateLoad( builder.getInt32Ty(), exceptionType );

    Type* lpad_type = StructType::get( F.getContext(),
                                       { GetCharPointerType( F.getContext() ), builder.getInt32Ty() } );
    Value* lpad_val = UndefValue::get( lpad_type );
    lpad_val = builder.CreateInsertValue( lpad_val, exception_ptr_val, 0, "lpad.val" );
    lpad_val = builder.CreateInsertValue( lpad_val, exception_type_val, 1, "lpad.val" );
    builder.CreateResume( lpad_val );

    return resume_block;
}
