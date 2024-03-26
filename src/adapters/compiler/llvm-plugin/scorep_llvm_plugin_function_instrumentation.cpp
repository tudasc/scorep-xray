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
 * @brief Implementation of the instrumentation pass used for the LLVM plugin
 */
#include <config.h>

#include "scorep_llvm_plugin.hpp"
#include "scorep_llvm_plugin_function_instrumentation.hpp"

#include <llvm/ADT/SetVector.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include <sstream>

using namespace llvm;


GlobalVariable*
create_global_variable( Module& module, Type* type, StringRef name )
{
    module.getOrInsertGlobal( name, type );
    return module.getNamedGlobal( name );
}

SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::FunctionInstrumentation()
    : PassInfoMixin<SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation>()
{
    m_instrumentation_filter = GetInstrumentationFilter();
}

PreservedAnalyses
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::run( Module& module, ModuleAnalysisManager& moduleAnalysisManager )
{
    if ( !ModuleIsInstrumentable( module ) )
    {
        return PreservedAnalyses::all();
    }
    if ( module.getNamedMetadata( "scorep.instrumentation.module_visited" ) )
    {
        VerboseMessage( "[Score-P] Skipping LLVM pass 'SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation' on module ",
                        module.getName().str(), ". Module was instrumented already." );
        return PreservedAnalyses::all();
    }
    VerboseMessage( "[Score-P] Running LLVM pass 'SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation' on module ",
                    module.getName().str() );

    SetVector<Function*> worklist;
    for ( auto& function: module )
    {
        worklist.insert( &function );
    }

    /* Normally, setting just the moduleIdentifier for hashing is sufficient. However, flang-new invokes the MLIR before
     * we get a chance to run our IR plugin. During that phase, the moduleIdentifier is replaced by FIRModule being
     * used for each compilation unit. This causes name collisions when we try to create a unique function for each
     * compilation unit to register functions. Even using a structural hash is not sufficient, as modules
     * might have the exact same layout, except for function / symbol names. In addition, the function might not be
     * available in LLVM 16 and lower. Therefore, combine the hashed module identifier with the hash of all
     * function names. */
    std::stringstream module_function_names;
    for ( auto& func : module.getFunctionList() )
    {
        module_function_names << func.getName().str();
    }
    m_module_identifier = StringToSHA1( module.getModuleIdentifier() ) + "_" +
                          StringToSHA1( module_function_names.str() );
    m_external.init_prototypes( module );
    insert_register_functions( module, worklist );

    for ( auto function: worklist )
    {
        instrument( *function );
    }

    module.getOrInsertNamedMetadata( "scorep.instrumentation.module_visited" );
    verifyModule( module );

    return PreservedAnalyses::none();
}

