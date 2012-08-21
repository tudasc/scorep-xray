#include <config.h>
#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
static POMP2_Region_handle pomp2_region_3 = NULL;

void POMP2_Init_reg_1319702462535739_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "351*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/jacobi.c:61:61*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/jacobi.c:90:90**" );
    POMP2_Assign_handle( &pomp2_region_2, "346*regionType=for*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/jacobi.c:63:63*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/jacobi.c:70:70**" );
    POMP2_Assign_handle( &pomp2_region_3, "374*regionType=for*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/jacobi.c:74:74*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root-4-merge/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/services/metric/data/jacobi/OpenMP/C/jacobi.c:89:89*hasReduction=1*hasOrdered=1**" );
}
