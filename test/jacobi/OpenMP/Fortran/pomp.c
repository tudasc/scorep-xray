#include <config.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
FORTRAN_MANGLED( pomp2_init_reg_8j6lstatlxpyf_3 ) ();
extern void
FORTRAN_MANGLED( pomp2_init_reg_5j6lstatla52g_1 ) ();

void
POMP2_Init_regions()
{
    FORTRAN_MANGLED( pomp2_init_reg_8j6lstatlxpyf_3 ) ();
    FORTRAN_MANGLED( pomp2_init_reg_5j6lstatla52g_1 ) ();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

#ifdef __cplusplus
}
#endif