void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::insert_register_functions( Module&                     module,
                                                                                  const SetVector<Function*>& worklist )
{
    auto&             context       = module.getContext();
    const std::string function_name = "scorep_llvm_register_regions_" + m_module_identifier;
    VerboseMessage( "Adding function ", function_name, " to module ", module.getModuleIdentifier() );

    auto func = Function::Create( FunctionType::get( Type::getVoidTy( context ), false ), Function::ExternalLinkage,
                                  function_name, module );
    func->addFnAttr( Attribute::NoInline );
    func->addFnAttr( Attribute::NoUnwind );
    auto entry_basic_block = BasicBlock::Create( context, "entry", func );

    ir_builder_set_insert( entry_basic_block );

    for ( auto function: worklist )
    {
        if ( FunctionIsInstrumentable( *function, m_instrumentation_filter ) )
        {
            Constant*    name           = m_builder->CreateGlobalStringPtr( DemangleFunctionName( function->getName().str() ) );
            Constant*    canonical_name = m_builder->CreateGlobalStringPtr( function->getName() );
            Constant*    file           = m_builder->CreateGlobalStringPtr( module.getSourceFileName() );
            unsigned int lno            = 0;
            if ( DISubprogram* subprogram = function->getSubprogram() )
            {
                lno = subprogram->getLine();
            }
            Constant* begin_lno = ConstantInt::get( m_builder->getInt32Ty(), lno );
            Constant* end_lno   = ConstantInt::get( m_builder->getInt32Ty(), 0 );
            Constant* flags     = ConstantInt::get( m_builder->getInt32Ty(), 0 );

            // SCOREP_Region handle for function
            auto region_handle = create_global_variable( module, m_builder->getInt32Ty(),
                                                         "scorep_region." + m_module_identifier + "." +
                                                         StringToSHA1( function->getName().str() ) );
            region_handle->setConstant( false );
            region_handle->setAlignment( Align( 4 ) );
            region_handle->setInitializer( m_builder->getInt32( SCOREP_INVALID_REGION ) );
            region_handle->setLinkage( GlobalValue::ExternalLinkage );
            region_handle->setSection( ".scorep.region.handles" );


            auto int_pointer = GetInt32PointerType( function->getContext() );
            // Create scorep_compiler_region_description from parameters
            std::vector<Constant*> args {
                ConstantExpr::getPointerCast( region_handle, int_pointer ),
                name,
                canonical_name,
                file,
                begin_lno,
                end_lno,
                flags
            };
            auto* region_descriptor = create_global_variable( module, m_external.scorep_region_desc,
                                                              "scorep_desc." + m_module_identifier + "." +
                                                              StringToSHA1( function->getName().str() ) );
            region_descriptor->setConstant( false );
            region_descriptor->setAlignment( Align( 64 ) );
            region_descriptor->setInitializer( ConstantStruct::get( m_external.scorep_region_desc, args ) );
            region_descriptor->setLinkage( GlobalValue::ExternalLinkage );
            region_descriptor->setSection( ".scorep.region.descrs" );

            // Create call to scorep_plugin_register_region with descriptor
            m_builder->CreateCall( m_external.scorep_plugin_register_region, { region_descriptor } );
        }
    }
    m_builder->CreateRetVoid();
    m_external.module_register_regions = func;
}

void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::ir_builder_set_insert( BasicBlock* basicBlock )
{
    if ( !m_builder )
    {
        m_builder = std::make_unique<IRBuilder<> >( basicBlock );
    }
    m_builder->SetInsertPoint( basicBlock );
}

void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::ir_builder_set_insert( Instruction* instruction )
{
    if ( !m_builder )
    {
        m_builder = std::make_unique<IRBuilder<> >( instruction );
    }
    m_builder->SetInsertPoint( instruction );
}

Value*
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::get_region_id( Function& function, Instruction* instruction )
{
    ir_builder_set_insert( instruction );
    auto region_global_variable = function.getParent()->getNamedGlobal(
        "scorep_region." + m_module_identifier + "." + StringToSHA1( function.getName().str() ) );
    auto region_pointer = m_builder->CreateLoad( m_builder->getInt32Ty(), region_global_variable );
    return region_pointer;
}

Value*
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::get_region_desc( Function& function, Instruction* instruction )
{
    ir_builder_set_insert( instruction );
    auto region_global_variable = function.getParent()->getNamedGlobal(
        "scorep_desc." + m_module_identifier + "." + StringToSHA1( function.getName().str() ) );
    auto region_pointer = m_builder->CreateLoad( m_external.scorep_region_desc, region_global_variable );
    return region_pointer;
}

void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::add_register_region( Function& function, Instruction* instruction )
{
    ir_builder_set_insert( instruction );
    auto region_id = get_region_id( function, instruction );
    auto condition = m_builder->CreateICmpEQ( region_id, m_builder->getInt32( SCOREP_INVALID_REGION ) );

    auto then_block = SplitBlockAndInsertIfThen( condition, instruction, false );
    m_builder->SetInsertPoint( then_block );
    m_builder->CreateCall( m_external.module_register_regions );
}

void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::add_enter_region( Function& function, Instruction* instruction, Value* regionId )
{
    ir_builder_set_insert( instruction );
    auto condition = m_builder->CreateICmpNE( regionId, m_builder->getInt32( SCOREP_FILTERED_REGION ) );

    auto ThenBlock = SplitBlockAndInsertIfThen( condition, instruction, false );
    m_builder->SetInsertPoint( ThenBlock );
    m_builder->CreateCall( m_external.scorep_plugin_enter_region, { regionId } );
}

