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


#USE_HDF5=TRUE
USE_HDF5=FALSE

ECHO = /bin/echo

CC := cc  # sets the C-compiler

GSL_DIR := $(shell gsl-config --prefix)
GSL_INCL := $(shell gsl-config --cflags)
GSL_LIBS := $(shell gsl-config --libs)
GSL_LIBDIR := $(GSL_DIR)/lib

HDF5INCL = -I/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/include
HDF5LIB = -L/usr/local/x86_64/gnu/hdf5-1.8.17-openmpi-1.10.2-psm/lib

OPTIMIZE = -g -O0 -Wall -Werror # optimization and warning flags

CFLAGS = $(OPTIMIZE) $(GSL_INCL) $(OPTS) 

ifeq ($(USE_HDF5),TRUE)
	$(ECHO) "Compiling with HDF5" 
	CFLAGS += $(HDF5INCL)
	LIBS  += -g -lm  $(GSL_LIBS) -lgsl -lgslcblas $(HDF5LIB) -lhdf5 
	OPTS += -DUSE_HDF5
else
	LIBS  += -g -lm  $(GSL_LIBS) -lgsl -lgslcblas 
endif



all: $(EXECS)

sum_densfield: $(OBJS) densfield_sum.o
	$(CC) $(CFLAGS) $^ $(LIBS) -Xlinker -rpath -Xlinker $(GSL_LIBDIR) -o  $@

grid_normalize: normalize_grid.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -Xlinker -rpath -Xlinker $(GSL_LIBDIR) -o  $@

clean:
	rm -f $(OBJS) $(EXECS_OBJS) $(EXECS)

.PHONY: all clean clena celan celna

celan celna clena claen:clean
