#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

CXX             = g++
CFLAGS          = -g `xml2-config --cflags`
CXXFLAGS        = -g `xml2-config --cflags`
LIBS            = `xml2-config --libs`

WRAPGEN         = ./wrapgen
WFLAGS          = -b -p skel/SILC_Mpi_Prototypes.xml skel/SILC_Mpi_Header.txt 
INSTALL         = /usr/bin/install

DOXYGEN         = doxygen
DOXYGEN_CONF    = dox_mpi_wrapper.cfg
DOXYGEN_OPTS    = 

MODS            = SILC_Wrapgen_Funcparam.o SILC_Wrapgen_Func.o SILC_Wrapgen_MpiFunc.o SILC_Wrapgen_Util.o SILC_Wrapgen_Handlers.o SILC_Wrapgen_HandlersMpi.o 

OUTPUTDIR       = generated
INSTALLDIR      = ../../../src/adapters/mpi/

GENCORE         = skel/SILC_Mpi_Prototypes.xml

FILES           = $(patsubst skel/%.tmpl,%,$(wildcard skel/*.tmpl))


all: protogen wrapgen all-files

install: all install-files

clean: clean-files
	rm -f *.o wrapgen

doc: all-files $(DOXYGEN_CONF)
	$(DOXYGEN) $(DOXYGEN_OPTS) $(DOXYGEN_CONF)

wrapgen : SILC_Wrapgen.o $(MODS)
	$(CXX) $(CXXFLAGS) $(LIBS) $(MODS) SILC_Wrapgen.o -o $@

SILC_Wrapgen_Funcparam.o : SILC_Wrapgen_Funcparam.h SILC_Wrapgen_Funcparam.cpp 
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SILC_Wrapgen_Func.o : SILC_Wrapgen_Func.h SILC_Wrapgen_Func.cpp SILC_Wrapgen_Util.h SILC_Wrapgen_Util.cpp SILC_Wrapgen_Funcparam.h \
        SILC_Wrapgen_Funcparam.cpp
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SILC_Wrapgen_MpiFunc.o : SILC_Wrapgen_MpiFunc.h SILC_Wrapgen_MpiFunc.cpp SILC_Wrapgen_Util.h SILC_Wrapgen_Util.cpp \
        SILC_Wrapgen_Funcparam.h SILC_Wrapgen_Funcparam.cpp
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SILC_Wrapgen_Util.o : SILC_Wrapgen_Util.cpp SILC_Wrapgen_Util.h
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SILC_Wrapgen_Handlers.o : SILC_Wrapgen_Handlers.cpp SILC_Wrapgen_Handlers.h SILC_Wrapgen_MpiFunc.h SILC_Wrapgen_Func.h 
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

wrapgen_handlers_mpi.o :SILC_Wrapgen_HandlersMpi.cppSILC_Wrapgen_HandlersMpi.h SILC_Wrapgen_MpiFunc.h SILC_Wrapgen_Func.hSILC_Wrapgen_HandlersMpi_datatypes.h
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SILC_Wrapgen.o : SILC_Wrapgen.cpp SILC_Wrapgen.h SILC_Wrapgen_Help.h
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

# Targets for creating wrapper files

all-files: $(addprefix $(OUTPUTDIR)/,$(FILES))

clean-files:
	if [ -d $(OUTPUTDIR) ]; then (cd $(OUTPUTDIR); rm -f $(FILES)) fi
	if [ -d $(OUTPUTDIR) ] && [ -z `ls $(OUTPUTDIR)` ]; then (rmdir $(OUTPUTDIR)) fi

install-files: all-files
	(cd $(OUTPUTDIR); $(INSTALL) -m 644 $(FILES) $(INSTALLDIR))

# Auto-gerenation targets for MPI wrappers

$(OUTPUTDIR)/SILC_%: $(GENCORE) skel/SILC_%.tmpl skel/*.w
	if [ ! -d $(OUTPUTDIR) ]; then mkdir -p $(OUTPUTDIR); fi
	$(WRAPGEN) $(WFLAGS) skel/$(notdir $@).tmpl > $@

SILC_Wrapgen_HeaderParser.o: SILC_Wrapgen_HeaderParser.cpp
	$(CXX) $(CXXFLAGS) SILC_Wrapgen_HeaderParser.cpp  -c -o SILC_Wrapgen_HeaderParser.o

protogen: SILC_Wrapgen_HeaderParser.o $(MODS)
	$(CXX) $(CXXFLAGS) $(LIBS) $(MODS) SILC_Wrapgen_HeaderParser.o -o protogen