void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::add_exit_region( Function& function, Instruction* instruction, Value* regionId )
{
    ir_builder_set_insert( instruction );
    auto condition = m_builder->CreateICmpNE( regionId, m_builder->getInt32( SCOREP_FILTERED_REGION ) );

    auto then_block = SplitBlockAndInsertIfThen( condition, instruction, false );
    m_builder->SetInsertPoint( then_block );
    m_builder->CreateCall( m_external.scorep_plugin_exit_region, { regionId } );
}

void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::instrument( Function& function )
{
    if ( !FunctionIsInstrumentable( function, m_instrumentation_filter ) )
    {
        VerboseMessage( "[Score-P] Filtered function ", DemangleFunctionGetBasename( function.getName().str() ) );
        return;
    }

    VerboseMessage( "[Score-P] Instrumenting function ", DemangleFunctionGetBasename( function.getName().str() ) );

    // Keep an original copy of our instructions to iterate through
    // This ensures that we do not run into any issues because we add / remove stuff
    SetVector<Instruction*> worklist;
    for ( inst_iterator iterator = inst_begin( function ), end = inst_end( function ); iterator != end; ++iterator )
    {
        worklist.insert( &*iterator );
    }

    // We want to insert to our first instruction of the function.
    Instruction* first = function.getEntryBlock().getFirstNonPHI();
    if ( !first )
    {
        return;
    }

    add_register_region( function, first );
    auto region_id = get_region_id( function, first );
    add_enter_region( function, first, region_id );
    for ( auto instruction: worklist )
    {
        // Exception case. We want to check if the function rethrows an exception. We do not handle throw itself
        // since it is handled in resume instructions
        if ( auto call_instruction = dyn_cast<CallInst>( instruction ) )
        {
            if ( auto called_function = call_instruction->getCalledFunction() )
            {
                if ( called_function->hasName() && called_function->getName().equals( "__cxa_rethrow" ) )
                {
                    add_exit_region( function, instruction, region_id );
                }
            }
        }
        else if ( isa<ReturnInst>( instruction ) || isa<ResumeInst>( instruction ) )
        {
            add_exit_region( function, instruction, region_id );
        }
    }

    verifyFunction( function );
}


void
SCOREP::Compiler::LLVMPlugin::FunctionInstrumentation::external_definitions::init_prototypes( Module& module )
{
    auto& context      = module.getContext();
    auto  int32_type   = Type::getInt32Ty( context );
    auto  char_pointer = GetCharPointerType( context );
    auto  int_pointer  = GetInt32PointerType( context );

    // typedef struct
    // {
    //     uint32_t*   handle;
    //     const char* name;
    //     const char* canonical_name;
    //     const char* file;
    //     int         begin_lno;
    //     int         end_lno;
    //     unsigned    flags;
    // }
    // __attribute__( ( aligned( 64 ) ) )
    // scorep_compiler_region_description
    scorep_region_desc = StructType::create( module.getContext(),
                                             { int_pointer, char_pointer, char_pointer, char_pointer, int32_type, int32_type,
                                               int32_type }, "scorep_compiler_region_description" );
    auto scorep_plugin_register_region_type = FunctionType::get( Type::getVoidTy( context ),
                                                                 PointerType::get( scorep_region_desc, 0 ), false );
    // See scorep_compiler_event_plugin.inc.c for implementation
    scorep_plugin_register_region = module.getOrInsertFunction( "scorep_plugin_register_region",
                                                                scorep_plugin_register_region_type );

    // See scorep_compiler_event_plugin.inc.c for implementation
    auto scorep_plugin_region_type = FunctionType::get( Type::getVoidTy( context ), Type::getInt32Ty( context ), false );
    scorep_plugin_enter_region = module.getOrInsertFunction( "scorep_plugin_enter_region",
                                                             scorep_plugin_region_type );
    scorep_plugin_exit_region = module.getOrInsertFunction( "scorep_plugin_exit_region",
                                                            scorep_plugin_region_type );
}
