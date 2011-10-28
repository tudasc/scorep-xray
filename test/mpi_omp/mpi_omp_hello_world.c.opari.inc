#include <pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_regions_1311767832126593_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "303*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c:49:49*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/mpi_omp/mpi_omp_hello_world.c:53:53**" );
}
