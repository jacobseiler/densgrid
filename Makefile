EXECS   := densgrid 

OBJS   := 	./main.o \
			./grid.o \
			./io.o	\
			./io_hdf5.o \
			./particles.o

INCL   :=	./grid.h \
			./io.h \
			./io_hdf5.h \
			./particles.h \
			./Makefile
 
USE_MPI=TRUE
#USE_MPI=FALSE

USE_HDF5=TRUE
#USE_HDF5=FALSE

ifeq ($(USE_MPI),TRUE)
    OPTS = -DMPI  #  This creates an MPI version that can be used to process files in parallel
    CC := mpicc  # sets the C-compiler
	LIBS := -lmpi
else
    CC = cc  # sets the C-compiler
	OPTS = 
endif

ifeq ($(USE_HDF5),TRUE)
    OPTS = -DUSE_HDF5 #  This creates an MPI version that can be used to process files in parallel
	LIBS += $(HDF5LIB) -lhdf5
	CFLAGS += $(HDF5INCL)  
else

endif


GSL_DIR := $(shell gsl-config --prefix)
GSL_INCL := $(shell gsl-config --cflags)
GSL_LIBS := $(shell gsl-config --libs)
GSL_LIBDIR := $(GSL_DIR)/lib

HDF5INCL := -I/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/include
HDF5LIB := -L/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/lib

OPTIMIZE = -g -O0 -Wall -Werror # optimization and warning flags

OPTS += -DBRITTON_SIM #-DDEBUG_PARTBUFFER #-DDEBUG_HDF5 

LIBS   += -g -lm  $(GSL_LIBS) -lgsl -lgslcblas 

CFLAGS =   $(OPTIONS) $(OPT) $(OPTIMIZE) $(GSL_INCL) $(OPTS) 

all: $(EXECS)
	@if [ "$(USE_MPI)" = "TRUE" ]; then echo "RUNNING WITH MPI"; else echo "MPI DISABLED"; fi
	@if [ "$(USE_HDF5)" = "TRUE" ]; then echo "RUNNING WITH HDF5"; else echo "HDF5 DISABLED"; fi

densgrid: $(OBJS)
	$(CC) $(CCFLAGS) $^ $(LIBS) -Xlinker -rpath -Xlinker $(GSL_LIBDIR) -o  $@ 

clean:
	rm -f $(OBJS) $(EXECS)

.PHONY: all clean clena celan celna

celan celna clena claen:clean
	
