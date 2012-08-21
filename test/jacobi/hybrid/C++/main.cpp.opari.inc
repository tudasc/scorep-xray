#include <config.h>
#include <opari2/pomp2_lib.h>

extern int64_t FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
extern "C" 
{
void POMP2_Init_reg_1320063929619524_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "222*regionType=parallelfor*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/main.cpp:186:186*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/trunk/test/jacobi/hybrid/C++/main.cpp:201:201**" );
}
}
