#include <config.h>
#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;

void POMP2_Init_regions_1319703213396018_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "351*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/jacobi.c:67:67*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/jacobi.c:86:86**" );
    POMP2_Assign_handle( &pomp2_region_2, "374*regionType=for*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/jacobi.c:70:70*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/jacobi.c:85:85*hasReduction=1*hasOrdered=1**" );
    POMP2_Assign_handle( &pomp2_region_3, "358*regionType=parallelfor*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/jacobi.c:156:156*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/hybrid/C/jacobi.c:163:163**" );
}
