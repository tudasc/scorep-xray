
## The SUBDIRS variable holds a list of subdirectories automake (recursivly)
## must build.
SUBDIRS = $(subdirs) $(subdirs_custom)

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
ACLOCAL_AMFLAGS = -I build-config/m4

## Still, sometimes there are files that must be distributed, but which are not
## covered in the automatic rules. These files should be listed in the 
## EXTRA_DIST variable. You can mention files from subdirectories in EXTRA_DIST.
## 
## You can also mention a directory in EXTRA_DIST. In this case the entire
## directory will be recursively copied into the distribution. Please note that
## this will also copy everything in the directory, including CVS/RCS version
## control files. We recommend against using this feature.
EXTRA_DIST = $(srcdir)/src/config.h                         \
             $(srcdir)/src/config-custom.h                  \
             $(srcdir)/build-config/VERSION                 \
             $(srcdir)/build-config/process_arguments.awk   \
             $(srcdir)/build-config/platforms/altix         \
             $(srcdir)/build-config/platforms/bgl           \
             $(srcdir)/build-config/platforms/bgp           \
             $(srcdir)/build-config/platforms/crayxt        \
             $(srcdir)/build-config/platforms/ibm           \
             $(srcdir)/build-config/platforms/linux         \
             $(srcdir)/build-config/platforms/sun           \
             $(srcdir)/build-config/platforms/user_provided

if ! CROSS_BUILD
EXTRA_DIST += $(srcdir)/build-frontend/configure.ac \
              $(srcdir)/build-frontend/aclocal.m4   \
              $(srcdir)/build-frontend/Makefile.am  \
              $(srcdir)/build-frontend/Makefile.in  \
              $(srcdir)/build-frontend/configure    \
              $(srcdir)/src/config-frontend.h.in
endif

clean-local: clean-doxygen-user
	rm -rf user_provided_configure_args

## In the following conditional we must distinguish if we are in "developer
## mode", i.e. if we are working on a svn working copy, or if we are in "user
## mode", i.e. we are working on a tarball.
##
## In developer mode we can create doxygen and pdf user documentation (make
## doxygen-user) the will be (automatically) included in the distribution at
## make dist. When creating a distribution, the developer should make sure
## that she has doxygen and related tools e.g. latex in path in order to
## generate a complete documentation, see also
## build-config/m4/ac_silc_doxygen.m4.
##
## make install is ignorant of documentation issues in developer mode.
##
## The user gets ready to install documention in the tarball, i.e. there is no
## need to run doxygen and pdflatex in user mode. In user mode the provided
## documentation is just copied to a subdirectory of $prefix.
if SVN_CONTROLLED #-----------------------------------------------------------

#USER_DOC_DIR = $(top_distdir)/doc
USER_DOC_DIR = $(top_distdir)$(ac_silc_doxygen_distdir)/doc

dist-hook: doxygen-user-pdf
	if test -d "$(USER_DOC_DIR)/html"; then                               \
	    cd "$(USER_DOC_DIR)"; tar czvf html.tar.gz html; rm -rf html/;    \
	fi
	cp -p "$(srcdir)/build-config/REVISION" "$(distdir)/build-config/"
	if grep -E [A-Z] "$(srcdir)/build-config/REVISION" ||                               \
           grep "-" "$(srcdir)/build-config/REVISION"; then                                 \
            echo "WARNING: distribution does not match a single, unmodified revision." >&2; \
	fi
#	rm -rf "$(USER_DOC_DIR)/doxygen"

doxygen-project-number: update-revision
	echo "PROJECT_NUMBER = `cat $(srcdir)/build-config/VERSION` (revision `cat $(srcdir)/build-config/REVISION`)" > doc/doxygen-project-number.cfg

update-revision:
	echo `svnversion $(srcdir)` > "$(srcdir)/build-config/REVISION"

