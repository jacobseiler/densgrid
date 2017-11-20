#include <hdf5.h>
#include <stdlib.h>
#include <assert.h>

// This file is taken in full from the Densfield code.
// It contains basic reading functions for hdf5 files.

extern int
get_swap(char *file_name, char *dataset_name) {
    
    hid_t       file_id, dataset_id;
    
    hid_t       datatype;
    
    H5T_order_t order;
    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);

    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    order       = H5Tget_order(datatype);    

    H5Tclose(datatype);

    /* Close the dataset. */
    H5Dclose(dataset_id);

    /* Close the file. */
    H5Fclose(file_id);
    
    return (int)order;
}

extern size_t 
get_byte_size(char *file_name, char *dataset_name) {
    
    hid_t       file_id, dataset_id;  /* identifiers */    
    hid_t       datatype;
    
    size_t      size;
    
    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);

    datatype    = H5Dget_type(dataset_id);     /* datatype handle */;    
    size        = H5Tget_size(datatype);
    
    H5Tclose(datatype);

    /* Close the dataset. */
    H5Dclose(dataset_id);

    /* Close the file. */
    H5Fclose(file_id);
    
    return size;
}

extern int 
get_dataset_rank(char *file_name, char *dataset_name) {
    
    hid_t       file_id, dataset_id;  /* identifiers */
    hid_t       dataspace;
    
    int         rank;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);    
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    
    /* Close the dataset. */
    H5Dclose(dataset_id);

    /* Close the file. */
    H5Fclose(file_id);
    
    return rank;
}

extern hsize_t 
*get_dataset_dims(char *file_name, char *dataset_name) {
    
    hid_t       file_id, dataset_id;  /* identifiers */
    hid_t       dataspace;

    hsize_t     *dims_out;           /* dataset dimensions */    

    int         rank, status_n;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }

    /* Close the dataset. */
    H5Dclose(dataset_id);

    /* Close the file. */
    H5Fclose(file_id);
    
    return dims_out;
}

extern int 
read_dataset_double(char *file_name, char *dataset_name, int dim, int *offset_in, int *count_in, void *data_out) {

    hid_t       file_id, dataset_id;  /* identifiers */
    herr_t      status;
    
    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;           /* dataset dimensions */
    hsize_t     *offset, *count;
    
    hsize_t     *dimsm;           /* dataset dimensions */
        
    int         rank, status_n;
    int         i;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);

    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }
    
    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        offset[i]   = offset_in[i];
        count[i]    = count_in[i];
    }
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    /*
     * Define the memory dataspace.
     */
    dimsm        = malloc(sizeof(int)*rank);
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        dimsm[i] = count_in[i];
    }
    memspace     = H5Screate_simple(rank,dimsm,NULL);
    
    status      = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, memspace, dataspace, H5P_DEFAULT, data_out);
    if(status<0.){
        fprintf(stderr, "Failed to read dataset in HDF5 file");
        exit(EXIT_FAILURE);
    }
        
    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_dataset_float(char *file_name, char *dataset_name, int dim, int *offset_in, int *count_in, void *data_out) {

    hid_t       file_id, dataset_id;  /* identifiers */
    herr_t      status;
    
    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;           /* dataset dimensions */
    hsize_t     *offset, *count;
    
    hsize_t     *dimsm;           /* dataset dimensions */
        
    int         rank, status_n;
    int         i;
    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);


#ifdef DEBUG_HDF5
    printf("Float HDF5 dataset opened\n");
