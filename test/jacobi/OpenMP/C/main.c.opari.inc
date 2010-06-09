#include <config.h>
#include "pomp_lib.h"

extern uint64_t POMP_TPD_MANGLED;
#define STR_( s ) #s
#define STR( s ) STR_( s )
#define PRAGMA_OMP_THREADPRIVATE(tpd) _Pragma( STR(omp threadprivate(tpd)) )
#ifdef _OPENMP
PRAGMA_OMP_THREADPRIVATE(POMP_TPD_MANGLED)
#endif

#define POMP_DLIST_00001 shared(pomp_region_1)
static POMP_Region_handle pomp_region_1;

void POMP_Init_regions_1269863448102139_1()
{
    POMP_Assign_handle( &pomp_region_1, "65*regionType=parallelfor*sscl=main.c:123:123*escl=main.c:139:139**" );
}
