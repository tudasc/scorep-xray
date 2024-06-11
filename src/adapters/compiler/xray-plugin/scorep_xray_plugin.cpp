/**
 * @file scorep_xry_plugin.cpp
 * @brief Score-P LLVM XRAY instrumentation plugin - runtime management
 */

#include <iostream>
#include "scorep_xray_plugin.hpp"
//#include "../../../measurement/scorep_environment.h" // TODO!: Determine best source for executable name?
#include "../../measurement/include/SCOREP_RuntimeManagement.h"


// No need to check whether XRAY runtime is available since this header is only included when xray plugin
// instrumentation is enabled, which means compiling and linking with -fxray-instrumented
#include <xray/xray_log_interface.h>
#include <xray/xray_interface.h>
#include <xray/xray_records.h>

#include "llvm/Demangle/Demangle.h"
#include "llvm/DebugInfo/Symbolize/Symbolize.h"
#include <llvm/XRay/InstrumentationMap.h>


/**
 * Builds a map XRayFuncId -> XrayIFMetadata by first retrieving sled info from XRay, filtering to unique fIDs
 * (Entry/exit sleds not needed), symbolizing and then demangling the found addresses
 * @param execFileName Name of the file to retrieve Function data from, should be full path to current executable
 * @return Map XrayFuncID -> Metadata about that function
 */
std::unordered_map<int32_t, XRayPlugin::XRayIFuncMetadata>buildFunctionMap(std::string& execFileName) XRAY_INSTRUMENT_NEVER {
    std::unordered_map<int, XRayPlugin::XRayIFuncMetadata> idMap;
    auto maybeMap = llvm::xray::loadInstrumentationMap(execFileName);
    if(auto err = maybeMap.takeError()){
        // TODO!: Maybe make use of UTILS_ERROR or FATAL to abort?
        std::cerr << "Could not read XRay instrumentation map!: " << toString(std::move(err)) << std::endl;
        return idMap;
    }
    auto funcAddressMap = maybeMap.get().getFunctionAddresses();
    for(auto mapping : funcAddressMap){
        int32_t funcId = mapping.first;
        if(idMap.find(funcId) == idMap.end()){
            // This funcID is new
            uint64_t funcAddr = mapping.second;
            llvm::symbolize::LLVMSymbolizer symbolizer;
            llvm::object::SectionedAddress sectAddress{funcAddr}; // init Address but keep SectionIndex default
            auto maybeFuncInfo = symbolizer.symbolizeCode(execFileName, sectAddress);
            if(auto err = maybeFuncInfo.takeError()){
                std::cerr << "Could not get symbol for XRay instrumented function " << funcId << " @" << funcAddr
                << ": " << toString(std::move(err)) << std::endl;
                // TODO!: Add to map or just discard?
                idMap.insert({funcId, XRayPlugin::XRayIFuncMetadata(funcId, funcAddr)});
            } else{
                std::string funcNameMangled = maybeFuncInfo.get().FunctionName;
                // TODO!: Check why mangled = demangled
                std::string funcNameDemangled = llvm::demangle(funcNameMangled);
                idMap.insert({funcId, XRayPlugin::XRayIFuncMetadata(funcId, funcAddr, funcNameMangled, funcNameDemangled)});
#ifdef SCOREP_XRAY_DEBUG
                std::cout << funcId << " @" << funcAddr << ": " << "\n\tdemangled: " << funcNameDemangled
                          << "\n\tname: " << funcNameMangled << std::endl;
#endif
            }
        }
    }
    return idMap;
}


/**
* @return 0 (SCOREP_SUCCESS) on success, error code otherwise
*
*/
bool patchAndRegister(std::unordered_map<int32_t, XRayPlugin::XRayIFuncMetadata> instrumentedFunctions,
                                  std::string filter) XRAY_INSTRUMENT_NEVER{
    __xray_patch();
    return true;
}

void handleInstrumentationPoint(int32_t fid, XRayEntryType entryType) XRAY_INSTRUMENT_NEVER {
    std::cout << fid << ":" << entryType << std::endl;
}


bool XRayPlugin::initXRay() XRAY_INSTRUMENT_NEVER{
    std::cout << "Xray active!" << std::endl;
    __xray_init(); // Safe even if it is already initialized
    bool success = __xray_set_handler(&handleInstrumentationPoint);
    if(!success){
        std::cerr << "Could not set XRay handler function!" << std::endl;
        // TODO!: Handle 'spurious calls' as per docs?
    }
    bool execNameIsFile;
    std::string fileName = SCOREP_GetExecutableName(&execNameIsFile);
    auto functionMap = buildFunctionMap(fileName);
    patchAndRegister(functionMap, "TODO!: Filter");

    return true;
}

namespace {
    struct InitXRayPlugin {
        InitXRayPlugin() XRAY_INSTRUMENT_NEVER { XRayPlugin::initXRay(); }
    };
    InitXRayPlugin _;
}