#endif

    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);    
    dims_out    = malloc(sizeof(long int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }
    
#ifdef DEBUG_HDF5
    printf("Dimensions of float HDF5 file gotten\n");
    printf("Rank of data is %d\n", rank);
#endif

    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(long int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(long int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        offset[i]   = offset_in[i];
        count[i]    = count_in[i];
    }
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
    
#ifdef DEBUG_HDF5
    printf("Float HDF5 hyperslab defined\n");
#endif

    /*
     * Define the memory dataspace.
     */
    dimsm        = malloc(sizeof(long int)*rank);
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        dimsm[i] = count_in[i];
#ifdef DEBUG_HDF5
        printf("Count_in[%d] = %d\n", i, count_in[i]);
#endif
    }
    memspace     = H5Screate_simple(rank,dimsm,NULL);

#ifdef DEBUG_HDF5
    printf("Float HDF5 memory dataspace defined.\n");
#endif

    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    
    status      = H5Dread(dataset_id, H5T_NATIVE_FLOAT, memspace, dataspace, H5P_DEFAULT, data_out);
    if(status<0.){
        fprintf(stderr, "Failed to read dataset in HDF5 file");
        exit(EXIT_FAILURE);
    }
 
#ifdef DEBUG_HDF5
    printf("Float HDF5 file fully read.\n");
#endif
       
    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_dataset_long(char *file_name, char *dataset_name, int dim, int *offset_in, int *count_in, void *data_out) {

    hid_t       file_id, dataset_id;  /* identifiers */
    herr_t      status;
    
    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;           /* dataset dimensions */
    hsize_t     *offset, *count;
    
    hsize_t     *dimsm;           /* dataset dimensions */
        
    int         rank, status_n;
    int         i;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

#ifdef DEBUG_HDF5
    printf("Opened the file.\n");
#endif
    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);

#ifdef DEBUG_HDF5
    printf("Dataset Read\n");
#endif

    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(long int)*rank);

#ifdef DEBUG_HDF5
    printf("Allocated a few other things\n");
#endif

    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }

#ifdef DEBUG_HDF5  
    printf("Dimensions gotten\n");
    printf("rank = %d\n", rank); 
