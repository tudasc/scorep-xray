#include <config.h>
#include <opari2/pomp2_lib.h>

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2)
static POMP2_Region_handle pomp2_region_1 = NULL;
static POMP2_Region_handle pomp2_region_2 = NULL;
#define POMP2_DLIST_00003 shared(pomp2_region_3)
static POMP2_Region_handle pomp2_region_3 = NULL;
extern "C" 
{
void POMP2_Init_reg_1320064211885492_3()
{
    POMP2_Assign_handle( &pomp2_region_1, "219*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/jacobi.cpp:67:67*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/jacobi.cpp:88:88**" );
    POMP2_Assign_handle( &pomp2_region_2, "242*regionType=for*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/jacobi.cpp:72:72*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/jacobi.cpp:87:87*hasReduction=1*hasOrdered=1**" );
    POMP2_Assign_handle( &pomp2_region_3, "226*regionType=parallelfor*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/jacobi.cpp:154:154*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/jacobi.cpp:161:161**" );
}
}
