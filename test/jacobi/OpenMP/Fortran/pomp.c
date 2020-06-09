#include <config.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
F77_FUNC_( pomp2_init_reg_8j6lstatlxpyf_3,
           POMP2_INIT_REG_8J6LSTATLXPYF_3 ) ();
extern void
F77_FUNC_( pomp2_init_reg_5j6lstatla52g_1,
           POMP2_INIT_REG_5J6LSTATLA52G_1 ) ();

void
POMP2_Init_regions()
{
    F77_FUNC_( pomp2_init_reg_8j6lstatlxpyf_3, POMP2_INIT_REG_8J6LSTATLXPYF_3 ) ();
    F77_FUNC_( pomp2_init_reg_5j6lstatla52g_1, POMP2_INIT_REG_5J6LSTATLA52G_1 ) ();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

#ifdef __cplusplus
}
#endif
