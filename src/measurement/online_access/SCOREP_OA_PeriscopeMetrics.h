/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OA_PERISCOPEMETRICS_H
#define SCOREP_OA_PERISCOPEMETRICS_H

/**
 * @file
 *
 * @brief   Persciope metrics
 *
 */

/* *INDENT-OFF* */

#define PSC_MIN_METRICID                 0
#define PSC_MAX_METRICID               168


typedef enum PSC_MetricGroup
{
    GROUP_PAPI_GENERIC        = 1,          /*Requests's from 1 to 169*/
    GROUP_PAPI_NATIVE_P6      = 2,
    GROUP_PAPI_NATIVE_NEHALEM = 3,
    GROUP_PAPI_NATIVE_ITANIUM = 4,
    GROUP_PAPI_NATIVE_NVIDIA  = 5,
    GROUP_CUPTI_NVIDIA        = 6,
    GROUP_MRIMONITOR_OVERHEAD = 7,
    GROUP_TIME_MEASUREMENT    = 8,
    GROUP_RUSAGE              = 9,
    GROUP_UNDEFINED           = 999
} PSC_MetricGroup;

typedef enum PSC_MetricID
{
    PSC_EXECUTION_TIME                   =  1,
    PSC_PAPI_L1_TCM                      =  2,
    PSC_PAPI_L1_DCM                      =  3,
    PSC_PAPI_L1_DCA                      =  4,
    PSC_PAPI_L1_ICM                      =  5,
    PSC_PAPI_L2_DCM                      =  6,
    PSC_PAPI_L2_DCA                      =  7,
    PSC_PAPI_L3_TCM                      =  8,
    PSC_PAPI_L3_TCA                      =  9,
    PSC_PAPI_TOT_INS                     = 10, // issued instructions on Itanium
    PSC_PAPI_INT_INS                     = 11,
    PSC_PAPI_FP_OPS                      = 12,
    PSC_PAPI_LD_INS                      = 13,
    PSC_PAPI_SR_INS                      = 14,
    PSC_PAPI_TLB_DM                      = 15,
    PSC_PAPI_TLB_IM                      = 16,
    PSC_PAPI_LST_INS                     = 17,
    PSC_PAPI_L2_ICR                      = 18,
    PSC_PAPI_L2_ICH                      = 19,
    PSC_PAPI_TOT_CYC                     = 20,
    PSC_BACK_END_BUBBLE_ALL              = 21,
    PSC_BACK_END_BUBBLE_FE               = 22,
    PSC_BE_EXE_BUBBLE_FRALL              = 23,
    PSC_BE_EXE_BUBBLE_GRALL              = 24,
    PSC_BE_EXE_BUBBLE_GRGR               = 25,
    PSC_BE_FLUSH_BUBBLE_BRU              = 26,
    PSC_BE_FLUSH_BUBBLE_XPN              = 27,
    PSC_BE_L1D_FPU_BUBBLE_FPU            = 28,
    PSC_BE_L1D_FPU_BUBBLE_L1D            = 29,
    PSC_BE_L1D_FPU_BUBBLE_L1D_FULLSTBUF  = 30,
    PSC_BE_L1D_FPU_BUBBLE_L1D_DCURECIR   = 31,
    PSC_BE_L1D_FPU_BUBBLE_L1D_HPW        = 32,
    PSC_BE_L1D_FPU_BUBBLE_L1D_TLB        = 33,
    PSC_BE_L1D_FPU_BUBBLE_L1D_L2BPRESS   = 34,
    PSC_BE_RSE_BUBBLE_ALL                = 35,
    PSC_FP_FALSE_SIRSTALL                = 36,
    PSC_FP_FLUSH_TO_ZERO                 = 37,
    PSC_FP_TRUE_SIRSTALL                 = 38,
    PSC_L1D_READ_MISSES_ALL              = 39,
    PSC_L2_DATA_REFERENCES_L2_DATA_READS = 40,
    PSC_L2_FORCE_RECIRC_FILL_HIT         = 41,
    PSC_L2_FORCE_RECIRC_L1W              = 42,
    PSC_L2_FORCE_RECIRC_OZQ_MISS         = 43,
    PSC_L2_FORCE_RECIRC_SAME_INDEX       = 44,
    PSC_L2_OZQ_CANCELS1_BANK_CONF        = 45,
    PSC_L3_READS_DATA_READ_ALL           = 46,
    PSC_L3_READS_DATA_READ_HIT           = 47,
    PSC_L3_READS_DATA_READ_MISS          = 48,
    PSC_L2_OZQ_FULL_THIS                 = 49,
    PSC_BE_FLUSH_BUBBLE_ALL              = 50,
    PSC_BE_L1D_FPU_BUBBLE_ALL            = 51,
    PSC_BE_EXE_BUBBLE_ALL                = 52,


    PSC_DATA_EAR_CACHE_LAT4              = 53,
    PSC_DATA_EAR_CACHE_LAT8              = 54,
    PSC_DATA_EAR_CACHE_LAT16             = 55,
    PSC_DATA_EAR_CACHE_LAT32             = 56,
    PSC_DATA_EAR_CACHE_LAT64             = 57,
    PSC_DATA_EAR_CACHE_LAT128            = 58,
    PSC_DATA_EAR_CACHE_LAT256            = 59,
    PSC_DATA_EAR_CACHE_LAT512            = 60,
    PSC_DATA_EAR_CACHE_LAT1024           = 61,
    PSC_DATA_EAR_CACHE_LAT2048           = 62,
    PSC_DATA_EAR_CACHE_LAT4096           = 63,

//p575 - Power Metric
    PSC_PM_START                         = 64,
    PSC_PM_RUN_CYC                       = 65,
    PSC_PM_DPU_HELD                      = 66,
    PSC_PM_DPU_HELD_FP_FX_MULT           = 67,
    PSC_PM_DPU_HELD_MULT_GPR             = 68,
    PSC_PM_FPU_FXMULT                    = 69,
    PSC_PM_FPU_FXDIV                     = 70,
    PSC_PM_FXU_PIPELINED_MULT_DIV        = 71,
    PSC_PM_LD_MISS_L1_CYC                = 72,
    PSC_PM_DATA_FROM_L2_CYC              = 73,
    PSC_PM_DATA_FROM_L21_CYC             = 74,
    PSC_PM_DATA_FROM_L3_CYC              = 75,
    PSC_PM_LSU_LMQ_FULL_CYC              = 76,
    PSC_PM_DC_PREF_OUT_OF_STREAMS        = 77,
    PSC_PM_DC_PREF_STREAM_ALLOC          = 78,
    PSC_PM_LSU_DERAT_MISS_CYC            = 79,
    PSC_PM_DERAT_MISS_4K                 = 80,
    PSC_PM_DERAT_MISS_64K                = 81,
    PSC_PM_DERAT_MISS_16M                = 82,
    PSC_PM_DERAT_MISS_16G                = 83,
    PSC_PM_DERAT_REF_4K                  = 84,
    PSC_PM_DERAT_REF_64K                 = 85,
    PSC_PM_DERAT_REF_16M                 = 86,
    PSC_PM_DERAT_REF_16G                 = 87,
    PSC_PM_L2_ST_REQ_DATA                = 88,
    PSC_PM_L2_ST_MISS_DATA               = 89,
    PSC_PM_LSU_REJECT_STQ_FULL           = 90,
    PSC_PM_ST_REF_L1                     = 91,
    PSC_PM_LSU_ST_CHAINED                = 92,
    PSC_PM_L2_PREF_LD                    = 93,
    PSC_PM_L1_PREF                       = 94,
    PSC_PM_LD_REF_L1                     = 95,
    PSC_PM_L2_PREF_ST                    = 96,
    PSC_PM_DPU_WT                        = 97,
    PSC_PM_BR_MPRED                      = 98,
    PSC_PM_BR_PRED                       = 99,
    PSC_PM_DPU_WT_IC_MISS                = 100,
    PSC_PM_LSU_REJECT_LHS                = 101,
    PSC_PM_DPU_HELD_GPR                  = 102,
    PSC_PM_DPU_HELD_FPQ                  = 103,
    PSC_PM_DPU_HELD_FPU_CR               = 104,
    PSC_PM_DPU_HELD_SMT                  = 105,
    PSC_PM_FPU_FCONV                     = 106,
    PSC_PM_FPU_FSQRT_FDIV                = 107,
    PSC_PM_FPU_FMA                       = 108,
    PSC_PM_DPU_HELD_FXU_MULTI            = 109,
    PSC_PM_DPU_HELD_INT                  = 110,
    PSC_PM_DPU_HELD_CR_LOGICAL           = 111,
    PSC_PM_L3SA_REF                      = 112,
    PSC_PM_L3SA_MISS                     = 113,
    PSC_PM_L3SA_HIT                      = 114,
    PSC_PM_L3SB_REF                      = 115,
    PSC_PM_L3SB_MISS                     = 116,
    PSC_PM_L3SB_HIT                      = 117,
    PSC_PM_FPU_ISSUE_0                   = 118,
    PSC_PM_FPU_ISSUE_1                   = 119,
    PSC_PM_FPU_ISSUE_2                   = 120,
    PSC_PM_FPU_ISSUE_DIV_SQRT_OVERLAP    = 121,
    PSC_PM_FPU_ISSUE_STALL_FPR           = 122,
    PSC_PM_FPU_ISSUE_STALL_ST            = 123,
    PSC_PM_FPU_FLOP                      = 124,
    PSC_PM_DATA_FROM_L2MISS              = 125,
    PSC_PM_DATA_FROM_L2                  = 126,
    PSC_PM_DATA_FROM_L3MISS              = 127,
    PSC_PM_DATA_FROM_L3                  = 128,
    PSC_PM_DATA_FROM_L21                 = 129,
    PSC_PM_DATA_FROM_LMEM                = 130,
    PSC_PM_L2_LD_REQ_DATA                = 131,
    PSC_PM_LD_REQ_L2                     = 132,
    PSC_PM_L2_LD_MISS_DATA               = 133,
    PSC_PM_INST_DISP                     = 134,
    PSC_PM_RUN_INST_CMPL                 = 135,
    PSC_PM_1PLUS_PPC_CMPL                = 136,
    PSC_PM_1PLUS_PPC_DISP                = 137,
    PSC_PM_LD_MISS_L1                    = 138,
    PSC_PM_END                           = 139,
//end p575

//added for benchmarking strategy
    PSC_PAPI_BR_CN                       = 140,
    PSC_PAPI_BR_INS                      = 141,
    PSC_PAPI_BR_MSP                      = 142,
    PSC_PAPI_L1_DCH                      = 143,
    PSC_PAPI_L2_DCH                      = 144,
    PSC_PAPI_L3_DCA                      = 145,
    PSC_PAPI_L3_DCH                      = 146,
    PSC_PAPI_L3_DCM                      = 147,
    PSC_PAPI_RES_STL                     = 148,
    PSC_PAPI_TLB_TL                      = 149,
    PSC_PAPI_TOT_IIS                     = 150,

    PSC_PAPI_L2_TCM                      = 151,
    PSC_PAPI_L2_TCH                      = 152,
//end PAPI

/* NP Metrics - Nehalem property*/
    PSC_NP_THREAD_P                      = 153,
    PSC_NP_UOPS_EXECUTED_PORT015         = 154,
    PSC_NP_UOPS_ISSUED_FUSED             = 155,
    PSC_NP_UOPS_ISSUED_ANY               = 156,
    PSC_NP_UOPS_RETIRED_ANY              = 157,
    PSC_NP_STALL_CYCLES                  = 158,
    PSC_NP_RESOURCE_STALLS_ANY           = 159,
    PSC_NP_INSTRUCTION_RETIRED           = 160,
    PSC_NP_MEM_INST_RETIRED_LOADS        = 161,
    PSC_NP_MEM_INST_RETIRED_STORES       = 162,
    PSC_NP_DTLB_MISSES_ANY               = 163,
    PSC_NP_DTLB_LOAD_MISSES_ANY          = 164,
    PSC_NP_DTLB_MISSES_WALK_COMPLETED    = 165,
    PSC_NP_ITLB_MISSES_ANY               = 166,
    PSC_NP_PARTIAL_ADDRESS_ALIAS         = 167,
    PSC_NP_UOPS_DECODED_MS               = 168,

    PSC_INSTANCES                        = 301,
    PSC_TOTAL_ISSUED_INSTRUCTIONS        = 302,

    //Maqao
    PSC_INT_FLOAT_CONVERSION             = 401,
    PSC_ADVANCE_LOAD                     = 402,
    PSC_PREFETCH                         = 403,
    PSC_FOUR_LOADS_IN_SUBSEQUENT_BUNDLES = 404,
    PSC_ISSUES                           = 405,
    PSC_THEORETICAL_ISSUE_BOUND          = 406,
    PSC_ITERATIONS_IN_SOFTWARE_PIPELINE  = 407,
    PSC_INSTRUCTIONS_IN_LOOP_BODY        = 408,
    //End Maqao

    //MPI
    PSC_MPI                              = 500,
    PSC_MPI_TIME_SPENT                   = 511,
    PSC_MPI_CALL_COUNT                   = 512,
    PSC_MPI_CALL_COUNT_REMOTE            = 513,
    PSC_MPI_REMOTE_SITE_COUNT            = 514,
    PSC_MPI_AGGREGATE_MESSAGE_SIZE       = 515,
    PSC_MPI_SGI_COUNTERS                 = 516,
    PSC_MPI_LATE_SEND                    = 517,
    PSC_MPI_EARLY_RECV                   = 518,
    PSC_MPI_LATE_RECV                    = 519,
    PSC_MPI_EARLY_BCAST                  = 520,
    PSC_MPI_LATE_BCAST                   = 521,
    PSC_MPI_EARLY_SCATTER                = 522,
    PSC_MPI_LATE_SCATTER                 = 523,
    PSC_MPI_LATE_GATHER                  = 530,
    PSC_MPI_LATE_REDUCE                  = 531,
    PSC_MPI_LATE_ALLREDUCE               = 540,
    PSC_MPI_LATE_ALLGATHER               = 541,
    PSC_MPI_LATE_ALLTOALL                = 542,
    PSC_MPI_LATE_BARRIER                 = 550,
    PSC_MPI_LATE_SCAN                    = 552,

    PSC_MRI_OVERHEAD                     = 600,
    PSC_MRI_LIBCALLS                     = 601,
    PSC_MRI_EXCL_OVERHEAD                = 602,

/* OpenMP Metrics */
    PSC_IMPLICIT_BARRIER_TIME            = 701,
    PSC_CRITICAL_REGION_CYCLE            = 702,
    PSC_CRITICAL_BODY_CYCLE              = 703,
    PSC_SINGLE_REGION_CYCLE              = 704,
    PSC_SINGLE_BODY_CYCLE                = 705,
    PSC_MASTER_BODY_CYCLE                = 706,
    PSC_PARALLEL_REGION_CYCLE            = 707,
    PSC_PARALLEL_REGION_BODY_CYCLE       = 708,
    PSC_OMP_BARRIER_CYCLE                = 709,
    PSC_ORDERED_REGION_CYCLE             = 710,
    PSC_OMP_ATOMIC_CYCLE                 = 801,
    PSC_OMP_SECTIONS_REGION_CYCLE        = 802,
    PSC_OMP_SECTION_BODY_CYCLE           = 803,
    PSC_OMP_DO_REGION_CYCLE              = 804,
    PSC_TASK_REGION_CYCLE                = 805,
    PSC_TASK_REGION_BODY_CYCLE           = 806,
    PSC_TASKS_CREATED                    = 807,
    PSC_TASKS_EXECUTED                   = 808,


    PSC_UNDEFINED_METRIC                 = 999,
} PSC_MetricID;

