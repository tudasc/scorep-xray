#include <config.h>
#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3,pomp2_region_4)
static POMP2_Region_handle pomp2_region_3 = NULL;
static POMP2_Region_handle pomp2_region_4 = NULL;

void POMP2_Init_reg_1318332431780061_4()
{
    POMP2_Assign_handle( &pomp2_region_1, "201*regionType=task*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:48:48*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:51:51**" );
    POMP2_Assign_handle( &pomp2_region_2, "203*regionType=taskwait*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:53:53*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:0:0**" );
    POMP2_Assign_handle( &pomp2_region_3, "205*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:62:62*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:66:66**" );
    POMP2_Assign_handle( &pomp2_region_4, "203*regionType=single*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:64:64*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/omp_tasks/omp_tasks.c:65:65**" );
}
