/**
 * @file scorep_xry_plugin.cpp
 * @brief Score-P LLVM XRAY instrumentation plugin - runtime management
 */
extern "C" {
#include "SCOREP_Environment.h"
#include "SCOREP_RuntimeManagement.h"
#include "SCOREP_Filter.h"
#include "UTILS_Error.h"
#include <config-xray-plugin.h>
#include "scorep_compiler_plugin.h"
}

#include "scorep_xray_plugin.h"
#include <iostream>

// No need to check whether XRAY runtime is available since this header is only included when xray plugin
// instrumentation is enabled, which means compiling and linking with -fxray-instrumented
#include <xray/xray_interface.h>

#include "llvm/Demangle/Demangle.h"
#include "llvm/DebugInfo/Symbolize/Symbolize.h"
#include "SCOREP_Types.h"
#include <llvm/XRay/InstrumentationMap.h>

namespace XRayPlugin {

    // Array of region_descriptions for all XRay instrumented functions
    static std::vector<scorep_compiler_region_description> *regions;

    // Array of only the region handles for each region for more cache friendliness
    static std::vector<uint32_t> *regionHandles;

    /**
     * Creates a new, trivially copy-able scorep region description on the heap that can be referenced after passed
     * values go out of scope. Make sure to free contents once it is no longer needed.
     */
    scorep_compiler_region_description
    createRegionDesc(std::string &funcNameMangled, std::string &funcNameDemangled, std::string &sourceFile,
                     const uint32_t startLine, const uint32_t endLine) {
        // Copy strings to heap as they are not available once out of scope, but region info needs pointers to them
        auto nameMangled = strdup(funcNameMangled.c_str());
        auto nameDemangled = strdup(funcNameDemangled.c_str());
        auto file = strdup(sourceFile.c_str());
        // Handle is modified during lifetime, so it too must be persistent somewhere, but also requires pointer
        // and per-region access. Therefore, push it onto heap per created region info.
        auto heapHandle = new uint32_t(SCOREP_INVALID_REGION);
        return scorep_compiler_region_description{
                heapHandle, //region is reset in register call, init with unknown region
                nameDemangled, nameMangled, file, static_cast<int>(startLine), static_cast<int>(endLine), 0};
    }

    /**
     * Reads XRay instrumentation map from the executable, demangles names and builds Score-P region infos for it
     * May end execution of program via call to UTILS_FATAL
     * @param execFileName Name of the file to retrieve Function data from, should be full path to current executable
     * @return true if successful, false if an error occurred but execution can continue
     */
    static bool buildRegionsForExecutable(std::string &execFileName) XRAY_INSTRUMENT_NEVER {
        auto maybeMap = llvm::xray::loadInstrumentationMap(execFileName);
        if (auto err = maybeMap.takeError()) {
            std::string errString = toString(std::move(err));
            if (errString == "Failed to find XRay instrumentation map.") {
                // Assume this is because no function was instrumented (compile time filter) and therefore not an error
                UTILS_WARN_ONCE((errString + " Assuming this is because no function was instrumented during "
                                             "compilation and continuing...").c_str());
                return false;
            } else {
                // Other error messages should be treated as non-recoverable in the context of measurement
                UTILS_FATAL("Could not read XRay instrumentation map!: %s", errString.c_str());
            }
        }
        auto funcAddressMap = maybeMap.get().getFunctionAddresses(); // Mapping of XRay fid -> address (unique)
        regions = new std::vector<scorep_compiler_region_description>(
                funcAddressMap.size()); // Resize once so insert is trivial
        llvm::symbolize::LLVMSymbolizer symbolizer({.Demangle = false});
        for (auto mapping: funcAddressMap) {
            int32_t funcId = mapping.first;
            uint64_t funcAddr = mapping.second;
            llvm::object::SectionedAddress sectAddress{funcAddr}; // init Address but keep SectionIndex default
            auto maybeFuncInfo = symbolizer.symbolizeCode(execFileName, sectAddress);
            if (auto err = maybeFuncInfo.takeError()) {
                UTILS_BUG("Could not get symbol for XRay instrumented function %i @addr: %lu: %s", funcId, funcAddr,
                          toString(std::move(err)).c_str());
            } else {
                std::string funcNameMangled = maybeFuncInfo.get().FunctionName;
                std::string funcNameDemangled = llvm::demangle(funcNameMangled);
                // Path needn't be cleaned as it is convention to provide filenames with "*/"
                // "Source" is unreliable, use FileName (might still be <invalid>)
                std::string sourceFile = maybeFuncInfo.get().FileName;
                auto regionInfo = createRegionDesc(funcNameMangled, funcNameDemangled, sourceFile,
                                                   maybeFuncInfo.get().StartLine, maybeFuncInfo.get().Line);
                (*regions)[funcId - 1] = regionInfo;  // XrayIDs start at 1
                // Do not set regionHandles yet as they are still subject to change
                if (SCOREP_Env_RunVerbose()) {
                    std::cout << "XRay instrumented: " << funcId << " @" << funcAddr << ": " << "\n\tname: "
                              << funcNameMangled << "\n\tdemangled: " << funcNameDemangled << "\n\tlineStart: "
                              << maybeFuncInfo.get().StartLine << "\n\tfile: " << sourceFile << std::endl;
                }
            }
        }
        return true;
    }


