#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern "C"
{
extern int64_t FORTRAN_MANGLED(pomp_tpd);
}
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "250*regionType=parallelfor*sscl=main.cpp:187:187*escl=main.cpp:202:202**"
extern "C"
{
void POMP2_Init_reg_1h6ldx9tlsrcd_1()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
}
}
