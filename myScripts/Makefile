EXEC   = sum_densfield 

OBJS   = 	./densfield_sum.o \
			../grid.o \
			../io.o	
		
INCL   =	../grid.h \
			../io.h \
			./Makefile
 
CC = cc  # sets the C-compiler

GSL_INCL = -I/usr/local/include  # make sure your system knows where GSL_DIR is
GSL_LIBS = -L/usr/local/lib
HDF5INCL = -I/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/include
HDF5LIB = -L/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/lib

OPTIMIZE = -g -O0 -Wall  # optimization and warning flags

OPTS += 

LIBS   +=   -g -lm  $(GSL_LIBS) -lgsl -lgslcblas $(HDF5LIB) -lhdf5 

CFLAGS =   $(OPTIONS) $(OPT) $(OPTIMIZE) $(GSL_INCL) $(HDF5INCL) $(OPTS) 

default: all

$(EXEC): $(OBJS) 
	$(CC) $(OPTIMIZE) $(OBJS) $(LIBS)   -o  $(EXEC) 

$(OBJS): $(INCL) 

clean:
	rm -f $(OBJS)

tidy:
	rm -f $(OBJS) ./$(EXEC)

.PHONY: all clean clena celan celna

celan celna clena claen:clean

all:  tidy $(EXEC) clean


