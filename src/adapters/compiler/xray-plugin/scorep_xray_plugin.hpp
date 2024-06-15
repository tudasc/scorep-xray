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

    /**
    * Can hold metadata about a xray instrumented function, enriched with name info for Score-P
    */
    struct XRayFuncMetadata {
        int32_t xRayFuncId{0};
        uint64_t address{0};
        scorep_compiler_region_description *regionDescription;

        XRayFuncMetadata(int32_t xRayFuncId, uint64_t address, scorep_compiler_region_description *regionDescription)
                : xRayFuncId(xRayFuncId), address(address), regionDescription(regionDescription) {}
    };


    /**
     * Creates a new, trivially copy-able scorep region description on the heap that can be referenced after passed
     * values go out of scope. Make sure to free contents once it is no longer needed.
     */
    scorep_compiler_region_description createRegionDesc(std::string &funcNameMangled,
                                                         std::string &funcNameDemangled,
                                                         std::string &sourceFile, const uint32_t startLine, const uint32_t endLine){
        //TODO!: Figure out who is responsible for memory management, and if these pointers need to be freed
        // (Scorep seems to attempt to free handle?) Region infos are only created once and used the whole runtime

        // Copy strings to heap as they are not available once out of scope, but region info needs pointers to them
        auto nameMangled = strdup(funcNameMangled.c_str());
        auto nameDemangled = strdup(funcNameDemangled.c_str());
        auto file = strdup(sourceFile.c_str());
        // Handle is modified during lifetime, so it too must be persistent somewhere, but also requires pointer
        // and per-region access. Therefore, push it onto heap per created region info.
        auto heapHandle = new uint32_t (SCOREP_INVALID_REGION);
        return scorep_compiler_region_description{
                heapHandle, //region is reset in register call, init with unknown region
                nameDemangled,
                nameMangled,
                file,
                static_cast<int>(startLine),
                static_cast<int>(endLine),
                0
        };
    }


    /**
     * Initializes XRay runtime instrumentation and sets everything up.
     * On success, the application is fully instrumented and patched according to user specs, therefore ready to run
     * @return true on success, false otherwise
     */
    SCOREP_ErrorCode initXRay() XRAY_INSTRUMENT_NEVER;
};

#endif





