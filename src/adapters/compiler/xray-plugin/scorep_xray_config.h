//
// Created by paul on 6/20/24.
//

#ifndef _BUILD_SCOREP_XRAY_CONFIG_H
#define _BUILD_SCOREP_XRAY_CONFIG_H

namespace XRayPlugin{

        struct Config {
            bool useDefaultInstrumentFilter{true};
            bool deleteInstrumentFilterAfterCompile{true};
            int instructionThreshold{1};
            bool compileWithDebug{true};
        };

        namespace Filters{
            /*
         * Put default filters here as strings and write them to disk during instrumentation
         * This is necessary as the plugin can't assume the existence of the files at a specific location since the
         * user might copy the Score-P executable somewhere else, outside expected installation/_build directories
         */
            static const std::string
                    NO_INSTRUMENTATION("# This file is in the format of llvm sanitizer spacial case lists\n"
                                       "# It is used during compilation of the xray plugin to prevent internal methods from being instrumented\n"
                                       "# by effectively annotating every function that is involved in the compilation with never_instrument attributes\n"
                                       "[never]\n"
                                       "fun:*\n"
                                       "src:*\n");

            static const std::string
                    NO_INTERNALS("#!special-case-list-v1\n"
                                 "# XRay filter file that resembles the filter applied in the llvm plugin for better comparability and as a general\n"
                                 "# \"good\" baseline for which functions to exclude\n"
                                 "\n"
                                 "[never]\n"
                                 "fun:POMP*\n"
                                 "fun:Pomp*\n"
                                 "fun:pomp*\n"
                                 "fun:SCOREP_*\n"
                                 "fun:scorep_*\n"
                                 "fun:OTF2_*\n"
                                 "fun:otf2_*\n"
                                 "fun:cube_*\n"
                                 "fun:cubew_*\n"
                                 "fun:.omp*\n"
                                 "fun:*DIR.OMP.*\n"
                                 "fun:*.extracted*\n"
                                 "fun:*_tree_reduce_*\n"
                                 "fun:..omp*\n"
                                 "fun:__omp*\n"
                                 "fun:..acc*\n"
                                 "fun:virtual thunk*\n"
                                 "fun:non-virtual thunk*\n"
                                 "fun:*Kokkos::Tools*\n"
                                 "fun:*Kokkos::Profiling*\n"
                                 "fun:*6Kokkos5Tools*\n"
                                 "fun:*6Kokkos9Profiling*\n"
                                 "fun:_GLOBAL__*\n"
                                 "fun:*__gnu_cxx::*\n"
                                 "fun:*__cxx_*\n"
                                 "fun:*std::*\n"
                                 "fun:__clang_*\n"
                                 "fun:omp*$omp*$*\n"
                                 "fun:*.omp_outlined*\n"
                                 "fun:*.omp_outlined_debug__*\n"
                                 "fun:*_omp_*\n"
                                 "fun:*_omp$*\n"
                                 "fun:*__kmpc*\n"
                                 "fun:ompx::*\n"
                                 "fun:__keep_alive*\n"
                                 "fun:__assert_fail*\n"
                                 "fun:*__internal_trig_*\n"
                                 "fun:*thread-local wrapper routine*\n"
                                 "fun:*(anonymous namespace)::invokeMicrotask*\n"
                                 "fun:.red_init.*\n"
                                 "fun:.red_comb.*\n"
                                 "fun:MPI::*\n"
                                 "fun:*__device_stub__*\n"
                                 "fun:__cuda*\n"
                                 "fun:_*_nv_*\n"
                                 "fun:__sti_*cuda*\n"
                                 "fun:__hip*\n");
        }

} // XRayPlugin

#endif //_BUILD_SCOREP_XRAY_CONFIG_H
