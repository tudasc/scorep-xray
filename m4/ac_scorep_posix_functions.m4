## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


## file       ac_scorep_posix_functions.m4

AC_DEFUN([AC_SCOREP_POSIX_FUNCTIONS], [

    ##
    ## check function declarations
    ##

    AC_LANG_PUSH(C)

    ## check whether functions are declared and set HAVE_DECL_* appropriately
    AC_CHECK_DECLS([realpath, gethostid, gethostname, fileno, fseeko, fseeko64, getcwd, read, close], [], [], [[
      #include <unistd.h>
      #include <stdio.h>
      #include <stdlib.h>
    ]])

    ##
    ## try to link
    ##
    has_getrlimit_func="yes"
    AC_MSG_CHECKING([for getrlimit])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <sys/resource.h>

            #if !HAVE_DECL_GETRLIMIT
            int getrlimit(int resource, struct rlimit *rlp);
            #endif
            int main()
            {
                struct rlimit res_nofile;
                int res = getrlimit( RLIMIT_NOFILE, &res_nofile );
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_GETRLIMIT, 1, [Can link a getrlimit function])],
        [AC_MSG_RESULT(no)
         has_getrlimit_func="no"]
    ) # AC_LINK_IF_ELSE

    has_realpath_func="yes"
    AC_MSG_CHECKING([for realpath])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdlib.h>

            #if !HAVE_DECL_REALPATH
            char *realpath(const char *path, char *resolved_path);
            #endif

            int main()
            {
                const char * path = "/tmp";
                char * resolved_path = realpath( path, NULL );
                free( resolved_path );
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_REALPATH, 1, [Can link a realpath function])],
        [AC_MSG_RESULT(no)
         has_realpath_func="no"]
    ) # AC_LINK_IF_ELSE

    has_gethostid_func="yes"
    AC_MSG_CHECKING([for gethostid])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stddef.h>

            #if !HAVE_DECL_GETHOSTID
            long gethostid(void);
            #endif

            int main()
            {
                long host_id = gethostid();
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_GETHOSTID, 1, [Can link a gethostid function])],
        [AC_MSG_RESULT(no)
         has_gethostid_func="no"]
    ) # AC_LINK_IF_ELSE

    has_gethostname_func="yes"
    AC_MSG_CHECKING([for gethostname])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stddef.h>

            #if !HAVE_DECL_GETHOSTNAME
            int gethostname(char *name, size_t namelen);
            #endif

            int main()
            {
                char* name;
                return gethostname(name, 256);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_GETHOSTNAME, 1, [Can link a gethostname function])],
        [AC_MSG_RESULT(no)
         has_gethostname_func="no"]
    ) # AC_LINK_IF_ELSE

    has_fileno_func="yes"
    AC_MSG_CHECKING([for fileno])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>
            #include <fcntl.h>

            #if !HAVE_DECL_FILENO
            int fileno(FILE *stream);
            #endif

            int main()
            {
                FILE* stream;
                return fileno(stream);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_FILENO, 1, [Can link a fileno function])],
        [AC_MSG_RESULT(no)
         has_fileno_func="no"]
    ) # AC_LINK_IF_ELSE

    has_fseeko_func="yes"
    AC_MSG_CHECKING([for fseeko])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>
            #include <fcntl.h>

            #if !HAVE_DECL_FSEEKO
            int fseeko(FILE *stream, off_t offset, int whence);
            #endif

            int main()
            {
                FILE* stream;
                return fseeko(stream, 256, 0);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_FSEEKO, 1, [Can link a fseeko function])],
        [AC_MSG_RESULT(no)
         has_fseeko_func="no"]
    ) # AC_LINK_IF_ELSE

    has_fseeko64_func="yes"
    AC_MSG_CHECKING([for fseeko64])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>
            #include <unistd.h>
            #include <fcntl.h>

            #if !HAVE_DECL_FSEEKO64
            int fseeko64(FILE *stream, off64_t offset, int whence);
            #endif

            int main()
            {
                FILE* stream;
                return fseeko64(stream, 256lu, 0);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_FSEEKO64, 1, [Can link a fseeko64 function])],
        [AC_MSG_RESULT(no)
         has_fseeko64_func="no"]
    ) # AC_LINK_IF_ELSE

    has_getcwd_func="yes"
    AC_MSG_CHECKING([for getcwd])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>
            #include <unistd.h>
            #include <stdlib.h>

            #if !HAVE_DECL_GETCWD
            char* getcwd(char* buf, size_t size);
            #endif

            int main()
            {
                char* cwd = getcwd(NULL, 0);
                return (cwd == NULL ? EXIT_FAILURE : EXIT_SUCCESS);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_GETCWD, 1, [Can link a getcwd function])],
        [AC_MSG_RESULT(no)
         has_getcwd_func="no"]
    ) # AC_LINK_IF_ELSE

    has_read_func="yes"
    AC_MSG_CHECKING([for read])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdlib.h>
            #include <unistd.h>

            #if !HAVE_DECL_READ
            ssize_t read(int fd, void *buf, size_t count);
            #endif

            int main()
            {
                ssize_t result = read(0, NULL, 0);
                return EXIT_SUCCESS;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_READ, 1, [Can link a read function])],
        [AC_MSG_RESULT(no)
         has_read_func="no"]
    ) # AC_LINK_IF_ELSE

    has_close_func="yes"
    AC_MSG_CHECKING([for close])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdlib.h>
            #include <unistd.h>

            #if !HAVE_DECL_CLOSE
            int close(int fd);
            #endif

            int main()
            {
                int result = close(0);
                return EXIT_SUCCESS;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_CLOSE, 1, [Can link a close function])],
        [AC_MSG_RESULT(no)
         has_close_func="no"]
    ) # AC_LINK_IF_ELSE

    AC_LANG_POP(C)

    ##
    ## result
    ##
    AM_CONDITIONAL([HAVE_GETRLIMIT], [test "x${has_getrlimit_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_REALPATH], [test "x${has_realpath_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_GETHOSTID], [test "x${has_gethostid_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_GETHOSTNAME], [test "x${has_gethostname_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_FILENO], [test "x${has_fileno_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_FSEEKO], [test "x${has_fseeko_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_FSEEKO64], [test "x${has_fseeko64_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_GETCWD], [test "x${has_getcwd_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_READ], [test "x${has_read_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_CLOSE], [test "x${has_close_func}" = "xyes"])


    ##
    ## CXX
    ##

    ##
    ## check function declarations
    ##

    AC_LANG_PUSH(C++)

    ## check whether functions are declared and set HAVE_DECL_* appropriately
    AC_CHECK_DECLS([gethostid, gethostname, fileno, fseeko, fseeko64, getcwd, read, close], [], [], [[
      #include <stdio.h>
      #include <unistd.h>
    ]])

    ##
    ## try to link
    ##

    has_popen_func="yes"
    AC_MSG_CHECKING([for popen and pclose])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>

            int main()
            {
	        FILE *fh = popen( "echo test", "r" );
                pclose( fh );
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_POPEN, 1, [Can link against popen and pclose])],
        [AC_MSG_RESULT(no)
         has_popen_func="no"]
    ) # AC_LINK_IF_ELSE

    AC_LANG_POP(C++)

    ##
    ## result
    ##

    AM_CONDITIONAL([HAVE_POPEN], [test "x${has_popen_func}" = "xyes"])
])
