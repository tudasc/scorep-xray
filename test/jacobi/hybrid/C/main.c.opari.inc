#include <config.h>
#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_reg_1319703209222631_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "354*regionType=parallelfor*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/main.c:198:198*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/main.c:213:213**" );
}
