/**
 * @file scorep_xry_plugin.cpp
 * @brief Score-P LLVM XRAY instrumentation plugin - runtime management
 */
extern "C"{
#include <config.h>
#include "SCOREP_Environment.h"
#include "SCOREP_RuntimeManagement.h"
#include "SCOREP_Filter.h"
#include "UTILS_Error.h"
}
#include "scorep_xray_plugin.hpp"
#include <iostream>

// No need to check whether XRAY runtime is available since this header is only included when xray plugin
// instrumentation is enabled, which means compiling and linking with -fxray-instrumented
#include <xray/xray_log_interface.h>
#include <xray/xray_interface.h>
#include <xray/xray_records.h>

#include "llvm/Demangle/Demangle.h"
#include "llvm/DebugInfo/Symbolize/Symbolize.h"
#include "SCOREP_Types.h"
#include <llvm/XRay/InstrumentationMap.h>

// TODO!: XRayFuncMetadata is obsolete by now, region info mapping suffices, remove
// TODO!: Memory management for region infos
// TODO!: Check structures, array instead of map?
static std::unordered_map<int32_t, XRayPlugin::XRayFuncMetadata> functionMap;

/**
 * Builds a map XRayFuncId -> XrayIFMetadata by first retrieving sled info from XRay, filtering to unique fIDs
 * (Entry/exit sleds not needed), symbolizing and then demangling the found addresses
 * @param execFileName Name of the file to retrieve Function data from, should be full path to current executable
 * @return Map XrayFuncID -> Metadata about that function
 */
std::unordered_map<int32_t, XRayPlugin::XRayFuncMetadata>
buildFunctionMap(std::string &execFileName) XRAY_INSTRUMENT_NEVER {
    std::unordered_map<int, XRayPlugin::XRayFuncMetadata> idMap;
    auto maybeMap = llvm::xray::loadInstrumentationMap(execFileName);
    if (auto err = maybeMap.takeError()) {
        // TODO!: Check whether aborting program would be better
        UTILS_ERROR(SCOREP_ErrorCode::SCOREP_ERROR_XRAY_INIT, "Could not read XRay instrumentation map!: %s",
                    toString(std::move(err)).c_str());
        return idMap;
    }
    auto funcAddressMap = maybeMap.get().getFunctionAddresses();
    llvm::symbolize::LLVMSymbolizer symbolizer({.Demangle = false});
    for (auto mapping: funcAddressMap) {
        int32_t funcId = mapping.first;
        if (idMap.find(funcId) == idMap.end()) {
            // This funcID is new
            uint64_t funcAddr = mapping.second;
            llvm::object::SectionedAddress sectAddress{funcAddr}; // init Address but keep SectionIndex default
            auto maybeFuncInfo = symbolizer.symbolizeCode(execFileName, sectAddress);
            if (auto err = maybeFuncInfo.takeError()) {
                UTILS_BUG("Could not get symbol for XRay instrumented function %i @addr: %lu: %s",
                                funcId, funcAddr, toString(std::move(err)).c_str());
            } else {
                std::string funcNameMangled = maybeFuncInfo.get().FunctionName;
                std::string funcNameDemangled = llvm::demangle(funcNameMangled);
                // Path needn't be cleaned as it is convention to provide filenames with "*/"
                std::string sourceFile = maybeFuncInfo.get().FileName; // "Source" is unreliable, use FileName
                auto regionInfo = XRayPlugin::createRegionDesc(funcNameMangled, funcNameDemangled,
                                                               sourceFile, maybeFuncInfo.get().StartLine,
                                                               maybeFuncInfo.get().Line);
                idMap.emplace(funcId, XRayPlugin::XRayFuncMetadata(funcId, funcAddr, regionInfo));
#if SCOREP_XRAY_DEBUG
                std::cout << "XRay instrumented: " << funcId << " @" << funcAddr << ": " << "\n\tdemangled: "
                << funcNameDemangled << "\n\tname: " << funcNameMangled << "\n\tlineStart: "
                << maybeFuncInfo.get().StartLine << "\n\tfile: " << sourceFile <<std::endl;
#endif
            }
        }
    }
    return idMap;
}


