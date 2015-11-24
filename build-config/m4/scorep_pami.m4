dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013, 2015,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_DEFUN([SCOREP_PAMI], [
dnl Location and names of pami files unfortunatly not fixed. Search in all
dnl known locations. See
dnl http://www.fz-juelich.de/ias/jsc/EN/Expertise/Supercomputers/JUQUEEN/Documentation/MemoToUsersV1R2M1.html
dnl and http://www.redbooks.ibm.com/redbooks/pdfs/sg247948.pdf

    AS_IF([test "x${ac_scorep_platform}" != "xbgq"],
        [AC_MSG_ERROR([macro SCOREP[]_PAMI called on platform != Blue Gene/Q. This should not happen. Please contact <AC_PACKAGE_BUGREPORT>.])
        ])

    AC_LANG_PUSH([C])

    # check for pami.h
    scorep_cppflags_save="${CPPFLAGS}"
    for scorep_pami_cppflags in "-I/bgsys/drivers/ppcfloor/comm/include" \
                                "-I/bgsys/drivers/ppcfloor/comm/sys/include -I/bgsys/drivers/ppcfloor/comm/sys-fast/include"; do
        CPPFLAGS="${scorep_pami_cppflags}"
        AC_CHECK_HEADER([pami.h],
            [scorep_pami_h_found="yes"
             AC_SUBST([SCOREP_PAMI_CPPFLAGS], ["${scorep_pami_cppflags}"])
             break
            ],
            [scorep_pami_h_found="no"])
    done
    CPPFLAGS="${scorep_cppflags_save}"
    AS_UNSET([scorep_cppflags_save])
    AS_IF([test "x${scorep_pami_h_found}" = "xno"],
        [# providing --with-pami-* seems to be overkill. This is a system header
         # that will reside in one of a few fixed directories.
         AC_MSG_ERROR([pami.h header not found. Please search for pami.h under /bgsys/drivers/ppcfloor/. Please provide your search result to <AC_PACKAGE_BUGREPORT>. You will be provided with an updated Score-P version in a timely manner. Thanks.])
        ])

    # check for -lpami or -lpami-gcc.
    scorep_ldflags_save="${LDFLAGS}"
    scorep_pami_ldflags_common="-L/bgsys/drivers/ppcfloor/spi/lib"
    for scorep_pami_ldflags in "-L/bgsys/drivers/ppcfloor/comm/lib" \
                               "-L/bgsys/drivers/ppcfloor/comm/sys-fast/lib"; do
        for scorep_pami_lib in "pami-gcc" "pami"; do
            LDFLAGS="${scorep_pami_ldflags} ${scorep_pami_ldflags_common}"
            for need_pthread in "" "-pthread"; do
                scorep_pami_additional_libs="-lstdc++ -lSPI -lSPI_cnk -lrt ${need_pthread}"
                AC_CHECK_LIB([${scorep_pami_lib}],
                    [PAMI_Client_create],
                    [scorep_pami_lib_found="yes"
                     AC_SUBST([SCOREP_PAMI_LDFLAGS], ["${scorep_pami_ldflags} ${scorep_pami_ldflags_common}"])
                     AC_SUBST([SCOREP_PAMI_LIBS], ["-l${scorep_pami_lib} ${scorep_pami_additional_libs}"])
                     break
                    ],
                    [scorep_pami_lib_found="no"],
                    [${scorep_pami_additional_libs}])
                # reset cache variable
                AS_UNSET([AS_TR_SH([ac_cv_lib_${scorep_pami_lib}___PAMI_Client_create])])
            done
            AS_IF([test "x${scorep_pami_lib_found}" = "xyes"],
                [break])
        done
        AS_IF([test "x${scorep_pami_lib_found}" = "xyes"],
            [break])
    done
    LDFLAGS="${scorep_ldflags_save}"
    AS_UNSET([scorep_ldflags_save])
    AS_IF([test "x${scorep_pami_lib_found}" = "xno"],
        [# providing --with-pami-* seems to be overkill. This is a system library
         # that will reside in one of a few fixed directories.
         AC_MSG_ERROR([neither -lpami-gcc nor -lpami available. Please search for these libraries under /bgsys/drivers/ppcfloor/. Please provide your search result to <AC_PACKAGE_BUGREPORT>. You will be provided with an updated Score-P version in a timely manner. Thanks.])
        ])
    AC_LANG_POP([C])
    AFS_SUMMARY([libpami], [yes, using ${scorep_pami_ldflags} ${scorep_pami_ldflags_common} -l${scorep_pami_lib} ${scorep_pami_additional_libs}])
])
