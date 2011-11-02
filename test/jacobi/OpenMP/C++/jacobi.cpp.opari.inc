#include <config.h>
#include <pomp2_lib.h>

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
static POMP2_Region_handle pomp2_region_3 = NULL;
extern "C" 
{
void POMP2_Init_regions_1320065783274680_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "219*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/OpenMP/C++/jacobi.cpp:57:57*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/OpenMP/C++/jacobi.cpp:87:87**" );
    POMP2_Assign_handle( &pomp2_region_2, "214*regionType=for*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/OpenMP/C++/jacobi.cpp:59:59*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/OpenMP/C++/jacobi.cpp:66:66**" );
    POMP2_Assign_handle( &pomp2_region_3, "242*regionType=for*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/OpenMP/C++/jacobi.cpp:71:71*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/OpenMP/C++/jacobi.cpp:86:86*hasReduction=1*hasOrdered=1**" );
}
}
