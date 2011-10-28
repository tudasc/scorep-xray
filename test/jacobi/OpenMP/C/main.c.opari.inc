#include <pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) pomp_tpd_;
#pragma omp threadprivate(pomp_tpd_)
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_regions_1319702454368128_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "354*regionType=parallelfor*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/main.c:143:143*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/main.c:158:158**" );
}
