include ../config.mk

.PHONY: all
all: libbar.la libbar2.la

VPATH = $(SRC_ROOT)/target-lib-cdeps

bar.lo: bar.c bar.h
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I$(SRC_ROOT)/target-lib-c -c -o bar.lo $<

bar2.lo: bar2.c bar.h
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I$(SRC_ROOT)/target-lib-c -c -o bar2.lo $<

libbar.la: bar.lo
	$(LIBTOOL) --tag=CC --mode=link $(CC) -avoid-version -L$$PWD/../target-lib-c/lib -rpath $$PWD/lib -o libbar.la bar.lo -lfoo

libbar2.la: bar2.lo
	$(LIBTOOL) --tag=CC --mode=link $(CC) -avoid-version -L$$PWD/../target-lib-c/lib -rpath $$PWD/lib -o libbar2.la bar2.lo -lfoo

.PHONY: install
install: all
	@mkdir -p $$PWD/lib
	$(LIBTOOL) --mode=install $(INSTALL) libbar.la $$PWD/lib/libbar.la
	$(LIBTOOL) --mode=install $(INSTALL) libbar2.la $$PWD/lib/libbar2.la

.PHONY: uninstall
uninstall:
	$(LIBTOOL) --mode=uninstall rm -f $$PWD/lib/libbar.la
	$(LIBTOOL) --mode=uninstall rm -f $$PWD/lib/libbar2.la
	-rmdir $$PWD/lib

.PHONY: clean
clean:
	$(LIBTOOL) --mode=clean rm -f bar.o bar2.o bar.lo bar2.lo libbar.la libbar2.la
