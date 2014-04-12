#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern "C"
{
extern int64_t FORTRAN_MANGLED(pomp_tpd);
}
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "250*regionType=parallelfor*sscl=main.cpp:130:130*escl=main.cpp:145:145**"
extern "C"
{
void POMP2_Init_reg_qi6lq4btlls4l_1()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
}
}
