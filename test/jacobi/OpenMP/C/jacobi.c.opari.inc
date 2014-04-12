#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED(pomp_tpd);
#pragma omp threadprivate(FORTRAN_MANGLED(pomp_tpd))
#define POMP2_DLIST_00001 shared(pomp2_region_1,pomp2_region_2,pomp2_region_3)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "239*regionType=parallel*sscl=jacobi.c:59:59*escl=jacobi.c:88:88**"
static POMP2_Region_handle pomp2_region_2 = NULL;
#define pomp2_ctc_2 "234*regionType=for*sscl=jacobi.c:61:61*escl=jacobi.c:68:68**"
static POMP2_Region_handle pomp2_region_3 = NULL;
#define pomp2_ctc_3 "249*regionType=for*sscl=jacobi.c:72:72*escl=jacobi.c:87:87*hasReduction=1**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_ei6lzv6tlg2yd_3()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
}
