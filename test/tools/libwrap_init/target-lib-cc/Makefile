include ../config.mk

.PHONY: all
all: libfoo.la

VPATH = $(SRC_ROOT)/target-lib-cc

foo.lo: foo.cc foo.h
	$(LIBTOOL) --tag=CXX --mode=compile $(CXX) -c -o foo.lo $<

libfoo.la: foo.lo
	$(LIBTOOL) --tag=CXX --mode=link $(CXX) -avoid-version -rpath $$PWD/lib -o libfoo.la foo.lo

.PHONY: install
install: all
	@mkdir -p $$PWD/lib
	$(LIBTOOL) --mode=install $(INSTALL) libfoo.la $$PWD/lib/libfoo.la

.PHONY: uninstall
uninstall:
	$(LIBTOOL) --mode=uninstall rm -f $$PWD/lib/libfoo.la
	-rmdir $$PWD/lib

.PHONY: clean
clean:
	$(LIBTOOL) --mode=clean rm -f foo.o foo.lo libfoo.la
