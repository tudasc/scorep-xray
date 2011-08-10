all: clean
	doxygen specification.cfg
	$(MAKE) -C specification/latex
	mv specification/latex/refman.pdf specification.pdf

.PHONY: clean
clean:
	rm -rf specification specification.pdf
