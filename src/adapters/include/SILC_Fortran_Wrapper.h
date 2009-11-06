/** @file SILC_Fortran_Wrapper.h
    @author Daniel Lorenz

    This file provides macros to generate decoration for function names for C-functions
    called from Fortran.
 */

/** @def SILC_FORTRAN_UPCASE
    Appends '_U' to the name. With generator tools definies can be generated to exchange
    the name_U by all upcase letters.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_UPCASE( name ) name ## _U

/** @def SILC_FORTRAN_LOWCASE
    Appends '_L' to the name. With generator tools definies can be generated to exchange
    the name_L by all lowcase letters.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_LOWCASE( name ) name ## _L

/** @def SILC_FORTRAN_SUFFIX
    Appends '_' to the name.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUFFIX( name ) name ## _

/** @def SILC_FORTRAN_SUFFIX2
    Appends '__' to the name.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUFFIX2( name ) name ## __

/** @def SILC_FORTRAN_SUB1(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in upcase.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB1( name )  SILC_FORTRAN_UPCASE( name )

/** @def SILC_FORTRAN_SUB2(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB2( name )  SILC_FORTRAN_LOWCASE( name )

/** @def SILC_FORTRAN_SUB3(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '_'.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB3( name )  SILC_FORTRAN_SUFFIX( SILC_FORTRAN_LOWCASE( name ) )

/** @def SILC_FORTRAN_SUB4(name)
    Defines the first possibility to decorate C-functions called from Fortran. It puts all
    letters in lowcase and appends '__'.
    @param name The name that gets decorated.
 */
#define SILC_FORTRAN_SUB4( name )  SILC_FORTRAN_SUFFIX2( SILC_FORTRAN_LOWCASE( name ) )