/**
 * Checks whether a XRay instrumented function's sleds should be patched or unpatched
 * This is done by checking if the functions region description was marked as filtered by the score-p runtime
 * The region must have been registered beforehand or the handle will not be set correctly
 * @param funcMetadata Metadata of scorep-p REGISTERED xray-instrumented function
 * @return true if functions sleds should be patched, false if runtime filtered this function out (unpatch)
 */
inline bool shouldPatchFunction(XRayPlugin::XRayFuncMetadata &funcMetadata) XRAY_INSTRUMENT_NEVER {
    uint32_t handle = *funcMetadata.regionDescription->handle;
    return (handle != SCOREP_FILTERED_REGION) && (handle != SCOREP_INVALID_REGION);
}


/**
 * @return true on success, false otherwise
 */
bool registerAndPatch() XRAY_INSTRUMENT_NEVER {
    bool successStatus = true;
    for (const auto &entry: functionMap) {
        XRayPatchingStatus status;
        auto funcMetadata = entry.second;
        // Register region to init measurement and apply filter rules to region - let score-p do the filter work
        scorep_plugin_register_region(funcMetadata.regionDescription);
        if (shouldPatchFunction(funcMetadata)) {
            //std::cout << "Patching function " << funcMetadata.xRayFuncId << std::endl; //TODO!:
            status = __xray_patch_function(entry.first);
        } else {
            //std::cout << "UNPatching function " << funcMetadata.xRayFuncId << std::endl; //TODO!:
            status = __xray_unpatch_function(entry.first);
        }
        if (status != XRayPatchingStatus::SUCCESS) {
            successStatus = false;
            UTILS_WARNING("Could not (un)patch Xray function sled for xrayId %i: %i", entry.first, status);
        }
    }
    return successStatus;
}

void handleInstrumentationPoint(int32_t fid, XRayEntryType entryType) XRAY_INSTRUMENT_NEVER {
    if (entryType == XRayEntryType::ENTRY) {
        scorep_plugin_enter_region(*functionMap.at(fid).regionDescription->handle);
    } else if (entryType == XRayEntryType::EXIT) {
        scorep_plugin_exit_region(*functionMap.at(fid).regionDescription->handle);
    } else if (entryType == XRayEntryType::TAIL) {
        // TODO!: Ignore tail type for now as this will be the function map destruction
    } else {
        UTILS_WARN_ONCE("Unhandled Xray sled event %u for fid %i", entryType, fid);
    }
}

inline bool shouldInitXray(){
    return SCOREP_Env_DoProfiling() || SCOREP_Env_DoTracing() || SCOREP_Env_DoUnwinding();
}

SCOREP_ErrorCode XRayPlugin::initXRay() XRAY_INSTRUMENT_NEVER {
    if(!shouldInitXray()){
        return SCOREP_ErrorCode::SCOREP_SUCCESS;
    }
    __xray_init(); // Safe even if it is already initialized
    bool success = __xray_set_handler(&handleInstrumentationPoint);
    if (!success) {
        // TODO!: Handle 'spurious calls' as per docs?
        UTILS_ERROR(SCOREP_ERROR_XRAY_INIT, "Could not set XRay handler function!");
        return SCOREP_ErrorCode::SCOREP_ERROR_XRAY_INIT;
    }
    bool execNameIsFile;
    std::string fileName = SCOREP_GetExecutableName(&execNameIsFile);
    functionMap = buildFunctionMap(fileName);
    registerAndPatch();

    return SCOREP_ErrorCode::SCOREP_SUCCESS;
}

namespace {
    struct InitXRayPlugin {
        InitXRayPlugin() XRAY_INSTRUMENT_NEVER { XRayPlugin::initXRay(); }
    };

    InitXRayPlugin _;
}

