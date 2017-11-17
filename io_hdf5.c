#include <hdf5.h>
#include <stdlib.h>
#include <assert.h>

#include "io_hdf5.h"

extern int 
read_attribute_int(char *file_name, char *groupname, char *attr_name, void *data) {

    hid_t           file_id;  /* identifiers */
    herr_t          status;

    hid_t           attr_id;
    hid_t           attr_space, attr_type;

    int             attr_rank;
    hsize_t         *attr_dim;

    /* Open an existing file. */
    file_id = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing attribute */
    attr_id = H5Aopen_by_name(file_id, groupname, attr_name, H5P_DEFAULT, H5P_DEFAULT);

    /* Get details on attribute */
    attr_space = H5Aget_space(attr_id);
    attr_type = H5Aget_type(attr_id);
    attr_rank = H5Sget_simple_extent_ndims(attr_space);

    if(attr_rank == 0)
    {
        if(H5T_INTEGER == H5Tget_class(attr_type)){
            status = H5Aread(attr_id, H5T_NATIVE_INT, data);
            if(status<0.){
                fprintf(stderr, "Failed to read attribute in HDF5 file");
                exit(EXIT_FAILURE);
            }
        }
    }
    else if(attr_rank == 1)
    {
        attr_dim = malloc(sizeof(int)*attr_rank);
        if(attr_dim == NULL) {
            fprintf(stderr,  "Failed to read dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        status = H5Sget_simple_extent_dims(attr_space, attr_dim, NULL);
        if(status<0.){
            exit(EXIT_FAILURE);
        }

        if(H5T_INTEGER == H5Tget_class(attr_type))
        {   
            status = H5Aread(attr_id, H5T_NATIVE_INT, data);
            if(status<0.){
                fprintf(stderr, "Failed to read attribute in HDF5 file");
                exit(EXIT_FAILURE);
            }
        }
        free(attr_dim);
    }
    else
    {   
        printf("Not implemented!\n");
    }


    status = H5Tclose(attr_type);
    status = H5Sclose(attr_space);
    status = H5Aclose(attr_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }

    return 1;
}

