#ifndef IO_HDF5
#define IO_HDF5

#include <stdint.h>
#include <hdf5.h>

extern int
read_attribute_int(char *file_name, char *groupname, char *attr_name, void *data);

extern int
read_attribute_int_array(char *file_name, char *groupname, char *attr_name, void *data, int32_t array_size);

extern int 
read_attribute_double(char *file_name, char *groupname, char *attr_name, void *data);

extern int 
read_attribute_double_array(char *file_name, char *groupname, char *attr_name, void *data, int32_t array_size);

extern int 
read_dataset_long(char *file_name, char *dataset_name, int dim, int *offset_in, int *count_in, void *data_out);

extern int 
read_dataset_float(char *file_name, char *dataset_name, int dim, int *offset_in, int *count_in, void *data_out);

#endif
