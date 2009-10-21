all:
	doxygen specification.cfg
	$(MAKE) -C specification/latex
	mv specification/latex/refman.pdf specification.pdf
