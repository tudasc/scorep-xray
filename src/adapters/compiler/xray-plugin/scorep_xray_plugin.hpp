/**
 * @file scorep_xry_plugin.hpp
 * @brief Score-P LLVM XRAY instrumentation plugin
 * Sources are instrumented with basic XRAY functionality by scorep at compile time.
 * XRAYPlugin namespace contains entry point to the management of the XRay plugin to invoke at runtime
 * Other functionality implemented as free functions in {@link src/adapters/compiler/xray-plugin/scorep_xray_plugin.cpp}
 */

#ifndef SCOREP_XRAY_PLUGIN_HPP
#define SCOREP_XRAY_PLUGIN_HPP

#define XRAY_INSTRUMENT_NEVER __attribute__((xray_never_instrument))
#define XRAY_INSTRUMENT_ALWAYS __attribute__((xray_always_instrument))

#include <config.h>
#include <SCOREP_ErrorCodes.h>
#include <string>
extern "C" {
#include "scorep_compiler_plugin.h"
}
#include <unordered_map>
#include <xray/xray_interface.h>
#include <cstring>


namespace XRayPlugin {

    // Copy value for invalid region as region description expects a pointer to it
    static uint32_t invalid_region_id = SCOREP_INVALID_REGION;

    /**
    * Can hold metadata about a xray instrumented function, enriched with name info for Score-P
    */
    struct XRayFuncMetadata {
        int32_t xRayFuncId{0};
        uint64_t address{0};
        scorep_compiler_region_description* regionDescription;

        XRayFuncMetadata(int32_t xRayFuncId, uint64_t address, std::string& funcNameMangled, std::string& funcNameDemangled,
                         std::string& sourceFile, int startLine, int endLine)
        : xRayFuncId(xRayFuncId), address(address){
            auto nameMangled = strdup(funcNameMangled.c_str());
            auto nameDemangled = strdup(funcNameDemangled.c_str());
            auto file = strdup(sourceFile.c_str());
            regionDescription = new scorep_compiler_region_description{
                    &invalid_region_id, //region is reset in register call, init with unknown region
                    nameMangled,
                    nameDemangled,
                    file,
                    startLine,
                    endLine,
                    0
            };
        }

        XRayFuncMetadata(int32_t xRayFuncId, uint64_t address, scorep_compiler_region_description* regionDescription)
                         : xRayFuncId(xRayFuncId), address(address), regionDescription(regionDescription){}
    };


    /**
     * Initializes XRay runtime instrumentation and sets everything up.
     * On success, the application is fully instrumented and patched according to user specs, therefore ready to run
     * @return true on success, false otherwise
     */
     SCOREP_ErrorCode initXRay() XRAY_INSTRUMENT_NEVER;
};

#endif