#endif

    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(long int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(long int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
   
    for(i=0; i<rank; i++)
    {
        offset[i]   = offset_in[i];
        count[i]    = count_in[i];
    }

#ifdef DEBUG_HDF5
    printf("More mallocs\n");
#endif
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
   
#ifdef DEBUG_HDF5
    printf("Defined hyperslab\n");
#endif
 
    /*
     * Define the memory dataspace.
     */
 
    dimsm        = malloc(sizeof(long int)*rank);
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        dimsm[i] = count_in[i];
    }
    memspace     = H5Screate_simple(rank,dimsm,NULL);
/*
        attr_dim = malloc(sizeof(int)*attr_rank * array_size);
        if(attr_dim == NULL) {
            fprintf(stderr,  "dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        status = H5Sget_simple_extent_dims(attr_space, attr_dim, NULL);
*/
    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    
    status      = H5Dread(dataset_id, H5T_NATIVE_LONG, memspace, dataspace, H5P_DEFAULT, data_out);

#ifdef DEBUG_HDF5
    printf("Hyperslab read\n");
#endif

    if(status<0.){
        fprintf(stderr, "Failed to read dataset in HDF5 file");
        exit(EXIT_FAILURE);
    }
        
    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_dataset_int(char *file_name, char *dataset_name, int dim, int *offset_in, int *count_in, void *data_out) {

    hid_t       file_id, dataset_id;  /* identifiers */
    herr_t      status;
    
    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;           /* dataset dimensions */
    hsize_t     *offset, *count;
    
    hsize_t     *dimsm;           /* dataset dimensions */
        
    int         rank, status_n;
    int         i;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);

    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }
    
    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        offset[i]   = offset_in[i];
        count[i]    = count_in[i];
    }
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    /*
     * Define the memory dataspace.
     */
    dimsm        = malloc(sizeof(int)*rank);
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        dimsm[i] = count_in[i];
    }
    memspace     = H5Screate_simple(rank,dimsm,NULL);

    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    
    status      = H5Dread(dataset_id, H5T_NATIVE_INT, memspace, dataspace, H5P_DEFAULT, data_out);
    if(status<0.){
        fprintf(stderr, "Failed to read dataset in HDF5 file");
        exit(EXIT_FAILURE);
    }
        
    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_item_from_dataset_double(char *file_name, char *dataset_name, int dim, int *number, void *data_out_1D) {

    hid_t       file_id, dataset_id;    /* identifiers */
    herr_t      status;
    int         rank, status_n;

    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;              /* dataset dimensions */
    hsize_t     *offset, *count;
    hsize_t     *dimsm;                 /* memoryspace dimensions */
        
    int         i;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);
    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }
    
    assert(dim == rank);
    
    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        offset[i]   = number[i];
        count[i]    = 1;
    }
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    /*
     * Define the memory dataspace.
     */
    dimsm        = malloc(sizeof(int));
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    dimsm[0] = 1;
    memspace = H5Screate_simple(1, dimsm, NULL);

    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    if(H5T_NATIVE_DOUBLE == H5Tget_class(datatype)){
        status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, memspace, dataspace, H5P_DEFAULT, data_out_1D); 
        if(status<0.){
            fprintf(stderr, "Failed to read dataset in HDF5 file");
            exit(EXIT_FAILURE);
        }
    }

    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_item_from_dataset_float(char *file_name, char *dataset_name, int dim, int *number, void *data_out_1D) {

    hid_t       file_id, dataset_id;    /* identifiers */
    herr_t      status;
    int         rank, status_n;

    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;              /* dataset dimensions */
    hsize_t     *offset, *count;
    hsize_t     *dimsm;                 /* memoryspace dimensions */
        
    int         i;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);
    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }
    
    assert(dim == rank);
    
    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        offset[i]   = number[i];
        count[i]    = 1;
    }
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    /*
     * Define the memory dataspace.
     */
    dimsm        = malloc(sizeof(int));
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    dimsm[0] = 1;
    memspace = H5Screate_simple(1, dimsm, NULL);

    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    if(H5T_FLOAT == H5Tget_class(datatype)){
        status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, memspace, dataspace, H5P_DEFAULT, data_out_1D);   
        if(status<0.){
            fprintf(stderr, "Failed to read dataset in HDF5 file");
            exit(EXIT_FAILURE);
        }
    }

    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_item_from_dataset_long(char *file_name, char *dataset_name, int dim, int *number, void *data_out_1D) {

    hid_t       file_id, dataset_id;    /* identifiers */
    herr_t      status;
    int         rank, status_n;

    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;              /* dataset dimensions */
    hsize_t     *offset, *count;
    hsize_t     *dimsm;                 /* memoryspace dimensions */
        
    int         i;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);
    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }
    
    assert(dim == rank);
    
    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        offset[i]   = number[i];
        count[i]    = 1;
    }
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
    /*
     * Define the memory dataspace.
     */
    dimsm        = malloc(sizeof(int));
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    dimsm[0] = 1;
    memspace = H5Screate_simple(1, dimsm, NULL);

    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    if(H5T_NATIVE_LONG == H5Tget_class(datatype)){
        status = H5Dread(dataset_id, H5T_NATIVE_LONG, memspace, dataspace, H5P_DEFAULT, data_out_1D);  
        if(status<0.){
            fprintf(stderr, "Failed to read dataset in HDF5 file");
            exit(EXIT_FAILURE);
        }
    }

    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_item_from_dataset_int(char *file_name, char *dataset_name, int dim, int *number, void *data_out_1D) {

    hid_t       file_id, dataset_id;    /* identifiers */
    herr_t      status;
    int         rank, status_n;

    hid_t       datatype, dataspace;
    hid_t       memspace;
    
    hsize_t     *dims_out;              /* dataset dimensions */
    hsize_t     *offset, *count;
    hsize_t     *dimsm;                 /* memoryspace dimensions */
        
    int         i;

    /* Open an existing file. */
    file_id     = H5Fopen(file_name, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Open an existing dataset. */
    dataset_id  = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);
    datatype    = H5Dget_type(dataset_id);     /* datatype handle */
    dataspace   = H5Dget_space(dataset_id);    /* dataspace handle */
    rank        = H5Sget_simple_extent_ndims(dataspace);
    dims_out    = malloc(sizeof(int)*rank);
    if(dims_out == NULL) {
        fprintf(stderr,  "dimensions of HDF5 file");
        exit(EXIT_FAILURE);
    }
    status_n    = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    if(status_n<0) {
        fprintf(stderr, "Could not obtain dimension of dataset");
        
    }
    
    assert(dim == rank);
    
    /*
     * Define hyperslab in the dataset.
     */
    offset      = malloc(sizeof(int)*rank);
    if(offset == NULL) {
        fprintf(stderr,  "offset array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    count       = malloc(sizeof(int)*rank);
    if(count == NULL) {
        fprintf(stderr,  "count array for reading HDF5 file");
        exit(EXIT_FAILURE);
    }
    for(i=0; i<rank; i++)
    {
        offset[i]   = number[i];
        count[i]    = 1;
    }
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
    if(status<0.){
        fprintf(stderr, "Failed to select hyperslab HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    /*
     * Define the memory dataspace.
     */
    dimsm        = malloc(sizeof(int));
    if(dimsm == NULL) {
        fprintf(stderr,  "dimensions of memory space needed to read HDF5 file");
        exit(EXIT_FAILURE);
    }
    dimsm[0] = 1;
    memspace = H5Screate_simple(1, dimsm, NULL);

    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    if(H5T_NATIVE_INT == H5Tget_class(datatype)) {
        status = H5Dread(dataset_id, H5T_NATIVE_INT, memspace, dataspace, H5P_DEFAULT, data_out_1D);
        if(status<0.){
            fprintf(stderr, "Failed to read dataset in HDF5 file");
            exit(EXIT_FAILURE);
        }
    }

    free(dims_out);
    free(offset);
    free(count);
    free(dimsm);
    status = H5Tclose(datatype);
    status = H5Sclose(dataspace);
    
    /* Close the dataset. */
    status = H5Dclose(dataset_id);

    /* Close the file. */
    status = H5Fclose(file_id);
    if(status<0.){
        fprintf(stderr, "Failed to close HDF5 file");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}

extern int 
read_attribute_double(char *file_name, char *groupname, char *attr_name, void *data) {

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
        if(H5T_FLOAT == H5Tget_class(attr_type)) {
            status = H5Aread(attr_id, H5T_NATIVE_DOUBLE, data);
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
            fprintf(stderr,  "dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        status = H5Sget_simple_extent_dims(attr_space, attr_dim, NULL);
        if(status<0.){
            fprintf(stderr, "Failed to get dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        
        if(H5T_FLOAT == H5Tget_class(attr_type))
        {
            status = H5Aread(attr_id, H5T_NATIVE_DOUBLE, data);
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


extern int 
read_attribute_double_array(char *file_name, char *groupname, char *attr_name, void *data, int32_t array_size) {

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
        if(H5T_FLOAT == H5Tget_class(attr_type)) {
            status = H5Aread(attr_id, H5T_NATIVE_DOUBLE, data);
            status = H5Aread(attr_id, H5T_NATIVE_INT, data);
            if(status<0.){
                fprintf(stderr, "Failed to read attribute in HDF5 file");
                exit(EXIT_FAILURE);
            }
        }
    }
    else if(attr_rank == 1)
    {
        attr_dim = malloc(sizeof(int)*attr_rank * array_size);
        if(attr_dim == NULL) {
            fprintf(stderr,  "dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        status = H5Sget_simple_extent_dims(attr_space, attr_dim, NULL);
        if(status<0.){
            fprintf(stderr, "Failed to get dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        
        if(H5T_FLOAT == H5Tget_class(attr_type))
        {
            status = H5Aread(attr_id, H5T_NATIVE_DOUBLE, data);
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

extern int 
read_attribute_float(char *file_name, char *groupname, char *attr_name, void *data) {

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
        if(H5T_FLOAT == H5Tget_class(attr_type)) {
            status = H5Aread(attr_id, H5T_NATIVE_FLOAT, data);
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
            fprintf(stderr,  "dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        status = H5Sget_simple_extent_dims(attr_space, attr_dim, NULL);
        if(status<0.){
            fprintf(stderr, "Failed to get dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        
        if(H5T_FLOAT == H5Tget_class(attr_type))
        {
            status = H5Aread(attr_id, H5T_NATIVE_FLOAT, data);
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
            fprintf(stderr,  "dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        status = H5Sget_simple_extent_dims(attr_space, attr_dim, NULL);
        if(status<0.){
            fprintf(stderr, "Failed to get dimensions of attribute in HDF5 file");
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


extern int 
read_attribute_int_array(char *file_name, char *groupname, char *attr_name, void *data, int32_t array_size) {

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
        attr_dim = malloc(sizeof(int)*attr_rank * array_size);
        if(attr_dim == NULL) {
            fprintf(stderr,  "dimensions of attribute in HDF5 file");
            exit(EXIT_FAILURE);
        }
        status = H5Sget_simple_extent_dims(attr_space, attr_dim, NULL);
        if(status<0.){
            fprintf(stderr, "Failed to get dimensions of attribute in HDF5 file");
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

