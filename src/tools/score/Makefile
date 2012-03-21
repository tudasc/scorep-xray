CXX = g++

INC = -I/home/zam/lorenz/local/cube4/trunk/include/cube -I.

LIBS = -L/home/zam/lorenz/local/cube4/trunk/lib -lcube4 -lz -lm

OBJS = SCOREP_Score_Profile.o \
       scorep_score.o \
       SCOREP_Score_Estimator.o \
       SCOREP_Score_Group.o \
       SCOREP_Score_Types.o

all: $(OBJS)
	$(CXX) -o scorep-score $(OBJS) $(LIBS)

%.o: %.cpp Makefile
	$(CXX) -c $(INC) $<
