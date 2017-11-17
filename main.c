#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef MPI
#include <mpi.h>
#endif

#include "io.h"
#include "grid.h"


// Local Variables //
uint32_t GridSize;
char *finbase;
char *foutbase;
uint32_t num_files;

// Proto-types //

void parse_params(int argc, char **argv);
void init(grid_t *grid);


// Functions //

void parse_params(int argc, char **argv)
{

  if (argc != 4)
  {
    fprintf(stderr, "This is my version of densfield, tailored for usage with GADGET HDF5 files where a single file can (comfortably) fit in RAM.\n");
    fprintf(stderr, "Usage : ./densgrid <GridSize> <Input Snapshot Base> <Output Base>\n");
    fprintf(stderr, "The name format for the input snapshot is assumed to be '<Input Snapshot Base>.%%file_index%%.hdf5'\n");
    exit(0);
  }

  GridSize = atoi(argv[1]);
  if (GridSize < 64 || GridSize > 2048)
  {
    fprintf(stderr, "The GridSize must be between 64 and 2048\n"); 
    exit(0);
  }
  finbase = strdup(argv[2]);
  foutbase = strdup(argv[3]);
  printf("==================================================\n");
  printf("Running with a GridSize %d, input file base %s and output file base %s\n", GridSize, finbase, foutbase); 
  printf("==================================================\n\n");
}

void init(grid_t *grid)
{

  *grid = init_grid(GridSize);
  
  num_files = get_numfiles(finbase);

}

int main(int argc, char **argv)
{

  grid_t grid_local;

  parse_params(argc, argv); // Set the input parameters. 
  init(&grid_local); // Initialize parameters.  
  free_localgrid(&grid_local);

  free(finbase);
  free(foutbase); 
  return 0;
  
} 