clean-doxygen-user:
	rm -f doc/doxygen-project-number.cfg
	rm -f doc/doxygen-at-make-time.cfg
	rm -rf $(USER_DOC_DIR)/*

if HAVE_DOXYGEN #-----------------------------------------------------

doxygen-user: doxygen-project-number
	$(MKDIR_P) "$(USER_DOC_DIR)/tags"
	echo "OUTPUT_DIRECTORY = $(USER_DOC_DIR)" > doc/doxygen-at-make-time.cfg
	echo "GENERATE_TAGFILE = $(USER_DOC_DIR)/tags/@PACKAGE_NAME@.tag" >> doc/doxygen-at-make-time.cfg
	cat doc/doxygen-user.cfg doc/doxygen-project-number.cfg doc/doxygen-at-make-time.cfg | doxygen -

else # i.e. ! HAVE_DOXYGEN -------------------------------------------

doxygen-user: doxygen-project-number
	echo "WARNING: can not generate doxygen html documentation." >&2

endif # HAVE_DOXYGEN -------------------------------------------------

if HAVE_DOXYGEN_LATEX #-----------------------------------------------

doxygen-user-pdf: doxygen-user
	cd "$(USER_DOC_DIR)/pdf"; \
	make;                     \
	mv refman.pdf ../;        \
	rm -f *;                  \
	mv ../refman.pdf @PACKAGE_NAME@.pdf

else # i.e. ! HAVE_DOXYGEN_LATEX -------------------------------------

doxygen-user-pdf: doxygen-user
	echo "WARNING: can not generate doxygen pdf documentation." >&2
	if test -d "$(USER_DOC_DIR)/pdf"; then rm -rf "$(USER_DOC_DIR)/pdf"; fi

endif # HAVE_DOXYGEN_LATEX -------------------------------------------

else # i.e. ! SVN_CONTROLLED -------------------------------------------------

clean-doxygen-user:

DOC_SRC_DIR = $(srcdir)/doc

install-data-local: 
	if test -f "$(DOC_SRC_DIR)/html.tar.gz"; then                            \
            $(MKDIR_P) $(DESTDIR)$(datadir)/@PACKAGE_NAME@/doc;                  \
            $(INSTALL_DATA) $(DOC_SRC_DIR)/html.tar.gz                           \
                            $(DESTDIR)$(datadir)/@PACKAGE_NAME@/doc/html.tar.gz; \
	    cd $(DESTDIR)$(datadir)/@PACKAGE_NAME@/doc;                          \
            tar xzvf html.tar.gz;                                                \
	    rm -f html.tar.gz;                                                   \
	fi
	if test -d "$(DOC_SRC_DIR)"; then                                                       \
            cd $(DOC_SRC_DIR);                                                                  \
            dirs=`find . -type d`;                                                              \
            cd -;                                                                               \
            for i in $$dirs; do                                                                 \
                $(MKDIR_P) $(DESTDIR)$(datadir)/@PACKAGE_NAME@/doc/$$i;                         \
            done;                                                                               \
	    cd $(DOC_SRC_DIR);                                                                  \
            files=`find . -type f`;                                                             \
            cd -;                                                                               \
            for i in $$files; do                                                                \
                $(INSTALL_DATA) $(DOC_SRC_DIR)/$$i $(DESTDIR)$(datadir)/@PACKAGE_NAME@/doc/$$i; \
            done;                                                                               \
            rm -rf $(DESTDIR)$(datadir)/@PACKAGE_NAME@/doc/doxygen;                             \
        fi

uninstall-local:
	rm -rf $(DESTDIR)$(datadir)/@PACKAGE_NAME@

endif # SVN_CONTROLLED -------------------------------------------------------


##### todo: doxygen-dev ########################################################

# if SVN_CONTROLLED

# if HAVE_DOXYGEN

# doxygen-dev: clean-doxygen-dev doxygen-project-number 
# 	for dir in $(SUBDIRS); do                               \
#             (cd $$dir && $(MAKE) $(AM_MAKEFLAGS) $@) || exit 1; \
#         done
# 	$(MKDIR_P) "$(top_builddir)/doxygen-dev-doc/@PACKAGE_NAME@/tags"
# 	cat doc/doxygen-dev.cfg doc/doxygen-project-number.cfg | doxygen -

# clean-doxygen-dev: clean-doxygen-project-number
# 	for dir in $(SUBDIRS); do                               \
#             (cd $$dir && $(MAKE) $(AM_MAKEFLAGS) $@) || exit 1; \
#         done
# 	rm -rf "$(top_builddir)/doxygen-dev-doc"

# else # HAVE_DOXYGEN

# clean-doxygen-dev:

# endif # HAVE_DOXYGEN

# update-revision:
# 	echo `svnversion $(srcdir)` > $(srcdir)/build-config/REVISION

# clean-local:  clean-doxygen-dev
# 	rm -rf user_provided_configure_args

# else # SVN_CONTROLLED

# endif # SVN_CONTROLLED


if SVN_CONTROLLED #-----------------------------------------------------------
if HAVE_UNCRUSTIFY

beautify:
	for dir in $(SUBDIRS); do                        \
            (cd $$dir && $(MAKE) $(AM_MAKEFLAGS) $@) || exit 1; \
        done
	cd $(srcdir); ./tools/beautifier/beautify

beautify-clean:
	find $(srcdir) -name "*.unc-backup*" -exec rm '{}' \;

endif # HAVE_UNCRUSTIFY
endif # SVN_CONTROLLED -------------------------------------------------------
