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

void POMP_Init_regions_1275696211160436_1()
{
    POMP_Assign_handle( &pomp_region_1, "74*regionType=parallel*sscl=mpi_omp_test.c:45:45*escl=mpi_omp_test.c:49:49**" );
}
