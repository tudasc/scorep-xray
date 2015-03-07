#include <config.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
FORTRAN_MANGLED( pomp2_init_reg_kh6l759tlp3ei_3 ) ();
extern void
FORTRAN_MANGLED( pomp2_init_reg_hh6l759tl8jhi_1 ) ();

void
POMP2_Init_regions()
{
    FORTRAN_MANGLED( pomp2_init_reg_kh6l759tlp3ei_3 ) ();
    FORTRAN_MANGLED( pomp2_init_reg_hh6l759tl8jhi_1 ) ();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

#ifdef __cplusplus
}
#endif
