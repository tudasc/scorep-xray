CXX = g++

UTIL_INC = -I../../../vendor/otf2/vendor/utility/include
UTIL_LIB = -L../../../gcc-mpich2/vendor/otf2/vendor/utility/build-backend/.libs \
           -lscorep_utilities

FILTER_INC = -I../../utils/include
FILTER_LIB = -L../../../gcc-mpich2/build-backend/.libs -lscorep_filter

INC = -I/home/zam/lorenz/local/cube4/trunk/include/cube -I. $(UTIL_INC) $(FILTER_INC)

LIBS = -L/home/zam/lorenz/local/cube4/trunk/lib -lcube4 -lz -lm $(UTIL_LIB) $(FILTER_LIB)

OBJS = SCOREP_Score_Profile.o \
       scorep_score.o \
       SCOREP_Score_Estimator.o \
       SCOREP_Score_Group.o \
       SCOREP_Score_Types.o

all: $(OBJS)
	$(CXX) -o scorep-score $(OBJS) $(LIBS)

%.o: %.cpp *.hpp Makefile
	$(CXX) -c $(INC) $<
