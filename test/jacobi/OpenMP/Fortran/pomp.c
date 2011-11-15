#include <config.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void
FORTRAN_MANGLED( pomp2_init_regions_1320070048701483_3 ) ();
extern void
FORTRAN_MANGLED( pomp2_init_regions_1320070039548462_1 ) ();

void
POMP2_Init_regions()
{
    FORTRAN_MANGLED( pomp2_init_regions_1320070048701483_3 ) ();
    FORTRAN_MANGLED( pomp2_init_regions_1320070039548462_1 ) ();
}

size_t
POMP2_Get_num_regions()
{
    return 4;
}

const char*
POMP2_Get_opari2_version()
{
    return "0.9";
}

#ifdef __cplusplus
}
#endif