/*----------------------------------------------------------------------------*/
/* CID = CounterId */
#define CID_NO_CODE                                                      0xffffffff

typedef struct PSC_Event
{
    PSC_MetricGroup      metricGroup;
    const char* metricName;
    PSC_MetricID         metricID;
} PSC_MetricType;

static const PSC_MetricType PSC_MetricList[] =
{
//{ enum Group             ,    const char* EventName              , enum Metric,                       },
    { GROUP_TIME_MEASUREMENT,    "EXECUTION_TIME",                     PSC_EXECUTION_TIME                             }, //0
    { GROUP_PAPI_GENERIC,        "PAPI_L1_TCM",                        PSC_PAPI_L1_TCM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L1_DCM",                        PSC_PAPI_L1_DCM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L1_DCA",                        PSC_PAPI_L1_DCA                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L1_ICM",                        PSC_PAPI_L1_ICM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L2_DCM",                        PSC_PAPI_L2_DCM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L2_DCA",                        PSC_PAPI_L2_DCA                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L3_TCM",                        PSC_PAPI_L3_TCM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L3_TCA",                        PSC_PAPI_L3_TCA                                },
    { GROUP_PAPI_GENERIC,        "PAPI_TOT_INS",                       PSC_PAPI_TOT_INS                               }, //9
    { GROUP_PAPI_GENERIC,        "PAPI_INT_INS",                       PSC_PAPI_INT_INS                               },
    { GROUP_PAPI_GENERIC,        "PAPI_FP_OPS",                        PSC_PAPI_FP_OPS                                },
    { GROUP_PAPI_GENERIC,        "PAPI_LD_INS",                        PSC_PAPI_LD_INS                                },
    { GROUP_PAPI_GENERIC,        "PAPI_SR_INS",                        PSC_PAPI_SR_INS                                },
    { GROUP_PAPI_GENERIC,        "PAPI_TLB_DM",                        PSC_PAPI_TLB_DM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_TLB_IM",                        PSC_PAPI_TLB_IM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_LST_INS",                       PSC_PAPI_LST_INS                               },
    { GROUP_PAPI_GENERIC,        "PAPI_L2_ICR",                        PSC_PAPI_L2_ICR                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L2_ICH",                        PSC_PAPI_L2_ICH                                },
    { GROUP_PAPI_GENERIC,        "PAPI_TOT_CYC",                       PSC_PAPI_TOT_CYC                               }, //19

    { GROUP_PAPI_NATIVE_ITANIUM, "BACK_END_BUBBLE_ALL",                PSC_BACK_END_BUBBLE_ALL                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "BACK_END_BUBBLE_FE",                 PSC_BACK_END_BUBBLE_FE                         },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_EXE_BUBBLE_FRALL",                PSC_BE_EXE_BUBBLE_FRALL                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_EXE_BUBBLE_GRALL",                PSC_BE_EXE_BUBBLE_GRALL                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_EXE_BUBBLE_GRGR",                 PSC_BE_EXE_BUBBLE_GRGR                         },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_FLUSH_BUBBLE_BRU",                PSC_BE_FLUSH_BUBBLE_BRU                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_FLUSH_BUBBLE_XPN",                PSC_BE_FLUSH_BUBBLE_XPN                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_FPU",              PSC_BE_L1D_FPU_BUBBLE_FPU                      },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_L1D",              PSC_BE_L1D_FPU_BUBBLE_L1D                      },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_L1D_FULLSTBUF",    PSC_BE_L1D_FPU_BUBBLE_L1D_FULLSTBUF            }, //29
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_L1D_DCURECIR",     PSC_BE_L1D_FPU_BUBBLE_L1D_DCURECIR             },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_L1D_HPW",          PSC_BE_L1D_FPU_BUBBLE_L1D_HPW                  },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_L1D_TLB",          PSC_BE_L1D_FPU_BUBBLE_L1D_TLB                  },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_L1D_L2BPRESS",     PSC_BE_L1D_FPU_BUBBLE_L1D_L2BPRESS             },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_RSE_BUBBLE_ALL",                  PSC_BE_RSE_BUBBLE_ALL                          },
    { GROUP_PAPI_NATIVE_ITANIUM, "FP_FALSE_SIRSTALL",                  PSC_FP_FALSE_SIRSTALL                          },
    { GROUP_PAPI_NATIVE_ITANIUM, "FP_FLUSH_TO_ZERO",                   PSC_FP_FLUSH_TO_ZERO                           },
    { GROUP_PAPI_NATIVE_ITANIUM, "FP_TRUE_SIRSTALL",                   PSC_FP_TRUE_SIRSTALL                           },
    { GROUP_PAPI_NATIVE_ITANIUM, "L1D_READ_MISSES_ALL",                PSC_L1D_READ_MISSES_ALL                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "L2_DATA_REFERENCES_L2_DATA_READS",   PSC_L2_DATA_REFERENCES_L2_DATA_READS           }, //39
    { GROUP_PAPI_NATIVE_ITANIUM, "L2_FORCE_RECIRC_FILL_HIT",           PSC_L2_FORCE_RECIRC_FILL_HIT                   },
    { GROUP_PAPI_NATIVE_ITANIUM, "L2_FORCE_RECIRC_L1W",                PSC_L2_FORCE_RECIRC_L1W                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "L2_FORCE_RECIRC_OZQ_MISS",           PSC_L2_FORCE_RECIRC_OZQ_MISS                   },
    { GROUP_PAPI_NATIVE_ITANIUM, "L2_FORCE_RECIRC_SAME_INDEX",         PSC_L2_FORCE_RECIRC_SAME_INDEX                 },
    { GROUP_PAPI_NATIVE_ITANIUM, "L2_OZQ_CANCELS1_BANK_CONF",          PSC_L2_OZQ_CANCELS1_BANK_CONF                  },
    { GROUP_PAPI_NATIVE_ITANIUM, "L3_READS_DATA_READ_ALL",             PSC_L3_READS_DATA_READ_ALL                     },
    { GROUP_PAPI_NATIVE_ITANIUM, "L3_READS_DATA_READ_HIT",             PSC_L3_READS_DATA_READ_HIT                     },
    { GROUP_PAPI_NATIVE_ITANIUM, "L3_READS_DATA_READ_MISS",            PSC_L3_READS_DATA_READ_MISS                    },
    { GROUP_PAPI_NATIVE_ITANIUM, "L2_OZQ_FULL_THIS",                   PSC_L2_OZQ_FULL_THIS                           },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_FLUSH_BUBBLE_ALL",                PSC_BE_FLUSH_BUBBLE_ALL                        }, //49
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_L1D_FPU_BUBBLE_ALL",              PSC_BE_L1D_FPU_BUBBLE_ALL                      },
    { GROUP_PAPI_NATIVE_ITANIUM, "BE_EXE_BUBBLE_ALL",                  PSC_BE_EXE_BUBBLE_ALL                          },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT4",                PSC_DATA_EAR_CACHE_LAT4                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT8",                PSC_DATA_EAR_CACHE_LAT8                        },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT16",               PSC_DATA_EAR_CACHE_LAT16                       },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT32",               PSC_DATA_EAR_CACHE_LAT32                       },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT64",               PSC_DATA_EAR_CACHE_LAT64                       },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT128",              PSC_DATA_EAR_CACHE_LAT128                      },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT256",              PSC_DATA_EAR_CACHE_LAT256                      },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT512",              PSC_DATA_EAR_CACHE_LAT512                      }, //59
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT1024",             PSC_DATA_EAR_CACHE_LAT1024                     },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT2048",             PSC_DATA_EAR_CACHE_LAT2048                     },
    { GROUP_PAPI_NATIVE_ITANIUM, "DATA_EAR_CACHE_LAT4096",             PSC_DATA_EAR_CACHE_LAT4096                     },

    //p575
    { GROUP_UNDEFINED,           "PM_START",                           PSC_PM_START                                   },
    { GROUP_PAPI_NATIVE_P6,      "PM_RUN_CYC",                         PSC_PM_RUN_CYC                                 },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD",                        PSC_PM_DPU_HELD                                },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_FP_FX_MULT",             PSC_PM_DPU_HELD_FP_FX_MULT                     },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_MULT_GPR",               PSC_PM_DPU_HELD_MULT_GPR                       },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_FXMULT",                      PSC_PM_FPU_FXMULT                              },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_FXDIV",                       PSC_PM_FPU_FXDIV                               }, //69
    { GROUP_PAPI_NATIVE_P6,      "PM_FXU_PIPELINED_MULT_DIV",          PSC_PM_FXU_PIPELINED_MULT_DIV                  },
    { GROUP_PAPI_NATIVE_P6,      "PM_LD_MISS_L1_CYC",                  PSC_PM_LD_MISS_L1_CYC                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L2_CYC",                PSC_PM_DATA_FROM_L2_CYC                        },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L21_CYC",               PSC_PM_DATA_FROM_L21_CYC                       },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L3_CYC",                PSC_PM_DATA_FROM_L3_CYC                        },
    { GROUP_PAPI_NATIVE_P6,      "PM_LSU_LMQ_FULL_CYC",                PSC_PM_LSU_LMQ_FULL_CYC                        },
    { GROUP_PAPI_NATIVE_P6,      "PM_DC_PREF_OUT_OF_STREAMS",          PSC_PM_DC_PREF_OUT_OF_STREAMS                  },
    { GROUP_PAPI_NATIVE_P6,      "PM_DC_PREF_STREAM_ALLOC",            PSC_PM_DC_PREF_STREAM_ALLOC                    },
    { GROUP_PAPI_NATIVE_P6,      "PM_LSU_DERAT_MISS_CYC",              PSC_PM_LSU_DERAT_MISS_CYC                      },
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_MISS_4K",                   PSC_PM_DERAT_MISS_4K                           }, //79
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_MISS_64K",                  PSC_PM_DERAT_MISS_64K                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_MISS_16M",                  PSC_PM_DERAT_MISS_16M                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_MISS_16G",                  PSC_PM_DERAT_MISS_16G                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_REF_4K",                    PSC_PM_DERAT_REF_4K                            },
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_REF_64K",                   PSC_PM_DERAT_REF_64K                           },
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_REF_16M",                   PSC_PM_DERAT_REF_16M                           },
    { GROUP_PAPI_NATIVE_P6,      "PM_DERAT_REF_16G",                   PSC_PM_DERAT_REF_16G                           },
    { GROUP_PAPI_NATIVE_P6,      "PM_L2_ST_REQ_DATA",                  PSC_PM_L2_ST_REQ_DATA                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_L2_ST_MISS_DATA",                 PSC_PM_L2_ST_MISS_DATA                         },
    { GROUP_PAPI_NATIVE_P6,      "PM_LSU_REJECT_STQ_FULL",             PSC_PM_LSU_REJECT_STQ_FULL                     }, //89
    { GROUP_PAPI_NATIVE_P6,      "PM_ST_REF_L1",                       PSC_PM_ST_REF_L1                               },
    { GROUP_PAPI_NATIVE_P6,      "PM_LSU_ST_CHAINED",                  PSC_PM_LSU_ST_CHAINED                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_L2_PREF_LD",                      PSC_PM_L2_PREF_LD                              },
    { GROUP_PAPI_NATIVE_P6,      "PM_L1_PREF",                         PSC_PM_L1_PREF                                 },
    { GROUP_PAPI_NATIVE_P6,      "PM_LD_REF_L1",                       PSC_PM_LD_REF_L1                               },
    { GROUP_PAPI_NATIVE_P6,      "PM_L2_PREF_ST",                      PSC_PM_L2_PREF_ST                              },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_WT",                          PSC_PM_DPU_WT                                  },
    { GROUP_PAPI_NATIVE_P6,      "PM_BR_MPRED",                        PSC_PM_BR_MPRED                                },
    { GROUP_PAPI_NATIVE_P6,      "PM_BR_PRED",                         PSC_PM_BR_PRED                                 },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_WT_IC_MISS",                  PSC_PM_DPU_WT_IC_MISS                          }, //99
    { GROUP_PAPI_NATIVE_P6,      "PM_LSU_REJECT_LHS",                  PSC_PM_LSU_REJECT_LHS                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_GPR",                    PSC_PM_DPU_HELD_GPR                            },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_FPQ",                    PSC_PM_DPU_HELD_FPQ                            },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_FPU_CR",                 PSC_PM_DPU_HELD_FPU_CR                         },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_SMT",                    PSC_PM_DPU_HELD_SMT                            },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_FCONV",                       PSC_PM_FPU_FCONV                               },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_FSQRT_FDIV",                  PSC_PM_FPU_FSQRT_FDIV                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_FMA",                         PSC_PM_FPU_FMA                                 },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_FXU_MULTI",              PSC_PM_DPU_HELD_FXU_MULTI                      },
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_INT",                    PSC_PM_DPU_HELD_INT                            }, //109
    { GROUP_PAPI_NATIVE_P6,      "PM_DPU_HELD_CR_LOGICAL",             PSC_PM_DPU_HELD_CR_LOGICAL                     },
    { GROUP_PAPI_NATIVE_P6,      "PM_L3SA_REF",                        PSC_PM_L3SA_REF                                },
    { GROUP_PAPI_NATIVE_P6,      "PM_L3SA_MISS",                       PSC_PM_L3SA_MISS                               },
    { GROUP_PAPI_NATIVE_P6,      "PM_L3SA_HIT",                        PSC_PM_L3SA_HIT                                },
    { GROUP_PAPI_NATIVE_P6,      "PM_L3SB_REF",                        PSC_PM_L3SB_REF                                },
    { GROUP_PAPI_NATIVE_P6,      "PM_L3SB_MISS",                       PSC_PM_L3SB_MISS                               },
    { GROUP_PAPI_NATIVE_P6,      "PM_L3SB_HIT",                        PSC_PM_L3SB_HIT                                },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_ISSUE_0",                     PSC_PM_FPU_ISSUE_0                             },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_ISSUE_1",                     PSC_PM_FPU_ISSUE_1                             },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_ISSUE_2",                     PSC_PM_FPU_ISSUE_2                             }, //119
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_ISSUE_DIV_SQRT_OVERLAP",      PSC_PM_FPU_ISSUE_DIV_SQRT_OVERLAP              },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_ISSUE_STALL_FPR",             PSC_PM_FPU_ISSUE_STALL_FPR                     },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_ISSUE_STALL_ST",              PSC_PM_FPU_ISSUE_STALL_ST                      },
    { GROUP_PAPI_NATIVE_P6,      "PM_FPU_FLOP",                        PSC_PM_FPU_FLOP                                },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L2MISS",                PSC_PM_DATA_FROM_L2MISS                        },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L2",                    PSC_PM_DATA_FROM_L2                            },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L3MISS",                PSC_PM_DATA_FROM_L3MISS                        },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L3",                    PSC_PM_DATA_FROM_L3                            },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_L21",                   PSC_PM_DATA_FROM_L21                           },
    { GROUP_PAPI_NATIVE_P6,      "PM_DATA_FROM_LMEM",                  PSC_PM_DATA_FROM_LMEM                          }, //129
    { GROUP_PAPI_NATIVE_P6,      "PM_L2_LD_REQ_DATA",                  PSC_PM_L2_LD_REQ_DATA                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_LD_REQ_L2",                       PSC_PM_LD_REQ_L2                               },
    { GROUP_PAPI_NATIVE_P6,      "PM_L2_LD_MISS_DATA",                 PSC_PM_L2_LD_MISS_DATA                         },
    { GROUP_PAPI_NATIVE_P6,      "PM_INST_DISP",                       PSC_PM_INST_DISP                               },
    { GROUP_PAPI_NATIVE_P6,      "PM_RUN_INST_CMPL",                   PSC_PM_RUN_INST_CMPL                           },
    { GROUP_PAPI_NATIVE_P6,      "PM_1PLUS_PPC_CMPL",                  PSC_PM_1PLUS_PPC_CMPL                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_1PLUS_PPC_DISP",                  PSC_PM_1PLUS_PPC_DISP                          },
    { GROUP_PAPI_NATIVE_P6,      "PM_LD_MISS_L1",                      PSC_PM_LD_MISS_L1                              },
    { GROUP_UNDEFINED,           "PM_END",                             PSC_PM_END                                     },
    //p575

    // added for benchmarking strategy
    { GROUP_PAPI_GENERIC,        "PAPI_BR_CN",                         PSC_PAPI_BR_CN                                 }, //139
    { GROUP_PAPI_GENERIC,        "PAPI_BR_INS",                        PSC_PAPI_BR_INS                                },
    { GROUP_PAPI_GENERIC,        "PAPI_BR_MSP",                        PSC_PAPI_BR_MSP                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L1_DCH",                        PSC_PAPI_L1_DCH                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L2_DCH",                        PSC_PAPI_L2_DCH                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L3_DCA",                        PSC_PAPI_L3_DCA                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L3_DCH",                        PSC_PAPI_L3_DCH                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L3_DCM",                        PSC_PAPI_L3_DCM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_RES_STL",                       PSC_PAPI_RES_STL                               },
    { GROUP_PAPI_GENERIC,        "PAPI_TLB_TL",                        PSC_PAPI_TLB_TL                                },
    { GROUP_PAPI_GENERIC,        "PAPI_TOT_IIS",                       PSC_PAPI_TOT_IIS                               }, //149
    { GROUP_PAPI_GENERIC,        "PAPI_L2_TCM",                        PSC_PAPI_L2_TCM                                },
    { GROUP_PAPI_GENERIC,        "PAPI_L2_TCH",                        PSC_PAPI_L2_TCH                                },
    // papi

    //NP Metrics
    { GROUP_PAPI_NATIVE_NEHALEM, "CPU_CLK_UNHALTED:THREAD_P",          PSC_NP_THREAD_P                                },
    { GROUP_PAPI_NATIVE_NEHALEM, "UOPS_EXECUTED:PORT015",              PSC_NP_UOPS_EXECUTED_PORT015                   },
    { GROUP_PAPI_NATIVE_NEHALEM, "UOPS_ISSUED:FUSED",                  PSC_NP_UOPS_ISSUED_FUSED                       },
    { GROUP_PAPI_NATIVE_NEHALEM, "UOPS_ISSUED:ANY",                    PSC_NP_UOPS_ISSUED_ANY                         },
    { GROUP_PAPI_NATIVE_NEHALEM, "UOPS_RETIRED:ANY",                   PSC_NP_UOPS_RETIRED_ANY                        },
    { GROUP_PAPI_NATIVE_NEHALEM, "SQ_FULL_STALL_CYCLES",               PSC_NP_STALL_CYCLES                            },
    { GROUP_PAPI_NATIVE_NEHALEM, "RESOURCE_STALLS:ANY",                PSC_NP_RESOURCE_STALLS_ANY                     },
    { GROUP_PAPI_NATIVE_NEHALEM, "INSTRUCTION_RETIRED",                PSC_NP_INSTRUCTION_RETIRED                     }, //159
    { GROUP_PAPI_NATIVE_NEHALEM, "MEM_INST_RETIRED:LOADS",             PSC_NP_MEM_INST_RETIRED_LOADS                  },
    { GROUP_PAPI_NATIVE_NEHALEM, "MEM_INST_RETIRED:STORES",            PSC_NP_MEM_INST_RETIRED_STORES                 },
    { GROUP_PAPI_NATIVE_NEHALEM, "DTLB_MISSES:ANY",                    PSC_NP_DTLB_MISSES_ANY                         },
    { GROUP_PAPI_NATIVE_NEHALEM, "DTLB_LOAD_MISSES:ANY",               PSC_NP_DTLB_LOAD_MISSES_ANY                    },
    { GROUP_PAPI_NATIVE_NEHALEM, "DTLB_MISSES:WALK_COMPLETED",         PSC_NP_DTLB_MISSES_WALK_COMPLETED              },
    { GROUP_PAPI_NATIVE_NEHALEM, "ITLB_MISSES:ANY",                    PSC_NP_ITLB_MISSES_ANY                         },
    { GROUP_PAPI_NATIVE_NEHALEM, "PARTIAL_ADDRESS_ALIAS",              PSC_NP_PARTIAL_ADDRESS_ALIAS                   },
    { GROUP_PAPI_NATIVE_NEHALEM, "UOPS_DECODED:MS",                    PSC_NP_UOPS_DECODED_MS                         }, //Last metric
    { GROUP_MRIMONITOR_OVERHEAD, "OVERHEAD",                           PSC_MRI_OVERHEAD                               }, //169
    { GROUP_MRIMONITOR_OVERHEAD, "LIBCALLS",                           PSC_MRI_LIBCALLS                               },
    { GROUP_UNDEFINED,           "UNDEFINED",                          PSC_UNDEFINED_METRIC                           }
};

#endif /* SCOREP_OA_PERISCOPEMETRICS_H */
