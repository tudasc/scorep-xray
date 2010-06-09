#include <config.h>
#include "pomp_lib.h"

extern uint64_t POMP_TPD_MANGLED;
#define STR_( s ) #s
#define STR( s ) STR_( s )
#define PRAGMA_OMP_THREADPRIVATE(tpd) _Pragma( STR(omp threadprivate(tpd)) )
#ifdef _OPENMP
PRAGMA_OMP_THREADPRIVATE(POMP_TPD_MANGLED)
#endif

extern "C"
{

#define POMP_DLIST_00001 shared(pomp_region_1)
static POMP_Region_handle pomp_region_1;

void POMP_Init_regions_1269949782624507_1()
{
    POMP_Assign_handle( &pomp_region_1, "69*regionType=parallelfor*sscl=main.cpp:111:111*escl=main.cpp:127:127**" );
}

}