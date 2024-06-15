/**
 * @file scorep_xry_plugin.cpp
 * @brief Score-P LLVM XRAY instrumentation plugin - runtime management
 */
extern "C" {
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

// Array of region_descriptions for all XRay instrumented functions
static std::vector<scorep_compiler_region_description> regions;

// Array of only the region handles for each region for more cache friendliness
static std::vector<uint32_t> regionHandles;

/**
 * @param execFileName Name of the file to retrieve Function data from, should be full path to current executable
 * @return TODO
 */
void buildRegionsForExecutable(std::string &execFileName) XRAY_INSTRUMENT_NEVER {
    auto maybeMap = llvm::xray::loadInstrumentationMap(execFileName);
    if (auto err = maybeMap.takeError()) {
        UTILS_BUG("Could not read XRay instrumentation map!: %s", toString(std::move(err)).c_str());
    }
    auto funcAddressMap = maybeMap.get().getFunctionAddresses(); // Mapping of XRay fid -> address (unique)
    regions.resize(funcAddressMap.size()); // Resize once so insert is trivial
    llvm::symbolize::LLVMSymbolizer symbolizer({.Demangle = false});
    for (auto mapping: funcAddressMap) {
        int32_t funcId = mapping.first;
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
            regions[funcId-1] = regionInfo;  // XrayIDs start at 1
            // Do not set regionHandles yet as they are still subject to change
#if SCOREP_XRAY_DEBUG
            std::cout << "XRay instrumented: " << funcId << " @" << funcAddr << ": " << "\n\tdemangled: "
            << funcNameDemangled << "\n\tname: " << funcNameMangled << "\n\tlineStart: "
            << maybeFuncInfo.get().StartLine << "\n\tfile: " << sourceFile <<std::endl;
#endif
        }
    }
}


/**
 * @return true on success, false otherwise
 */
bool registerAndPatch() XRAY_INSTRUMENT_NEVER {
    bool successStatus = true;
    regionHandles.resize(regions.size());
    for (int i = 0; i<regions.size(); i++) {
        XRayPatchingStatus status;
        // Register region to init measurement and apply filter rules to region - let score-p do the filter work
        scorep_plugin_register_region(&regions[i]);
        // Registering also updated the region handle to its final value
        uint32_t handle = *regions[i].handle;
        regionHandles[i] = handle;
        // Check if handle corresponds to filtered value or if registering failed
        if ((handle != SCOREP_FILTERED_REGION) && (handle != SCOREP_INVALID_REGION)) {
            status = __xray_patch_function(i+1);  // XrayIDs start at 1
        } else {
            status = __xray_unpatch_function(i+1); // XrayIDs start at 1
        }
        if (status != XRayPatchingStatus::SUCCESS) {
            successStatus = false;
            UTILS_WARNING("Could not (un)patch Xray function sled for xrayId %i: %i", i, status);
        }
    }
    return successStatus;
}

void handleInstrumentationPoint(int32_t fid, XRayEntryType entryType) XRAY_INSTRUMENT_NEVER {
    if (entryType == XRayEntryType::ENTRY) {
        scorep_plugin_enter_region(regionHandles[fid-1]);
    } else if (entryType == XRayEntryType::EXIT) {
        scorep_plugin_exit_region(regionHandles[fid-1]);
    } else if (entryType == XRayEntryType::TAIL) {
        // TODO!: Ignore tail type for now as this will be the function map destruction
    } else {
        UTILS_WARN_ONCE("Unhandled Xray sled event %u for fid %i", entryType, fid);
    }
}

inline bool shouldInitXray() {
    return SCOREP_Env_DoProfiling() || SCOREP_Env_DoTracing() || SCOREP_Env_DoUnwinding();
}

SCOREP_ErrorCode XRayPlugin::initXRay() XRAY_INSTRUMENT_NEVER {
    if (!shouldInitXray()) {
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
    buildRegionsForExecutable(fileName);
    registerAndPatch();

    return SCOREP_ErrorCode::SCOREP_SUCCESS;
}

namespace {
    struct InitXRayPlugin {
        InitXRayPlugin() XRAY_INSTRUMENT_NEVER { XRayPlugin::initXRay(); }
    };

    InitXRayPlugin _;
}

