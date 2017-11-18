#ifndef IO_HDF5
#define IO_HDF5

#include <hdf5.h>

extern int
read_attribute_int(char *file_name, char *groupname, char *attr_name, void *data);


extern int 
read_attribute_double(char *file_name, char *groupname, char *attr_name, void *data);

#endif
