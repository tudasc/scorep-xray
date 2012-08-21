#include <config.h>
#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_reg_1320069835786106_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "191*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp/omp_test.c:49:49*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp/omp_test.c:52:52**" );
}