    /**
     * Registers every created region info with the measurement runtime and patches/unpatches sleds depending on whether
     * a function was filtered at runtime or not
    * @return true on success, false otherwise
    */
    static bool registerAndPatch() XRAY_INSTRUMENT_NEVER {
        bool successStatus = true;
        regionHandles = new std::vector<uint32_t>((*regions).size());
        for (int i = 0; i < (*regions).size(); i++) {
            XRayPatchingStatus status;
            // Register region to init measurement and apply filter rules to region - let score-p do the filter work
            scorep_compiler_plugin_register_region(&(*regions)[i]);
            // Registering also updated the region handle to its final value, they can now be cached
            uint32_t handle = *(*regions)[i].handle;
            (*regionHandles)[i] = handle;
            // Check if handle corresponds to filtered value or if registering failed
            bool shouldPatch = (handle != SCOREP_FILTERED_REGION) && (handle != SCOREP_INVALID_REGION);
            if (shouldPatch) {
                status = __xray_patch_function(i + 1);  // XrayIDs start at 1
            } else {
                status = __xray_unpatch_function(i + 1); // XrayIDs start at 1
            }
            if(SCOREP_Env_RunVerbose()){
                std::cout << "XRay fid " << i+1 << " was " << (shouldPatch ? "patched" : "unpatched") << std::endl;
            }
            if (status != XRayPatchingStatus::SUCCESS) {
                successStatus = false;
                UTILS_WARNING("Could not (un)patch Xray function sled for xrayId %i: %i", i, status);
            }
        }
        return successStatus;
    }

    /**
     * Handler for patched XRay sleds. When called by XRay, it calls the measurement code with the corresponding
     * region handle to measure the region
     * @param fid XRay id of function
     * @param entryType Type of sled
     */
    static void handleInstrumentationPoint(int32_t fid, XRayEntryType entryType) XRAY_INSTRUMENT_NEVER {
        switch (entryType) {
            case XRayEntryType::ENTRY:
                scorep_plugin_enter_region((*regionHandles)[fid - 1]);
                break;
            case XRayEntryType::TAIL:
            case XRayEntryType::EXIT:
                scorep_plugin_exit_region((*regionHandles)[fid - 1]);
                break;
            default:
                UTILS_WARN_ONCE("Unhandled Xray sled event %u for fid %i", entryType, fid);
        }
    }

    /**
     * @return true if measurement will be active. False if xray needn't be setup
     */
    static inline bool shouldInitXray() XRAY_INSTRUMENT_NEVER {
        return SCOREP_Env_DoProfiling() || SCOREP_Env_DoTracing() || SCOREP_Env_DoUnwinding();
    }

    /**
     * Initializes XRay Plugin runtime by making all necessary calls to XRay and Score-P
     * If successful, the application is patched according to runtime & instrumentation filters and ready to run
     * @return SUCCESS if successful, SCOREP_ERROR_XRAY_INIT if not
     */
    static SCOREP_ErrorCode initXRay() XRAY_INSTRUMENT_NEVER {
        if (!shouldInitXray()) {
            return SCOREP_ErrorCode::SCOREP_SUCCESS;
        }
        __xray_init(); // Safe even if it is already initialized
        bool execNameIsFile;
        std::string fileName = SCOREP_GetExecutableName(&execNameIsFile);
        bool regionsAvailable = buildRegionsForExecutable(fileName);
        if (regionsAvailable) {
            // XRay will throw errors if no function was actually instrumented
            int xrayInitSuccess = __xray_set_handler(&handleInstrumentationPoint);
            if (!xrayInitSuccess) {
                // TODO!: Handle 'spurious calls' as per docs?
                UTILS_ERROR(SCOREP_ERROR_XRAY_INIT, "Could not set XRay handler function!");
                return SCOREP_ErrorCode::SCOREP_ERROR_XRAY_INIT;
            }
            registerAndPatch();
        }
        return SCOREP_ErrorCode::SCOREP_SUCCESS;
    }


    /**
     * Free all region descriptions and optionally unpatch sleds
     * @param unpatch Whether to unpatch all sleds for a clean state
     */
    static void cleanupXRay() XRAY_INSTRUMENT_NEVER {
        if (regions) {
            for (auto region: (*regions)) {
                free((void *) region.name);
                free((void *) region.file);
                free((void *) region.canonical_name);
                delete region.handle;
            }
            (*regions).clear();
            delete regions;
        }
        if (regionHandles) {
            (*regionHandles).clear();
            delete regionHandles;
        }
    }
}

SCOREP_ErrorCode initXRayPlugin() XRAY_INSTRUMENT_NEVER {
    return XRayPlugin::initXRay();
}

void finalizeXRayPlugin() XRAY_INSTRUMENT_NEVER {
    XRayPlugin::cleanupXRay();
}

