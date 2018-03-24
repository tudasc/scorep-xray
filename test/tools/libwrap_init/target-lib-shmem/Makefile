include ../config-shmem.mk

.PHONY: all
all: libfoo.la

VPATH = $(SRC_ROOT)/target-lib-shmem

foo.lo: foo.c foo.h
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -c -o foo.lo $<

libfoo.la: foo.lo
	$(LIBTOOL) --tag=CC --mode=link $(CC) -avoid-version -rpath $$PWD/lib -o libfoo.la foo.lo

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
