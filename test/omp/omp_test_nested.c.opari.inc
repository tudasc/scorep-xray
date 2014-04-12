#include <opari2/pomp2_lib.h>

#include <stdint.h>
extern int64_t __attribute__((aligned (16))) FORTRAN_MANGLED( pomp_tpd );
#ifdef _OPENMP
#pragma omp threadprivate(FORTRAN_MANGLED( pomp_tpd ))
#endif
#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;
#define pomp2_ctc_1 "239*regionType=parallel*sscl=omp_test_nested.c:75:75*escl=omp_test_nested.c:87:87*hasNumThreads=1**"
#define POMP2_DLIST_00002 shared(pomp2_region_2)
static POMP2_Region_handle pomp2_region_2 = NULL;
#define pomp2_ctc_2 "239*regionType=parallel*sscl=omp_test_nested.c:78:78*escl=omp_test_nested.c:86:86*hasNumThreads=1**"
#define POMP2_DLIST_00003 shared(pomp2_region_3,pomp2_region_4)
static POMP2_Region_handle pomp2_region_3 = NULL;
#define pomp2_ctc_3 "239*regionType=parallel*sscl=omp_test_nested.c:81:81*escl=omp_test_nested.c:85:85*hasNumThreads=1**"
static POMP2_Region_handle pomp2_region_4 = NULL;
#define pomp2_ctc_4 "221*regionType=atomic*sscl=omp_test_nested.c:83:83*escl=omp_test_nested.c:84:84**"
#define POMP2_DLIST_00005 shared(pomp2_region_5)
static POMP2_Region_handle pomp2_region_5 = NULL;
#define pomp2_ctc_5 "241*regionType=parallel*sscl=omp_test_nested.c:94:94*escl=omp_test_nested.c:103:103*hasNumThreads=1**"
#define POMP2_DLIST_00006 shared(pomp2_region_6,pomp2_region_7)
static POMP2_Region_handle pomp2_region_6 = NULL;
#define pomp2_ctc_6 "241*regionType=parallel*sscl=omp_test_nested.c:97:97*escl=omp_test_nested.c:102:102*hasNumThreads=1**"
static POMP2_Region_handle pomp2_region_7 = NULL;
#define pomp2_ctc_7 "225*regionType=atomic*sscl=omp_test_nested.c:100:100*escl=omp_test_nested.c:101:101**"
#define POMP2_DLIST_00008 shared(pomp2_region_8)
static POMP2_Region_handle pomp2_region_8 = NULL;
#define pomp2_ctc_8 "243*regionType=parallel*sscl=omp_test_nested.c:110:110*escl=omp_test_nested.c:118:118*hasNumThreads=1**"
#define POMP2_DLIST_00009 shared(pomp2_region_9,pomp2_region_10)
static POMP2_Region_handle pomp2_region_9 = NULL;
#define pomp2_ctc_9 "243*regionType=parallel*sscl=omp_test_nested.c:113:113*escl=omp_test_nested.c:117:117*hasNumThreads=1**"
static POMP2_Region_handle pomp2_region_10 = NULL;
#define pomp2_ctc_10 "225*regionType=atomic*sscl=omp_test_nested.c:115:115*escl=omp_test_nested.c:116:116**"

#ifdef __cplusplus
 extern "C"
#endif
void POMP2_Init_reg_5lg6q5kdvs27_10()
{
    POMP2_Assign_handle( &pomp2_region_1, pomp2_ctc_1 );
    POMP2_Assign_handle( &pomp2_region_2, pomp2_ctc_2 );
    POMP2_Assign_handle( &pomp2_region_3, pomp2_ctc_3 );
    POMP2_Assign_handle( &pomp2_region_4, pomp2_ctc_4 );
    POMP2_Assign_handle( &pomp2_region_5, pomp2_ctc_5 );
    POMP2_Assign_handle( &pomp2_region_6, pomp2_ctc_6 );
    POMP2_Assign_handle( &pomp2_region_7, pomp2_ctc_7 );
    POMP2_Assign_handle( &pomp2_region_8, pomp2_ctc_8 );
    POMP2_Assign_handle( &pomp2_region_9, pomp2_ctc_9 );
    POMP2_Assign_handle( &pomp2_region_10, pomp2_ctc_10 );
}
