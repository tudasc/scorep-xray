#include <config.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
F77_FUNC_( pomp2_init_reg_kh6l759tlp3ei_3,
           POMP2_INIT_REG_KH6L759TLP3EI_3 ) ();
extern void
F77_FUNC_( pomp2_init_reg_hh6l759tl8jhi_1,
           POMP2_INIT_REG_HH6L759TL8JHI_1 ) ();

void
POMP2_Init_regions()
{
    F77_FUNC_( pomp2_init_reg_kh6l759tlp3ei_3, POMP2_INIT_REG_KH6L759TLP3EI_3 ) ();
    F77_FUNC_( pomp2_init_reg_hh6l759tl8jhi_1, POMP2_INIT_REG_HH6L759TL8JHI_1 ) ();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

#ifdef __cplusplus
}
#endif
