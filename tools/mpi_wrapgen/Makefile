#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

CXX             = g++
CFLAGS          = -g `xml2-config --cflags` -I.
CXXFLAGS        = -g `xml2-config --cflags` -I.
LIBS            = `xml2-config --libs`

WRAPGEN         = ./wrapgen
WFLAGS          = -b -p skel/SCOREP_Mpi_Prototypes.xml skel/SCOREP_Mpi_Header.txt 
INSTALL         = /usr/bin/install

DOXYGEN         = doxygen
DOXYGEN_CONF    = dox_mpi_wrapper.cfg
DOXYGEN_OPTS    = 

MODS            = SCOREP_Wrapgen_Funcparam.o SCOREP_Wrapgen_Func.o SCOREP_Wrapgen_MpiFunc.o SCOREP_Wrapgen_Util.o SCOREP_Wrapgen_Handlers.o SCOREP_Wrapgen_HandlersMpi.o 

OUTPUTDIR       = generated
INSTALLDIR      = ../../../src/adapters/mpi/

GENCORE         = skel/SCOREP_Mpi_Prototypes.xml

FILES           = $(patsubst skel/%.tmpl,%,$(wildcard skel/*.tmpl))


all: protogen wrapgen all-files

install: all install-files

clean: clean-files
	rm -f *.o wrapgen

doc: all-files $(DOXYGEN_CONF)
	$(DOXYGEN) $(DOXYGEN_OPTS) $(DOXYGEN_CONF)

wrapgen : SCOREP_Wrapgen.o $(MODS)
	$(CXX) $(CXXFLAGS) $(LIBS) $(MODS) SCOREP_Wrapgen.o -o $@

SCOREP_Wrapgen_Funcparam.o : SCOREP_Wrapgen_Funcparam.h SCOREP_Wrapgen_Funcparam.cpp 
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SCOREP_Wrapgen_Func.o : SCOREP_Wrapgen_Func.h SCOREP_Wrapgen_Func.cpp SCOREP_Wrapgen_Util.h SCOREP_Wrapgen_Util.cpp SCOREP_Wrapgen_Funcparam.h \
        SCOREP_Wrapgen_Funcparam.cpp
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SCOREP_Wrapgen_MpiFunc.o : SCOREP_Wrapgen_MpiFunc.h SCOREP_Wrapgen_MpiFunc.cpp SCOREP_Wrapgen_Util.h SCOREP_Wrapgen_Util.cpp \
        SCOREP_Wrapgen_Funcparam.h SCOREP_Wrapgen_Funcparam.cpp
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SCOREP_Wrapgen_Util.o : SCOREP_Wrapgen_Util.cpp SCOREP_Wrapgen_Util.h
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SCOREP_Wrapgen_Handlers.o : SCOREP_Wrapgen_Handlers.cpp SCOREP_Wrapgen_Handlers.h SCOREP_Wrapgen_MpiFunc.h SCOREP_Wrapgen_Func.h 
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

wrapgen_handlers_mpi.o :SCOREP_Wrapgen_HandlersMpi.cppSCOREP_Wrapgen_HandlersMpi.h SCOREP_Wrapgen_MpiFunc.h SCOREP_Wrapgen_Func.hSCOREP_Wrapgen_HandlersMpi_datatypes.h
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

SCOREP_Wrapgen.o : SCOREP_Wrapgen.cpp SCOREP_Wrapgen.h SCOREP_Wrapgen_Help.h
	$(CXX) $(CXXFLAGS) -Wall -c $*.cpp

# Targets for creating wrapper files

all-files: $(addprefix $(OUTPUTDIR)/,$(FILES))

clean-files:
	if [ -d $(OUTPUTDIR) ]; then (cd $(OUTPUTDIR); rm -f $(FILES)) fi
	if [ -d $(OUTPUTDIR) ] && [ -z `ls $(OUTPUTDIR)` ]; then (rmdir $(OUTPUTDIR)) fi

install-files: all-files
	(cd $(OUTPUTDIR); $(INSTALL) -m 644 $(FILES) $(INSTALLDIR))

# Auto-gerenation targets for MPI wrappers

$(OUTPUTDIR)/SCOREP_%: $(GENCORE) skel/SCOREP_%.tmpl skel/*.w
	if [ ! -d $(OUTPUTDIR) ]; then mkdir -p $(OUTPUTDIR); fi
	$(WRAPGEN) $(WFLAGS) skel/$(notdir $@).tmpl > $@

SCOREP_Wrapgen_HeaderParser.o: SCOREP_Wrapgen_HeaderParser.cpp
	$(CXX) $(CXXFLAGS) SCOREP_Wrapgen_HeaderParser.cpp  -c -o SCOREP_Wrapgen_HeaderParser.o

protogen: SCOREP_Wrapgen_HeaderParser.o $(MODS)
	$(CXX) $(CXXFLAGS) $(LIBS) $(MODS) SCOREP_Wrapgen_HeaderParser.o -o protogen
