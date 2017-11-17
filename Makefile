EXEC   = densgrid 

OBJS   = 	./main.o \
			./grid.o \
			./io.o	\
			./io_hdf5.o

INCL   =	./grid.h \
			./io.h \
			./io_hdf5.h \
			./Makefile 


ifdef USE-MPI
    OPT += -DMPI  #  This creates an MPI version that can be used to process files in parallel
    CC = mpicc  # sets the C-compiler
else
    CC = cc  # sets the C-compiler
endif

GSL_INCL = -I/usr/local/include  # make sure your system knows where GSL_DIR is
GSL_LIBS = -L/usr/local/lib
HDF5INCL = -I/usr/local/x86_64/gnu/hdf5-1.8.9/include
HDF5LIB = -L/usr/local/x86_64/gnu/hdf5-1.8.9/lib

OPTIMIZE = -g -O0 -Wall  # optimization and warning flags

LIBS   =   -g -lm  $(GSL_LIBS) -lgsl -lgslcblas $(HDF5LIB) -lhdf5 

CFLAGS =   $(OPTIONS) $(OPT) $(OPTIMIZE) $(GSL_INCL) $(HDF5INCL) 

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

