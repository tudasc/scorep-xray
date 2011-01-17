CXX=g++
CC=gcc
OBJDIR=obj
DEPDIR=.dep
BUILDOPT_FLAGS=-c  -g -O2 
LINK_FLAGS=
REGSRV=oa_regsrv

SRCS = \
	regsrv_client.cc \
	regsrv.cc \
	regsrv_sockets.c \
	stringutil.c \
	strutil.c

REG_OBJS = $(addprefix $(OBJDIR)/, \
	regsrv.o regsrv_client.o stringutil.o regsrv_sockets.o strutil.o)

all : $(REGSRV)

$(REGSRV) :  $(REG_OBJS) 
	$(CXX) $(LINK_FLAGS) -o $@ $^ 


DEP_FLAGS = -MMD -MP

df = $(DEPDIR)/$(*F)

# Include all dependencies
-include $(addsuffix .P, $(addprefix $(DEPDIR)/, $(SRCS)))

$(OBJDIR)/%.o : %.cc
	@mkdir -p $(DEPDIR)/;
	@mkdir -p $(OBJDIR)/;
	$(CXX) $(CXXFLAGS) $(BUILDOPT_FLAGS) $(DEP_FLAGS)  -MF $(df).cc.P -o $@ $<
	@sed -e '1s,\($*\)\.o[ :]*,\1.o $(df).cc.P: ,' $(df).cc.P > $(df).cc.P.temp
	@mv $(df).cc.P.temp $(df).cc.P

$(OBJDIR)/%.o : %.c
	@mkdir -p $(DEPDIR)/;
	@mkdir -p $(OBJDIR)/;
	$(CC) $(CFLAGS) $(BUILDOPT_FLAGS) $(DEP_FLAGS) -MF $(df).c.P -o $@ $<
	@sed -e '1s,\($*\)\.o[ :]*,\1.o $(df).c.P: ,' $(df).c.P > $(df).c.P.temp
	@mv $(df).c.P.temp $(df).c.P
	
clean : 
	@rm -f *.o 
	@rm -f $(REGSRV) 
	@rm -f $(DEPDIR)/*.P	
	@rm -fr $(DEPDIR) $(OBJDIR)


