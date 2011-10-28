#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3,pomp2_region_4)
static POMP2_Region_handle pomp2_region_3 = NULL;
static POMP2_Region_handle pomp2_region_4 = NULL;

void POMP2_Init_regions_1318332431780061_4()
{
    POMP2_Assign_handle( &pomp2_region_1, "283*regionType=task*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:48:48*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:51:51**" );
    POMP2_Assign_handle( &pomp2_region_2, "285*regionType=taskwait*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:53:53*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:0:0**" );
    POMP2_Assign_handle( &pomp2_region_3, "287*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:62:62*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:66:66**" );
    POMP2_Assign_handle( &pomp2_region_4, "285*regionType=single*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:64:64*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:65:65**" );
}
