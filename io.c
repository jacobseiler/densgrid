#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "io.h"

// Proto-types //
void read_header(char *fname, hdf5_header file_header);


// Functions //

// This function accepts an empty (but malloced) header struct to be read and filled.
void read_header(char *fname, hdf5_header file_header)
{
  if (file_header == NULL)
  {
    fprintf(stderr, "read_header was called but the header struct has not yet been malloced\n");
    exit(EXIT_FAILURE);
  }



}

uint32_t get_numfiles(char *finbase)
{

  char buf[1024];
  hdf5_header file_header; 

  snprintf(buf, 1024, "%s.0.hdf5", finbase);
  printf("Reading header from file %s\n", buf);

  file_header = malloc(sizeof(struct hdf5_header_struct));
  if (file_header == NULL)
  {
    fprintf(stderr, "Could not allocate memory for header\n");
    exit(EXIT_FAILURE);
  }

  read_header(buf, file_header); // Passes the header struct to be filled.
 

  free(file_header); 
  return 7; 

}


