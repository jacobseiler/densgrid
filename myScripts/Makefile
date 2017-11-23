EXECS  := sum_densfield grid_normalize 

OBJS  := ../grid.o \
	 ../io.o	\
	 ../io_hdf5.o
EXECS_OBJS := densfield_sum.o \
			  normalize_grid.o
INCL  := ../grid.h \
	../io.h \
	../io_hdf5.h \
	./Makefile

CC := cc  # sets the C-compiler

GSL_DIR := $(shell gsl-config --prefix)
GSL_INCL := $(shell gsl-config --cflags)
GSL_LIBS := $(shell gsl-config --libs)
GSL_LIBDIR := $(GSL_DIR)/lib

HDF5INCL = -I/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/include
HDF5LIB = -L/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/lib

OPTIMIZE = -g -O0 -Wall  # optimization and warning flags

OPTS += 

LIBS  += -g -lm  $(GSL_LIBS) -lgsl -lgslcblas $(HDF5LIB) -lhdf5 

CFLAGS = $(OPTIMIZE) $(GSL_INCL) $(HDF5INCL) $(OPTS) 

all: $(EXECS)

sum_densfield: $(OBJS) densfield_sum.o
	$(CC) $(CFLAGS) $^ $(LIBS) -Xlinker -rpath -Xlinker $(GSL_LIBDIR) -o  $@

grid_normalize: normalize_grid.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -Xlinker -rpath -Xlinker $(GSL_LIBDIR) -o  $@

clean:
	rm -f $(OBJS) $(EXECS_OBJS) $(EXECS)

.PHONY: all clean clena celan celna

celan celna clena claen:clean
